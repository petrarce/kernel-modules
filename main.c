#include <linux/module.h>
#include <linux/init.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/reboot.h>

#define printk_dbg(str, ...) \
		printk("[DBG:%s:%d] " str "\n" , __FUNCTION__, __LINE__, ##__VA_ARGS__ )
		
/********************MODULE PROTOTYPES**********************/
int call_userspacehelper_wrapper(struct notifier_block *nb,
			unsigned long action, void *data);
			

/********************Module Parameters**********************/
/*specify path to script. which should be 
 * called from userspace before reboot*/
static char* usrmod_helper_path = NULL;
/*specify parameters of script, that should be passed*/
/*TODO: integrate parameters parsing and asygning*/
static char* usrmod_helper_parameters = NULL;
module_param(usrmod_helper_path, charp, S_IRUSR|S_IWUSR);
/*TODO integrate userspace parameters parsing*/
module_param(usrmod_helper_parameters, charp, S_IRUSR|S_IWUSR);

/**********************MODULE VARIABLES**********************/
static char* usrmod_helper_argv[2] = { [0 ... 1] = NULL };
static struct notifier_block nb = {
	.notifier_call = call_userspacehelper_wrapper
};

/**********************MODULE FUNCTIONS**********************/
static int call_userspacehelper_wrapper(struct notifier_block *nb,
			unsigned long action, void *data)
{
	/*TODO: add usermod-helper parameters module parameters*/
	printk_dbg("Notification called");
	usrmod_helper_argv[0] = usrmod_helper_path;
	call_usermodehelper(usrmod_helper_path, usrmod_helper_argv, NULL, UMH_NO_WAIT);
	return 0;
}

/***************************MODULE INIT/EXIT FUNCTIONS*******/

static int __init hello_init(void)
{
	register_reboot_notifier(&nb);
    printk("Reboot notifier registered Sucessfully");
	return 0;
}
module_init(hello_init);

static void __exit hello_exit(void)
{
	unregister_reboot_notifier(&nb);
    printk("Reboot notifier unregistered\n");
}
module_exit(hello_exit);

/************************MODULE SYMBOLS*****************************/


/***********************LOCAL FUNCTIONS*****************************/


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Ivan Folbort (petrarce@gmail.com)");
MODULE_DESCRIPTION("Testing module, wroten in educational purposes");
MODULE_VERSION("1.0");
MODULE_ALIAS("screaming-B");
