#include <linux/module.h> //kmod interface
#include <linux/version.h> //kernel version stamps
#include <linux/kernel.h> //resolve kernel symbols calls
#include <linux/init.h> //
#include <linux/moduleparam.h>

void func(void);

static int val = 300;

//module_param(val,int,S_IRUGO);//read only
module_param(val,int,S_IRUGO|S_IWUSR);
MODULE_PARM_DESC(val, "INITIALISE ME AT INITIALISATION TIME");
MODULE_SUPPORTED_DEVICE("NULL");

void func(void)
{
	printk("function invoked\n");
	printk("val = %d\n",val);
}

void func1(void)
{
	printk("func1 invoked\n");
}
static int init_mod(void)
{
	printk("module is inserted\n");
	func();
	return 0;
}

static void cleanup_mod(void)
{
	printk("module is removed\n");
}

module_init(init_mod);
module_exit(cleanup_mod);

MODULE_LICENSE("DUAL BSD/GPL");
MODULE_AUTHOR("SHASHIDHAR");
MODULE_DESCRIPTION("FIRST MODULE");
MODULE_VERSION("0.1");
