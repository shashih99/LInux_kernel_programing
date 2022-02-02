#include <linux/module.h> //kmod interface
#include <linux/version.h> //kernel version stamps
#include <linux/kernel.h> //resolve kernel symbols calls
#include <linux/init.h> //


MODULE_LICENSE("GPL");

MODULE_AUTHOR("SHASHIDHAR");

MODULE_DESCRIPTION("FIRST MODULE");

MODULE_VERSION("0.1");
void func(void);

int val = 300;

#if 1
void func(void)
{
	printk("function invoked\n");
	printk("val = %d\n",val);
}
#endif
static int init_mod(void)
{
	printk("module is inserted\n");
	return 0;
}

static void cleanup_mod(void)
{
	printk("module is removed\n");
}

module_init(init_mod);
module_exit(cleanup_mod);

