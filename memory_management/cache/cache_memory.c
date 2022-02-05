#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/module.h>

#define    SUCCESS      0

struct kmem_cache *cache_ptr;
int *ret;
int mem = 200;

void func(void)
{
    ret = kmem_cache_alloc(cache_ptr,GFP_KERNEL);
}

int init_mod(void)
{
    /*Alloc cache and make it resident*/
    cache_ptr = kmem_cache_create("cache_mem",mem,0,SLAB_HWCACHE_ALIGN,NULL);

    if(cache_ptr == NULL)
        return -ENOMEM;

    printk(KERN_INFO "Module loaded..\n");
    func();

    return SUCCESS;
}

void cleanup_mod(void)
{
    printk(KERN_INFO "Module removed..\n");
    kmem_cache_free(cache_ptr,ret);
    kmem_cache_destroy(cache_ptr);
}

module_init(init_mod);
module_exit(cleanup_mod);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Shashidhar hegde");