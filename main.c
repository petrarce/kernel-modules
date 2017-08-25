#include <linux/module.h>
#include <linux/init.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/errno.h>


#define MY_DEV_COUNT 	4
#define MY_ARRAY_SIZE 	4

#define printk_dbg(str, ...) \
		printk("[DBG:%s:%d] " str "\n" , __FUNCTION__, __LINE__, ##__VA_ARGS__ )
/********************Module Parameters**********************/
/*
*alov parameter to be forvarded into module
*Next permitions are allowed:
	R - read
	W - write
	X - execute
*Next user instances allowed:
*	USR - user (only superuser)
*	GRP - grop (all users in sudo group)
* 	OTH - all users  (all users)
*	UGO - user, gorup and others
*use next makroses in thisrd parameter:
*	S_I<P><USR>
*	Where 
*		<P> - permission letter, specified abow
*		<USR> - user instance, specified abow
*/
int parameter_1 = 0;
int parameter_2 = 0;
int parameter_3 = 0;
uint parameter_4 = 0xffffffff;
module_param(parameter_1, int, S_IRUSR|S_IWUSR);
module_param(parameter_2, int, S_IRUSR);
module_param(parameter_3, int, S_IWUSR);
module_param(parameter_4, uint, 0);

/**********************MODULE VARIABLES**********************/
int* array_param[MY_ARRAY_SIZE] = 
{
	&parameter_1, 
	&parameter_2, 
	&parameter_3, 
	(int*) &parameter_4
};
dev_t first_dev = MKDEV(53,48);

/***************************MODULE INIT/EXIT FUNCTIONS*******/
/*
 *Use init __init macro to free virtual memory that was allocated for code of
 * this function after it will be executed
 */
static int __init hello_init(void)
{

	int err = 0;
	
	/*register a range of char devices in cernel*/
	err = alloc_chrdev_region( &first_dev, 48, parameter_1, "my-device");
	if (err != 0)
	{
	
		printk_dbg(KERN_ERR "cannot allocate major number for device in range of %i\n, retval = %d",
					MY_DEV_COUNT, 
					err);
	
	}
	
	printk("initilaisation done, minor=%i, major=%i\n", MINOR(first_dev), MAJOR(first_dev));

	return 0;
}
module_init(hello_init);

/*
 *__exit maks in cernel that function is for unlod only 
 *  functions, marked as __exit discarded IF module is built dirrectly in cernel
 * as builtin module will never exit, so no need to allocate memory for it
 */
static void __exit hello_exit(void)
{

 	unregister_chrdev_region( first_dev, MY_DEV_COUNT);    
    printk("by by!!!\n");
}
module_exit(hello_exit);

/************************MODULE SYMBOLS*****************************/
/*
 *Testing function
 */
void main_print_word(void)
{
	printk("[main_print_word] printing message: ");	
	hello_exit();
}
EXPORT_SYMBOL(main_print_word);

/***********************LOCAL FUNCTIONS*****************************/


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Ivan Folbort (petrarce@gmail.com)");
MODULE_DESCRIPTION("Testing module, wroten in educational purposes");
MODULE_VERSION("1.0");
MODULE_ALIAS("screaming-B");
