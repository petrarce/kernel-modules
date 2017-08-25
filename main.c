#include <linux/module.h>
#include <linux/init.h>

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
*	UGO - user, gorup and others
*use next makroses in thisrd parameter:
*	S_I<P><USR>
*	Where 
*		<P> - permission letter, specified abow
*		<USR> - user instance, specified abow
*/
int parameter_1 = 0;
int parameter_2 = 0;
int parameter_3 = 0;
int parameter_4 = 0;
module_param(parameter_1, int, S_IRUSR|S_IWUSR);
module_param(parameter_2, int, S_IRUSR);
module_param(parameter_3, int, S_IWUSR);
module_param(parameter_4, int, 0);

#define MY_ARRAY_SIZE 4
int* array_param[MY_ARRAY_SIZE] = {&parameter_1, &parameter_2, &parameter_3, &parameter_4};



/*
 *Use init __init macro to free virtual memory that was allocated for code of
 * this function after it will be executed
 */
static int __init hello_init(void){

	unsigned char i;
	for(i = 0; i < MY_ARRAY_SIZE; i++)
	{
		printk("parameter_%i = %d", i, *array_param[i]);
	}
	printk("initilaisation done");

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
