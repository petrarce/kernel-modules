#include <linux/module.h>
#include <linux/init.h>



/*
 *Use init __init macro to free virtual memory that was allocated for code of
 * this function after it will be executed
 */
static int __init hello_init(void){
    printk("this %s will %s some %s\n", "message", "contain", "strings");
	return 0;
}
module_init(hello_init);

/*
 *__exit maks in cernel that function is for unlod only 
 *  functions, marked as __exit discarded IF module is built dirrectly in cernel
 * as builtin module will never exit, so no need to allocate memory for it
 */
static void __exit hello_exit(void){
    
    printk("by by!!!\n");
}
module_exit(hello_exit);

/*
 *Testing function
 */
void main_print_word(void)
{
	printk("[main_print_word] printing message: ");	
	hello_exit();
}
EXPORT_SYMBOL(main_print_word);


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Ivan Folbort (petrarce@gmail.com)");
MODULE_DESCRIPTION("Testing module, wroten in educational purposes");
MODULE_VERSION("1.0");
MODULE_ALIAS("screaming-B");
