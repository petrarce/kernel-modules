#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/types.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include "reverce.h"

MODULE_LICENSE("GPL");
/*for reverce0->reverce3 and revercepipe0->revercepipe3*/
#define pr_dbg(str, ...) printk("[DEBUG:(%s:%d)]" str "\n", __FUNCTION__, __LINE__, ##__VA_ARGS__);
#define FSTART pr_dbg("Start")
#define FSTOP	pr_dbg("Stop")
/*SECTION module parameters*/


/*SECTION driver resources*/
struct 	proc_dir_entry *proc_entry;
char* 	buf = NULL;
uint32_t buf_size = 0;
/*SECTION set device file methods*/

static void reverse_buf(char* buf, uint32_t buf_size)
{
	uint32_t i;
	char tmp_symb;

	for(i = 0; i < (uint32_t)(buf_size/2); i++){
		tmp_symb = buf[i];
		buf[i] = buf[buf_size - i - 1];
		buf[buf_size - i - 1] = tmp_symb;
	}
}

static ssize_t reverce_read(struct file *f, char __user *user_buf, size_t size, loff_t *offset)
{
	if(*offset == buf_size){
		*offset = 0;
		return 0;
	}

	copy_to_user(user_buf, buf, buf_size);
	*offset = buf_size;

	return buf_size;
}

static ssize_t reverce_write(struct file *filep, const char __user * user_buf, size_t size,
			   loff_t * offset)
{

	if(buf){
		pr_dbg("freing previous buffer");
		kfree(buf);
		buf = NULL;
		buf_size = 0;
	}

	buf = kzalloc(size,GFP_KERNEL); //allocate buffer with size+1 for null terminated string
	buf_size = size;

	copy_from_user(buf, user_buf, buf_size);
	reverse_buf(buf, buf_size-1);

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
	if(buf){
		kfree(buf);
		buf = 0;
		buf_size = 0;
	}
	return 0;
}

static int reverce_alloc_res(void)
{

	proc_entry = proc_create("reverse", 0644, NULL, &reverce_fops);
	if(!proc_entry)
		return -1;

	return 0;
}


/*init functions*/
static int __init reverce_init(void)
{
	pr_dbg("start mod");
	return reverce_alloc_res();
}

module_init(reverce_init);

static void __exit reverce_exit(void)
{
	reverce_release_res();
}

module_exit(reverce_exit)
