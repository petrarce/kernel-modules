#include <linux/module.h>
#include <linux/init.h>

MODULE_LICENSE("GPL");

//int hello_init(void);
//int hello_exit(void);

static int hello_init(void){
    printk("this %s will %s some %s\n", "message", "contain", "strings");
	return 0;
}

static void hello_exit(void){
    
    printk("by by!!!\n");
}

module_init(hello_init);
module_exit(hello_exit);
