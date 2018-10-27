#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/kernel.h>
#include <linux/kprobes.h>
#include <linux/ptrace.h>
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

char* symb_name 	= "increase_counter";
module_param(symb_name, charp, 0);

int kprobe_test_pre_count 	= 0;
int kprobe_test_post_count	= 0;



static int kprobe_test_pre(struct kprobe* kp, struct pt_regs* regs)
{
	kprobe_test_pre_count++;
	//pr_dbg("%s is going to be executed", kp->symbol_name);
	return 0;
}

static void kprobe_test_post(struct kprobe* kp, struct pt_regs* regs, unsigned long flags)
{
	kprobe_test_post_count++;
	//pr_dbg("%s started execution", kp->symbol_name);
}

static struct kprobe kp = {
	.symbol_name 	= "reverse_write",
	.pre_handler 	= kprobe_test_pre,
	.post_handler	= kprobe_test_post
};


int __init kprobe_test_init(void)
{
	kp.symbol_name = symb_name;
	pr_dbg("symb_name=%s", symb_name);
	register_kprobe(&kp);

	return 0;

}

void __exit kprobe_test_exite(void)
{
	pr_dbg("kprobe_test_pre_count=%d, kprobe_test_post_count=%d", kprobe_test_pre_count, kprobe_test_post_count);
	unregister_kprobe(&kp);
}

module_init(kprobe_test_init);
module_exit(kprobe_test_exite);