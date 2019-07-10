#include <linux/cdev.h>
#include <linux/errno.h>
#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/ioctl.h>

#define BUFSIZE 32
#define WR_VALUE _IOW('a','a',char*)
#define RD_VALUE _IOR('a','b',char*)

MODULE_AUTHOR("Paul Beneteau");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("MFRC522 driver");

struct mfrc522_dev {
        struct cdev cdev;
        char buf[BUFSIZE];
};

static int major;
static struct mfrc522_dev *rdev;

static long mfrc522_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	switch(cmd) {
	  	// Write user data in buffer
		case WR_VALUE:
			// Set buffer to {0} and copy user data
			memset(rdev->buf, 0, BUFSIZE);
			copy_from_user(rdev->buf, (char*)arg, BUFSIZE);
			//pr_info("Value: %s\n", rdev->buf);
			break;
		// Read buffer for user
		case RD_VALUE:
			copy_to_user((char*)arg, rdev->buf, BUFSIZE);
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
	
	memset((*rdev)->buf, 0, BUFSIZE);
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
                pr_info("!Got major %d", major);
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
