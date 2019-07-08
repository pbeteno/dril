#include <linux/cdev.h>
#include <linux/errno.h>
#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/uaccess.h>


MODULE_AUTHOR("Paul Beneteau");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("MFRC522 driver");

struct mfrc522_dev {
        struct cdev cdev;
};

static int major;
static struct mfrc522_dev *rdev;

static long mfrc522_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
  	pr_info("start ioct");
	switch(cmd) {
		case 0:
			pr_info("ioctl0");
			break;
		case 1:
			pr_info("ioctl1");
			break;
	}
	return 0;
}

static struct file_operations mfrc522_fops = {
        .owner   = THIS_MODULE,
	.unlocked_ioctl = mfrc522_ioctl,
};


static int mfrc522_dev_init(struct mfrc522_dev **rdev) {

        *rdev = kmalloc(sizeof(*rdev), GFP_KERNEL);
	if (*rdev == NULL) {
                return -ENOMEM;
        }

        (*rdev)->cdev.owner = THIS_MODULE;
        cdev_init(&(*rdev)->cdev, &mfrc522_fops);

        return 0;
}

__init
static int mfrc522_init(void) {
        dev_t dev;
        int ret;
        const char devname[] = "mfrc522";

        ret = alloc_chrdev_region(&dev, 0, 1, devname);
        if (ret < 0) {
                pr_err("Failed to allocate major");
                return 1;
        }
        else {
                major = MAJOR(dev);
                pr_info("Got major %d", major);
        }

        if (mfrc522_dev_init(&rdev) < 0) {
                pr_err("Failed to init mfrc522_dev");
                return -ENOMEM;
        }

        if (cdev_add(&rdev->cdev, dev, 1) < 0) {
                pr_err("Failed to register char device");
                return -ENOMEM;
        }


        pr_info("Hello, card reader! (init)\n");
        return 0;
}

__exit
static void mfrc522_exit(void) {

        dev_t dev;

        dev = MKDEV(major, 0);
        cdev_del(&rdev->cdev);
        kfree(rdev);
        unregister_chrdev_region(dev, 1);

}

module_init(mfrc522_init);
module_exit(mfrc522_exit);
