#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/types.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/kthread.h>

MODULE_LICENSE("GPL");

#define DEBUG
#ifdef DEBUG
#define pr_dbg(str, ...) printk("[DEBUG:(%s:%d)]" str "\n", __FUNCTION__, __LINE__, ##__VA_ARGS__);
#define FSTART pr_dbg("Start")
#define FSTOP	pr_dbg("Stop")
#else
#define pr_dbg(str, ...)
#define FSTART
#define FSTOP
#endif

/*SECTION module parameters*/


/*SECTION driver resources*/
struct 	proc_dir_entry *proc_entry;
struct buf_t{
	char* 	buf;
	uint32_t buf_size;
} bufobj = {.buf = NULL, .buf_size = 0};

/*SECTION set device file methods*/
inline void deinit_buf(struct buf_t *bufobj)
{

	kfree(bufobj->buf);
	bufobj->buf 		= NULL;
	bufobj->buf_size 	= 0;
}	

inline int8_t init_buf(struct buf_t *bufobj, uint32_t size)
{
	if(bufobj->buf)
		deinit_buf(bufobj);

	bufobj->buf = kzalloc(size,GFP_KERNEL);
	if(!bufobj->buf){
		pr_err("failed to allocate memory");
		return -ENOMEM;
	}
	bufobj->buf_size = size;
	return 0;

}


static void reverse_buf(char* buf, uint32_t buf_size)
{
	uint32_t i;
	char tmp_symb;

	if(!buf)
		return;

	for(i = 0; i < (uint32_t)(buf_size/2); i++){
		tmp_symb = buf[i];
		buf[i] = buf[buf_size - i - 1];
		buf[buf_size - i - 1] = tmp_symb;
	}
}

ssize_t reverse_read(struct file *f, char __user *user_buf, size_t size, loff_t *offset)
{

	uint32_t ret = 0;

	if(*offset == bufobj.buf_size){
		*offset = 0;
		return 0;
	}

	if(size > bufobj.buf_size - *offset)
		ret = bufobj.buf_size - *offset;
	else
		ret = size;

	copy_to_user(user_buf, bufobj.buf, ret);
	*offset += ret;

	return ret;
}

ssize_t reverse_write(struct file *filep, const char __user * user_buf, size_t size,
			   loff_t * offset)
{
	int32_t status;

	deinit_buf(&bufobj);
	status = init_buf(&bufobj, size);
	if(status)
		return status;

	copy_from_user(bufobj.buf, user_buf, bufobj.buf_size);
	/*decrease size to ignore null terminated symbol to be reversed*/
	reverse_buf(bufobj.buf, bufobj.buf_size-1);

	return size;
}


struct file_operations reverse_fops = {
	.owner 		= THIS_MODULE,
	.read 		= reverse_read,
	.write 		= reverse_write
};

/*SECTION local static functions*/
static int reverse_release_res(void)
{
	proc_remove(proc_entry);
	deinit_buf(&bufobj);
	return 0;
}

static int reverse_alloc_res(void)
{

	proc_entry = proc_create("reverse", 0644, NULL, &reverse_fops);
	if(!proc_entry){
		pr_err("failed to mount file in procfs");
		return -1;
	}

	return 0;
}

static struct task_struct *kt1, *kt2;

int kthread1(void *data)
{

	while(1){

		if(kthread_should_stop()){
			pr_dbg("stopping %s", __FUNCTION__);
			return 0;
		}

		if(printk_ratelimit())
			pr_dbg("%s thread is running", __FUNCTION__);

		schedule();
	}

	return -1;
}

int kthread2(void *data)
{

	while(1){

		if(kthread_should_stop()){
			pr_dbg("stopping %s", __FUNCTION__);
			return 0;
		}

		if(printk_ratelimit())
			pr_dbg("%s thread is running", __FUNCTION__);

	}

	return -1;
}



/*mod init/exit functions*/
int __init reverse_init(void)
{
	pr_dbg("mod start");

	kt1 = kthread_run(kthread1, NULL, "kthread1");
	kt2 = kthread_run(kthread2, NULL, "kthread2");
	return reverse_alloc_res();
}

module_init(reverse_init);

void __exit reverse_exit(void)
{
	reverse_release_res();
	kthread_stop(kt1);
	kthread_stop(kt2);
	pr_dbg("mod exit");
}

module_exit(reverse_exit);