#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/interrupt.h>

char tasklet_data[] = "We use a string; but it could be pointer to a structure";

/*Tasklet handler, that just print  the data*/
void tasklet_work(struct tasklet_struct *t)
{
    t->data = (unsigned long)tasklet_data;
    printk("%s \n",(char *)t->data);
}

DECLARE_TASKLET(my_tasklet, tasklet_work);


static int __init my_init(void)
{
    /*
     * Schedule the handler.
     * Tasklet are also scheduled from interrupt handler
     */
     tasklet_schedule(&my_tasklet);
     return 0;
}

static void my_exit(void)
{
    tasklet_kill(&my_tasklet);
}

module_init(my_init);
module_exit(my_exit);
MODULE_AUTHOR("Shashidhar hegde");
MODULE_LICENSE("GPL");
