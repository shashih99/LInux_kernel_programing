#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/cdev.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <linux/uaccess.h>

#define MAJORNO         300
#define MINORNO           0
#define CHAR_DEV_NAME   "shash_cdriver"
#define SUCCESS           0
#define mem_size        1024

dev_t dev=0;
struct cdev *my_cdev;
struct class *dev_class;
uint8_t *kernel_buffer;
static int char_dev_open(struct inode * inode,struct file *file)
{
	/*creating physical memory*/
	if((kernel_buffer = kmalloc(mem_size,GFP_KERNEL)) == 0)
	{
		printk(KERN_INFO "Cannot allocate the memory to the kernel");
		return -1;
	}	
	return SUCCESS;
}

static int char_dev_release(struct inode *inode,struct file *file)
{
	/*freeing the memory*/
	kfree(kernel_buffer);
	printk(KERN_INFO "Device file cloased...\n");
	return 0;
}
static ssize_t char_dev_read(struct file* file,char __user*user_buf,size_t size,loff_t* ppos)
{
#if 0
	ssize_t nbytes = size - copy_to_user(user_buf,kernel_buffer + *ppos,size);
	*ppos += nbytes;
	printk(KERN_INFO "Reading nbytes = %ld,pos = %lld\n",nbytes,(loff_t)*ppos);
	return nbytes;
#endif
	copy_to_user(user_buf,kernel_buffer,mem_size);
	printk(KERN_INFO "Data read : DONE....\n");
	return mem_size;
}

static ssize_t char_dev_write(struct file*file,const char __user*user_buf,size_t size,loff_t*ppos)
{
#if 0
	ssize_t nbytes = size - copy_from_user(kernel_buffer + *ppos,user_buf,size);
        *ppos += nbytes;
        printk(KERN_INFO "Writing nbytes = %ld,pos = %lld\n",nbytes,(loff_t)*ppos);
	return nbytes;
#endif 
	copy_from_user(kernel_buffer,user_buf,size);
	printk("Data is written successfully...\n");
	return size;
}
static struct file_operations fops = {
	.owner = THIS_MODULE,
	.read = char_dev_read,
	.write = char_dev_write,
	.release = char_dev_release,
	.open = char_dev_open
};	
static int __init char_dev_init(void)
{
	/*Allocate the major and minor number*/

	if(alloc_chrdev_region(&dev,0,1,CHAR_DEV_NAME) < 0)
	{
		printk(KERN_INFO "Cannot allocate the device\n");
		return -1;
	}

	printk(KERN_INFO "MAJOR NO: %d and MINOR no: %d\n",MAJOR(dev),MINOR(dev));
	/*cdev structure*/
	cdev_init(my_cdev,&fops);

	my_cdev->owner = THIS_MODULE;
	/*adding cdev to the system*/
	if(cdev_add(my_cdev,dev,1) < 0)
	{
		printk(KERN_INFO "Cannot add device to the system...\n");
		goto r_class;
	}

	/*create the class*/
	if((dev_class = class_create(THIS_MODULE,"VIRTUAL")) == NULL)
	{
		printk(KERN_INFO "Unable to create the class..\n");
		goto r_class;
	}

	/*create the device*/
	if((device_create(dev_class,NULL,dev,NULL,CHAR_DEV_NAME)) == NULL)
	{
		printk(KERN_INFO "Unable to create the device..\n");
		goto r_device;
	}

	printk(KERN_INFO "Device driver inserted... done properly..\n");
	return 0;

r_device:
	class_destroy(dev_class);
	return -1;

r_class:
	unregister_chrdev_region(dev,1);
	return -1;

}

static void __exit char_dev_exit(void)
{
	device_destroy(dev_class,dev);
	class_destroy(dev_class);
	cdev_del(my_cdev);
	unregister_chrdev_region(dev,1);
	printk(KERN_INFO"Device driver removed...\n");
}

module_init(char_dev_init);
module_exit(char_dev_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("shashidhar hegde:shashidharh99@gmail.com");
MODULE_DESCRIPTION("CHARACTER DEVICE DRIVER");
