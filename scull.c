#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/types.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include "scull.h"

MODULE_LICENSE("GPL");
/*for scull0->scull3 and scullpipe0->scullpipe3*/
#define pr_dbg(str, ...) printk("[DEBUG:(%s:%d)]" str "\n", __FUNCTION__, __LINE__, ##__VA_ARGS__);
#define FSTART pr_dbg("Start")
#define FSTOP	pr_dbg("Stop")
/*SECTION module parameters*/

int scull_trim(struct scull_dev *dev)
{
	struct scull_qset *next, *dptr;
	int qset = dev->qset;	/* "dev" is not-null */
	int i;

	for (dptr = dev->data; dptr; dptr = next) {	/* all the list items */
		if (dptr->data) {
			for (i = 0; i < qset; i++)
				kfree(dptr->data[i]);
			kfree(dptr->data);
			dptr->data = NULL;
		}
		next = dptr->next;
		kfree(dptr);
	}
	dev->size = 0;
	dev->quantum = 0;
	dev->qset = 0;
	dev->data = NULL;
	return 0;
}

static void scull_qset_free(struct scull_qset *qset)
{

	int i;
	FSTART;
	
	if (!qset){
		pr_dbg("qset equals to 0");
		FSTOP;
		return;
	}

	if (!qset->data){
		pr_dbg("qset->data equals to 0");
		goto fqset;
	}

	for (i = 0; i < SCULL_MAX_DATA_COUNT; i++) {
		
		if(!qset->data[i]){
			pr_dbg("qset->data[%d] equals 0", i);
			continue;
		}
		pr_dbg("freeing qset->data[%d]", i);
		kfree(qset->data[i]);
		
	}

	kfree(qset->data);
	qset->data = 0;
 fqset:
	kfree(qset);
	qset = 0;
	FSTOP;
}

static int scull_dev_free(struct scull_dev *dev)
{
	int i;
	struct scull_qset *next, *data;
	
	if (!dev) {
		FSTOP;
		return 0;
	}

	if (!dev->data)
		goto fscull;
	data = dev->data;
	for (i = 0; data; data = next) {

		next = data->next;
		scull_qset_free(data);
	}

 fscull:
	kfree(dev);
	FSTOP;
	return 0;
}

static struct scull_qset *scull_qset_init(void)
{
	int i;
	struct scull_qset *qset;
	FSTART;

	qset = kmalloc(sizeof(struct scull_qset), GFP_KERNEL);
	if (!qset){
		pr_err("failed to allocate qset pointer");
		return 0;
		
	}
	memset(qset, 0, sizeof(struct scull_qset));
	
	qset->data = kmalloc(SCULL_MAX_DATA_COUNT * sizeof(void *), GFP_KERNEL);
	if (!qset->data) {
		pr_err("failed to allocate qset->data");
		goto free_qset;
	}
	memset(qset->data, 0, SCULL_MAX_DATA_COUNT * sizeof(void *));
		
	for (i = 0; i < SCULL_MAX_DATA_COUNT; i++) {
		
		pr_dbg("allocating qset->data[%d]", i);
		qset->data[i] = kmalloc(SCULL_MAX_DATA * sizeof(void* ), GFP_KERNEL);
		if (!qset->data[i]) {	/*deinitialise all allocated memory */
			
			pr_err("failed to allocate qset->data[%d]", i);
			
			for (i--; i >= 0; i--){
				if(!qset->data[i])
					continue;	/*just in case*/
				kfree(qset->data[i]);
			}
			
			goto free_data;
		}
		memset(qset->data[i], 0, SCULL_MAX_DATA);

	}
	FSTOP;
	return qset;

 free_data:
	kfree(qset->data);
 free_qset:
	kfree(qset);
	FSTOP;
	return 0;

}

static struct scull_dev *scull_dev_init(void)
{
	struct scull_dev *dev = NULL;
	FSTART;

	/*GSP_KERNEL - allocates data in behalf of kernel (can sleep) */
	dev = kmalloc(sizeof(struct scull_dev), GFP_KERNEL);
	if (!dev) {
		FSTOP;
		return 0;
	}
	memset(dev, 0, sizeof(struct scull_dev));
	/*initialise quantum */
	dev->data = scull_qset_init();
	if (!dev->data) {
		scull_dev_free(dev);
		return 0;
	}
	dev->quantum = SCULL_MAX_DATA * SCULL_MAX_DATA_COUNT;
	dev->qset = 1;
	dev->size = 0;
	dev->access_key = 0;
	mutex_init(&dev->sem);
	FSTOP;
	return dev;
}

static int scull_add_qset(struct scull_dev *dev)
{
	//TOFO: implement       
	return 0;
}

/*SECTION global variables*/
dev_t device;
struct scull_dev *scull_dev0 = NULL;
int dev_scull0_maj_min;
bool dev_scull0_status = 0;
struct scull_dev *scull_dev;

 /**/
/*SECTION set device file methods*/
static loff_t scull_llseek(struct file *f, loff_t offeset, int whence)
{
	return 0;
}

static ssize_t scull_read(struct file *filep, char __user * user_buf,
			  size_t size, loff_t * pos)
{

	int err;
	struct scull_dev *dev = filep->private_data;
	uint res_size;
	printk("dev=%p, pos=%d, size=%d\n", dev, *pos, size);
	
	if (!size)
		return 0;

	if (size > dev->size)
		res_size = dev->size;
	else
		res_size = size;
	pr_dbg("filep->private_data=%p", filep->private_data);
	pr_dbg("filep->private_data->data=%p", ((struct scull_dev *)filep->private_data)->data);
	err = copy_to_user(user_buf, dev->data->data[0], res_size);
	if(err){
		pr_err("failed to cuoy buffer to userspace");
	}

	return res_size;
}

static ssize_t scull_write(struct file *f, const char __user * a, size_t b,
			   loff_t * c)
{
	return 0;
}

static int scull_open(struct inode *inodep, struct file *filep)
{
	/*1. check device-specific errors (device not ready ...)
	 *2. init device, if it is opened first time
	 *3. update f_op pointer if necessary
	 *4. allocate and fill aney data to be put in file->private data
	 */
	struct scull_dev *dev;
	FSTART;

	/*no errors to check, as we didn's have any device */

	/*no device to initialise */

	/*no need to update f_op ponter */

	/*allocating device data */
	/*initialise new quantum, if previous is already full */

	/*check if device is opened for first time */

	dev = container_of(inodep->i_cdev, struct scull_dev, cdev);
	pr_dbg("dev=%p", dev);
	pr_dbg("dev->data=%p", dev->data);
	/*Why not to initialise with pointer of scull_dev0? scull_dev0->cdev and inode->dev are different instances? This is
	 * vaste of memory!*/
	
	filep->private_data = dev;
	FSTOP;
	return 0;
}

static int scull_release(struct inode *inodep, struct file *filep)
{
	/*
	 * deallocate all resource, allocated in open
	 * close hardvare device after last close
	 */
	/*scull douen't have any hardvare, so just return */
	return 0;
}

struct file_operations scull0_fops = {
	.owner = THIS_MODULE,
	.llseek = scull_llseek,
	.read = scull_read,
	.write = scull_write,
/*TODO: implement function*/
//      .unlock_ioctl   = scull_unlock_ioctl,
	.open = scull_open,
	.release = scull_release
};

/*SECTION local static functions*/
static int release_resources(void)
{

	/*release resource sequence */

	/*unregister device */
	if (dev_scull0_status != 0) {

		if (scull_dev0->cdev.kobj.state_initialized) {
			cdev_del(&scull_dev0->cdev);
		}

		unregister_chrdev_region(device, SCULL_CHARDEV_COUNT);
	}

	if (scull_dev0) {
		scull_trim(scull_dev0);
		scull_dev0 = 0;
	}

	return 0;
}

static int scull_setup_cdev(void)
{
	int err = 0;
	FSTART;

	scull_dev0 = scull_dev_init();
	pr_dbg("skull_dev0=%p", scull_dev0);
	pr_dbg("scull_dev0->data=%p",scull_dev0->data);
	if (!scull_dev0) {
		pr_err("cannot allocate scull_dev0");
		return -1;
	}
	printk("memset succeed\n");

	/*TODO: initialise mutex correctly */
	//mutex_init(scull_dev0->sem);
	//printk("inited sucessfull, scull_dev0=%p", scull_dev0);

	/*major/minor nubmer regions */
	err = alloc_chrdev_region(&device,
				  SCULL_MINOR_NUMBER,
				  SCULL_CHARDEV_COUNT, "skull-dev");
	if (err != 0) {
		pr_err("allocation major number region for device failed\n");
		return -1;
	}
	dev_scull0_maj_min = MKDEV(SCULL0_MAJOR(device), SCULL0_MINOR(device));
	pr_info("major/minor regions iniialised sucessfully\n");

	/*allocate device scull0 */

	cdev_init(&scull_dev0->cdev, &scull0_fops);
	printk("rev_scull0 initialised sucessfull");

	err = cdev_add(&scull_dev0->cdev, dev_scull0_maj_min, 1);
	if (err) {
		pr_err("cannot add device to a kernel structurei, err=%d\n",
		       err);
		cdev_del(&scull_dev0->cdev);	/*Can it break CO? */
		unregister_chrdev_region(device, SCULL_CHARDEV_COUNT);
		return -1;
	}
	pr_info("dev_scull0 initialised sucessfully, major = %i, minor = %i\n",
		MAJOR(dev_scull0_maj_min), MINOR(dev_scull0_maj_min));

	FSTOP;
	return 0;
}

#define _DEBUG_SECTION
#ifndef _DEBUG_SECTION  
/*init functions*/
static int __init scull_init(void)
{
	int err = 0;

	/*allocating resources */
	err = scull_setup_cdev();
	if (err)
		return -1;
	dev_scull0_status = 1;

	return 0;
}

module_init(scull_init);

static void __exit scull_exit(void)
{
	release_resources();
}

module_exit(scull_exit)
#else

struct scull_qset* test_qset;

static int __init scull_init(void)
{

	test_qset = scull_qset_init();
	return 0;
}

module_init(scull_init);

static void __exit scull_exit(void)
{
	scull_qset_free(test_qset);
}

module_exit(scull_exit)
#endif
/*symbols*/
/*EXPORT_SYMBOL_GPL*/
