#include <linux/module.h> //kmod interface
#include <linux/version.h> //kernel version stamps
#include <linux/kernel.h> //resolve kernel symbols calls
#include <linux/init.h> //


void func(void);

int val = 300;

void func(void)
{
	printk("function invoked\n");
	printk("val = %d\n",val);
}

int init_mod(void)
{
	printk("module is inserted\n");
	return 0;
}

void cleanup_mod(void)
{
	printk("module is removed\n");
}

module_init(init_mod);
module_exit(cleanup_mod);

MODULE_AUTHOR("SHASHIDHAR");
MODULE_DESCRIPTION("FIRST MODULE");
MODULE_LICENSE("GPL");
