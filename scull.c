#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/types.h>
#include <linux/cdev.h>

/*for scull0->scull3 and scullpipe0->scullpipe3*/
#define SCULL_CHARDEV_COUNT		2
#define SCULL_MINOR_NUMBER		666
#define SCULL0_MAJOR(dev) 			MAJOR(dev)
#define SCULL0_MINOR(dev)			MINOR(dev+0)

/*SECTION module parameters*/

/*SECTION global variables*/
dev_t device;
struct cdev *dev_scull0 = NULL;
int dev_scull0_maj_min;
bool dev_scull0_status = 0;

/*SECTION local static functions*/

static int release_resources(void)
{

	/*release resource sequence */

	/*unregister device */
	if (dev_scull0_status != 0) {

		cdev_del(dev_scull0);
		unregister_chrdev_region(device, SCULL_CHARDEV_COUNT);
		pr_info("device deinitialised sucessfully\n");
	}

	return 0;
}

static int scull_setup_cdev()
{

	/*allocate device scull0 */
	cdev_init(dev_scull0, &scull0_fops);
	if (!dev_scull0) {
		pr_err("failed to allocate memory for device structure\n");
		/*DO WE NEED TO UNREGISTER IT? */
		unregister_chrdev_region(device, SCULL_CHARDEV_COUNT);
		return -1;
	}

	err = cdev_add(dev_scull0, dev_scull0_maj_min, 1);
	if (err) {
		pr_err("cannot add device to a kernel structurei, err=%d\n",
		       err);
		cdev_del(dev_scull0);	/*Can it break CO? */
		unregister_chrdev_region(device, SCULL_CHARDEV_COUNT);
		return -1;
	}
	pr_info("dev_scull0 initialised sucessfully\n");

	return 0;
}

static loff_t scull_llseek(struct file *f, loff_t offeset, int whence)
{
	return 0;
}

static ssize_t scull_read(struct file *f, char __user * a, size_t b, loff_t * c)
{
	return 0;
}

static ssize_t scull_write(struct file *f, const char __user * a, size_t b,
			   loff_t * c)
{
	return 0;
}

/*SECTION set device file methods*/
struct file_operations scull0_fops = {
	.owner = THIS_MODULE,
	.llseek = scull_llseek,
	.read = scull_read,
	.write = scull_write
/*TODO: implement function*/
//      .unlock_ioctl   = scull_unlock_ioctl,
//      .open                   = scull_open,
//      .release                = scull_release,
};

/*init functions*/
static int __init scull_init(void)
{
	int err = 0;

	/*allocating resources */

	/*major/minor nubmer regions */
	err = alloc_chrdev_region(&device,
				  SCULL_MINOR_NUMBER,
				  SCULL_CHARDEV_COUNT, "skull-dev");
	if (err != 0) {
		pr_err("allocation major number region for device failed\n");
		return -1;
	}
	pr_info("major/minor regions iniialised sucessfully\n");

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
/*symbols*/
/*EXPORT_SYMBOL_GPL*/
