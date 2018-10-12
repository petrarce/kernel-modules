#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/types.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>

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

ssize_t reverce_read(struct file *f, char __user *user_buf, size_t size, loff_t *offset)
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

ssize_t reverce_write(struct file *filep, const char __user * user_buf, size_t size,
			   loff_t * offset)
{
	int32_t status;

	deinit_buf(&bufobj);
	status = init_buf(&bufobj, size);
	if(status)
		return status;

	copy_from_user(bufobj.buf, user_buf, bufobj.buf_size);
	/*decrease size to ignore null terminated symbol to be reverced*/
	reverse_buf(bufobj.buf, bufobj.buf_size-1);

	return size;
}


struct file_operations reverce_fops = {
	.owner 		= THIS_MODULE,
	.read 		= reverce_read,
	.write 		= reverce_write
};

/*SECTION local static functions*/
static int reverce_release_res(void)
{
	proc_remove(proc_entry);
	deinit_buf(&bufobj);
	return 0;
}

static int reverce_alloc_res(void)
{

	proc_entry = proc_create("reverse", 0644, NULL, &reverce_fops);
	if(!proc_entry){
		pr_err("failed to mount file in procfs");
		return -1;
	}

	return 0;
}


/*mod init/exit functions*/
int __init reverce_init(void)
{
	pr_dbg("mod start");
	return reverce_alloc_res();
}

module_init(reverce_init);

void __exit reverce_exit(void)
{
	reverce_release_res();
	pr_dbg("mod exit");
}

module_exit(reverce_exit);