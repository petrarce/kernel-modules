#include <linux/module.h>
#include <linux/init.h>
#include "main.h"
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Ivan FOlbort (petrarce@gmail.com)");

static int __init secondary_init(void)
{

	main_print_word();
	printk("printing succeed");
	return 0;
}

static void secondary_exit(void)
{
	printk("exit module");	
}

module_init(secondary_init);
module_exit(secondary_exit);
