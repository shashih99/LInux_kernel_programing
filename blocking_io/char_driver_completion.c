/*
*Techinque one is using semaphore 
*
*/


#include <linux/module.h>
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include<linux/slab.h>
#include <linux/types.h>
#include <linux/semaphore.h>
#include <linux/completion.h>
//#include <linux/mutex.h>

//#define init_MUTEX_LOCKED (SEM)  sema_init (SEM, 0)
#define   SUCCESS         0
#define   MAX_LENGTH      4000
static dev_t first; // Global variable for the first device number
static struct cdev c_dev; // Global variable for the character device structure
static struct class *cl; // Global variable for the device class
static struct semaphore mysem;
static char *char_device_buf;
static DECLARE_COMPLETION (my_wait);

static int chardev_open(struct inode *i, struct file *f)
{
    static int counter = 0;
    counter++;
	
	printk(KERN_INFO "Driver: open()\n");
	printk(KERN_INFO "Number of times open() was called:%d\n",counter);
    printk(KERN_INFO "Process id of the current process: %d\n",current->pid);
    printk(KERN_INFO "ref = %d\n",module_refcount(THIS_MODULE));
	return SUCCESS;
}
static int chardev_close(struct inode *i, struct file *f)
{
	printk(KERN_INFO "Driver: close()\n");
	return SUCCESS;
}

static ssize_t chardev_read(struct file *f, char __user *buf, size_t len, loff_t *off)
{
	int maxbytes; //number of bytes from ppos to MAX_LENGTH
	int bytes_to_do; //number of bytes to read
	int nbytes; //number of bytes actually read

	maxbytes = MAX_LENGTH - *off;

	if(maxbytes > len ) 
		bytes_to_do = len;
	else
		bytes_to_do = maxbytes;
	
	wait_for_completion(&my_wait);
	printk(KERN_INFO "Driver: read()\n");
	nbytes = bytes_to_do - copy_to_user(buf,char_device_buf + *off, bytes_to_do);

	*off += nbytes;
	return nbytes;
}

static ssize_t chardev_write(struct file *f, const char __user *buf, size_t len,
		loff_t *off)
{
	int maxbytes; //number of bytes from ppos to MAX_LENGTH
	int bytes_to_do; //number of bytes to read
	int nbytes; //number of bytes actually read

	maxbytes = MAX_LENGTH - *off;

	if(maxbytes > len ) 
		bytes_to_do = len;
	else
		bytes_to_do = maxbytes;
	
	nbytes = bytes_to_do - copy_from_user(char_device_buf + *off,buf, bytes_to_do);
	*off += nbytes;
	complete (&my_wait);
	printk(KERN_INFO "Driver: write()\n");
	return nbytes;
}
static struct file_operations fops =
{
	.owner = THIS_MODULE,
	.open = chardev_open,
	.release = chardev_close,
	.read = chardev_read,
	.write = chardev_write,
};

static int __init chardev_init(void) /* Constructor */
{
    int ret;
    struct device *dev_ret;

    printk(KERN_INFO "Character driver initiated...\n");
    if ((ret = alloc_chrdev_region(&first, 0, 1, "character_dev")) < 0)
	{
		return ret;
	}

    if (IS_ERR(cl = class_create(THIS_MODULE, "chardrv")))
	{
		unregister_chrdev_region(first, 1);
		return PTR_ERR(cl);
	}

    if (IS_ERR(dev_ret = device_create(cl, NULL, first, NULL, "mynull")))
	{
		class_destroy(cl);
		unregister_chrdev_region(first, 1);
		return PTR_ERR(dev_ret);
	}

    cdev_init(&c_dev, &fops);
	if ((ret = cdev_add(&c_dev, first, 1)) < 0)
	{
		device_destroy(cl, first);
		class_destroy(cl);
		unregister_chrdev_region(first, 1);
		return ret;
	}
	printk(KERN_INFO "Major no:%d and Minor no:%d\n",MAJOR(first),MINOR(first));
	//init_MUTEX_LOCKED(&mysem); // default state is lock state
	sema_init(&mysem,1);
	char_device_buf = (char *)kmalloc(MAX_LENGTH,GFP_KERNEL);
	return 0;
}

static void __exit chardev_exit(void) /* Destructor */
{
	cdev_del(&c_dev);
	device_destroy(cl, first);
	class_destroy(cl);
	kfree(char_device_buf);
	unregister_chrdev_region(first, 1);
	printk(KERN_INFO "BYE: character driver unregistered\n");
}

module_init(chardev_init);
module_exit(chardev_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Shashidhar hegde");