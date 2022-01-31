#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/types.h>
#include<linux/slab.h>
#include <linux/kdev_t.h>
#include <linux/uaccess.h>
#define mem_size     1024
#define ENABLE_MALLOC

dev_t dev = 0;

uint8_t *kernel_buffer;
static struct cdev *chr_cdev;
static struct class *cl; // Global variable for the device class

static int chr_open(struct inode *inode, struct file *file)
{
#ifdef ENABLE_MALLOC
	if((kernel_buffer = kmalloc(mem_size,GFP_KERNEL)) == 0)
	{
		printk(KERN_INFO "Cannot aloocate the memory to the kernel..\n");
		return -1;
	}
#endif
        printk(KERN_INFO "Device File Opened...!!!\n");
        return 0;
}

static int chr_release(struct inode *inode, struct file *file)
{
#ifdef ENABLE_MALLOC 
	kfree(kernel_buffer);
#endif
        printk(KERN_INFO "Device File Closed...!!!\n");
        return 0;
}

static ssize_t chr_read(struct file *filp,char __user *buf, size_t len, loff_t *off)
{
#ifdef ENABLE_MALLOC
	copy_to_user(buf,kernel_buffer,mem_size);
#endif
        printk(KERN_INFO "Read function\n");
	return mem_size;
}

static ssize_t chr_write(struct file *filp,const char __user *buf, size_t len, loff_t *off)
{
#ifdef ENABLE_MALLOC
	copy_from_user(kernel_buffer,buf,len);
#endif
        printk(KERN_INFO "Write Function\n");
        return 0;
}

static struct file_operations fops =
{
        .owner          = THIS_MODULE,
        .read           = chr_read,
        .write          = chr_write,
        .open           = chr_open,
        .release        = chr_release,
};

static int __init char_dev_init(void)
{
	int ret;
	struct device *dev_ret;
	/*Allocating Major number*/
        if((alloc_chrdev_region(&dev, 0, 1, "Embetronicx")) <0){
                printk(KERN_INFO "Cannot allocate major number for device 1\n");
                return -1;
        }
	cdev_init(chr_cdev,&fops);	

	ret = cdev_add(chr_cdev,dev,1);

	if(ret < 0)
	{
		printk("ERROR regestering device driver\n");
		return -1;
	}

	if (IS_ERR(cl = class_create(THIS_MODULE, "chardrv11")))
    	{
        	unregister_chrdev_region(dev, 1);
        	return PTR_ERR(cl);
    	}

	if (IS_ERR(dev_ret = device_create(cl, NULL, dev, NULL, "mynull")))
    	{
        	class_destroy(cl);
        	unregister_chrdev_region(dev, 1);
    		return PTR_ERR(dev_ret);
    	}
	
	printk(KERN_INFO "Major = %d Minor = %d \n",MAJOR(dev), MINOR(dev));
	printk(KERN_INFO "Kernel Module Inserted Successfully...\n");
	return 0;
}

static void __exit char_dev_exit(void)
{
#ifdef DYNAMIC_ALLOCATION
	device_destroy(dev_class,dev);
        class_destroy(dev_class);
#endif
	cdev_del(chr_cdev);
        unregister_chrdev_region(dev, 1);
        printk(KERN_INFO "Device Driver Remove...Done!!!\n");
    printk(KERN_INFO "Kernel Module Removed Successfully...\n");
}

module_init(char_dev_init);
module_exit(char_dev_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("EmbeTronicX <embetronicx@gmail.com>");
MODULE_DESCRIPTION("Simple linux driver (Statically allocating the Major and Minor number)");
