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
#include <linux/uaccess.h>  //copy from user
#include<linux/slab.h> //kmalloc
#include <linux/types.h>
#include <linux/interrupt.h> 
#include <linux/kthread.h>
#include <linux/delay.h>




#define   IRQ_NO          1 

#define   SUCCESS         0
#define   MAX_LENGTH      4000
static dev_t first; // Global variable for the first device number
static struct cdev c_dev; // Global variable for the character device structure
static struct class *cl; // Global variable for the device class

static char *char_device_buf;
unsigned long chr_spinlock_variable = 0;
// unsigned int i=0;
// void tasklet_func(unsigned long data);
int thrd_func1(void *p);
int thrd_func2(void *p);
/*Declare the tasklet*/
// DECLARE_TASKLET(tasklet,tasklet_func,1);
DEFINE_SPINLOCK(chr_spinlock);

struct tasklet_struct *tasklet;
static struct task_struct *chr_thread1;
static struct task_struct *chr_thread2;

int thrd_func1(void *p)
{
    // int i = 0;
    while(!kthread_should_stop())
    {
		if(!spin_is_locked(&chr_spinlock))
		{
			printk(KERN_INFO "Spinlock is not locked in thread function1..\n");
		}
        spin_lock(&chr_spinlock);
		if(spin_is_locked(&chr_spinlock))
		{
			printk (KERN_INFO "spinlock is locked in thread function1..\n");
		}
		chr_spinlock_variable++;
		printk (KERN_INFO "Thread function 1: %lu..\n",chr_spinlock_variable);
		spin_unlock(&chr_spinlock);
        msleep(1000);
    }
    return 0; 
}

int thrd_func2(void *p)
{
	while(!kthread_should_stop())
    {
		if(!spin_is_locked(&chr_spinlock))
		{
			printk(KERN_INFO "Spinlock is not locked in thread function2..\n");
		}
        spin_lock(&chr_spinlock);
		if(spin_is_locked(&chr_spinlock))
		{
			printk (KERN_INFO "spinlock is locked in thread function2..\n");
		}
		chr_spinlock_variable++;
		printk (KERN_INFO "Thread function 2: %lu..\n",chr_spinlock_variable);
		spin_unlock(&chr_spinlock);
        msleep(1000);
    }
	return 0;
}

void tasklet_func(unsigned long data)
{
	static unsigned char scancode, status;
	spin_lock_irq(&chr_spinlock);
	chr_spinlock_variable++;
    status   = inb(0x64);
    scancode = inb(0x60);
    switch (scancode)
    {
		case 0x01:  printk (KERN_INFO "! You pressed Esc ...\n");
			break;
		case 0x3B:  printk (KERN_INFO "! You pressed F1 ...\n");
			break;
		case 0x3C:  printk (KERN_INFO "! You pressed F2 ...\n");
			break;
		default: break;
    }
	printk(KERN_INFO "Executing the tasklet function: data %ld and spinlock variable = %lu\n",data,chr_spinlock_variable);
spin_unlock_irq(&chr_spinlock);
}

static irqreturn_t irq_handler(int irq,void *dev_id)
{
	spin_lock_irq(&chr_spinlock);
	chr_spinlock_variable++;
    printk(KERN_INFO "keyboard: Interrupt occurred and executing ISR routine ...%lu\n",chr_spinlock_variable);
	spin_unlock_irq(&chr_spinlock);
    tasklet_schedule(tasklet);
    return IRQ_HANDLED;
}
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
	char_device_buf = (char *)kmalloc(MAX_LENGTH,GFP_KERNEL);
    if(request_irq(IRQ_NO,irq_handler,IRQF_SHARED,"my_device",(void *)(irq_handler)))
    {
        printk(KERN_INFO "my_device: Cannot register IRQ ");
        free_irq(IRQ_NO,(void *)(irq_handler));
    }
	tasklet = kmalloc(sizeof(struct tasklet_struct),GFP_KERNEL);
	if(tasklet == NULL)
	{
		printk(KERN_INFO "Cannot allocate the memory..\n");
		free_irq(IRQ_NO,(void *)(irq_handler));
	}
	tasklet_init(tasklet,tasklet_func,0);

#if 1
    chr_thread1 = kthread_create(thrd_func1,NULL,"chr thread1");

    if(chr_thread1)
    {
        wake_up_process(chr_thread1);
    }
    else
    {
        printk(KERN_INFO "Unable to create the thread 1..\n");
        class_destroy(cl);
    }
#endif
    chr_thread2 = kthread_run(thrd_func2,NULL,"chr thread2");
    if(chr_thread2)
        printk (KERN_INFO "Successfully created the kernel thread 2...\n"); 
    else
        printk(KERN_INFO "Unalbe to create the thread 2...\n");
    
    
    return 0;
}

static void __exit chardev_exit(void) /* Destructor */
{
    kthread_stop(chr_thread1);
	kthread_stop(chr_thread2);
    free_irq(IRQ_NO,(void *)(irq_handler));
	tasklet_kill(tasklet);
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