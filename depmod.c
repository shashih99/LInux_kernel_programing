#include <linux/module.h>
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/init.h>

extern void func(void);

static __init int myinit(void)
{
	printk("Calling kernel symbol\n");
	func();
	return 0;
}

static __exit void myexit(void)
{
	printk("cleanup invoked\n");
}

module_init(myinit);
module_exit(myexit);

MODULE_AUTHOR("SHASHI");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("DEP MODULE");

