#include <linux/init.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/vmalloc.h>
#include <linux/fs.h>
#include <linux/genhd.h>
#include <linux/blkdev.h>
#include <linux/buffer_head.h>
#include <linux/blk-mq.h>
#include <linux/hdreg.h>

#ifndef SECTOR_SIZE
#define SECTOR_SIZE 512
#endif

static int dev_major = 0;

/* Just internal representation of the our block device
 * can hold any useful data */
struct block_dev {
    sector_t capacity;
    u8 *data;   /* Data buffer to emulate real storage device */
    struct blk_mq_tag_set tag_set;
    struct request_queue *queue;
    struct gendisk *gdisk;
};

/* Device instance */
static struct block_dev *block_device = NULL;

static int blockdev_open(struct block_device *dev, fmode_t mode)
{
    printk(">>> blockdev_open\n");

    return 0;
}

static void blockdev_release(struct gendisk *gdisk, fmode_t mode)
{
    printk(">>> blockdev_release\n");
}

int blockdev_ioctl(struct block_device *bdev, fmode_t mode, unsigned cmd, unsigned long arg)
{
    printk("ioctl cmd 0x%08x\n", cmd);

    return -ENOTTY;
}

/* Set block device file I/O */
static struct block_device_operations blockdev_ops = {
    .owner = THIS_MODULE,
    .open = blockdev_open,
    .release = blockdev_release,
    .ioctl = blockdev_ioctl
};

/* Serve requests */
static int do_request(struct request *rq, unsigned int *nr_bytes)
{
    int ret = 0;
    struct bio_vec bvec;
    struct req_iterator iter;
    struct block_dev *dev = rq->q->queuedata;
    loff_t pos = blk_rq_pos(rq) << SECTOR_SHIFT;
    loff_t dev_size = (loff_t)(dev->capacity << SECTOR_SHIFT);

    printk(KERN_WARNING "sblkdev: request start from sector %lld  pos = %lld  dev_size = %lld\n", blk_rq_pos(rq), pos, dev_size);

    /* Iterate over all requests segments */
    rq_for_each_segment(bvec, rq, iter)
    {
        unsigned long b_len = bvec.bv_len;

        /* Get pointer to the data */
        void* b_buf = page_address(bvec.bv_page) + bvec.bv_offset;

        /* Simple check that we are not out of the memory bounds */
        if ((pos + b_len) > dev_size) {
            b_len = (unsigned long)(dev_size - pos);
        }

        if (rq_data_dir(rq) == WRITE) {
            /* Copy data to the buffer in to required position */
            printk("Writing to disk..\n");
            memcpy(dev->data + pos, b_buf, b_len);
        } else {
            /* Read data from the buffer's position */
            printk("READING from disk\n");
            memcpy(b_buf, dev->data + pos, b_len);
        }

        /* Increment counters */
        pos += b_len;
        *nr_bytes += b_len;
    }

    return ret;
}

/* queue callback function */
static blk_status_t queue_rq(struct blk_mq_hw_ctx *hctx, const struct blk_mq_queue_data* bd)
{
    unsigned int nr_bytes = 0;
    blk_status_t status = BLK_STS_OK;
    struct request *rq = bd->rq;

    /* Start request serving procedure */
    blk_mq_start_request(rq);

    if (do_request(rq, &nr_bytes) != 0) {
        status = BLK_STS_IOERR;
    }

    /* Notify kernel about processed nr_bytes */
    if (blk_update_request(rq, status, nr_bytes)) {
        /* Shouldn't fail */
        BUG();
    }

    /* Stop request serving procedure */
    __blk_mq_end_request(rq, status);

    return status;
}

static struct blk_mq_ops mq_ops = {
    .queue_rq = queue_rq,
};

static int __init myblock_driver_init(void)
{
    /* Register new block device and get device major number */
    dev_major = register_blkdev(dev_major, "testblk");

    block_device = kmalloc(sizeof (struct block_dev), GFP_KERNEL);

    if (block_device == NULL) {
        printk("Failed to allocate struct block_dev\n");
        unregister_blkdev(dev_major, "testblk");

        return -ENOMEM;
    }

    /* Set some random capacity of the device */
    block_device->capacity = (112 * PAGE_SIZE) >> 9; /* nsectors * SECTOR_SIZE; */
    /* Allocate corresponding data buffer */
    block_device->data = kmalloc(block_device->capacity << 9, GFP_KERNEL);

    if (block_device->data == NULL) {
        printk("Failed to allocate device IO buffer\n");
        unregister_blkdev(dev_major, "testblk");
        kfree(block_device);

        return -ENOMEM;
    }

    printk("Initializing queue\n");

    block_device->queue = blk_mq_init_sq_queue(&block_device->tag_set, &mq_ops, 128, BLK_MQ_F_SHOULD_MERGE);

    if (block_device->queue == NULL) {
        printk("Failed to allocate device queue\n");
        kfree(block_device->data);

        unregister_blkdev(dev_major, "testblk");
        kfree(block_device);

        return -ENOMEM;
    }

    /* Set driver's structure as user data of the queue */
    block_device->queue->queuedata = block_device;

    /* Allocate new disk */
    block_device->gdisk = alloc_disk(1);

    /* Set all required flags and data */
    block_device->gdisk->flags = GENHD_FL_NO_PART_SCAN;
    block_device->gdisk->major = dev_major;
    block_device->gdisk->first_minor = 0;

    block_device->gdisk->fops = &blockdev_ops;
    block_device->gdisk->queue = block_device->queue;
    block_device->gdisk->private_data = block_device;

    /* Set device name as it will be represented in /dev */
    strncpy(block_device->gdisk->disk_name, "blockdev\0", 9);

    printk("Adding disk %s\n", block_device->gdisk->disk_name);

    /* Set device capacity */
    set_capacity(block_device->gdisk, block_device->capacity);

    /* Notify kernel about new disk device */
    add_disk(block_device->gdisk);

    return 0;
}

static void __exit myblock_driver_exit(void)
{
    /* Don't forget to cleanup everything */
    if (block_device->gdisk) {
        del_gendisk(block_device->gdisk);
        put_disk(block_device->gdisk);
    }

    if (block_device->queue) {
        blk_cleanup_queue(block_device->queue);
    }

    kfree(block_device->data);

    unregister_blkdev(dev_major, "testblk");
    kfree(block_device);
}

module_init(myblock_driver_init);
module_exit(myblock_driver_exit);
MODULE_LICENSE("GPL");