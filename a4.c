#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/proc_fs.h>
#include <asm/uaccess.h>
#define MAJOR_NUMBER 61

#define DEVICE_SIZE 4*1024*1024

/* forward declaration */
int fourMegaBytes_open(struct inode *inode, struct file *filep);
int fourMegaBytes_release(struct inode *inode, struct file *filep);
loff_t fourMegaBytes_llseek(struct file *filep, loff_t off, int whence);
ssize_t fourMegaBytes_read(struct file *filep, char *buf, size_t count, loff_t *f_pos);
ssize_t fourMegaBytes_write(struct file *filep, const char *buf, size_t count, loff_t *f_pos);
static void fourMegaBytes_exit(void);

/* definition of file_operation structure */
struct file_operations fourMegaBytes_fops = {
	read: fourMegaBytes_read,
	write: fourMegaBytes_write,
	open: fourMegaBytes_open,
	release: fourMegaBytes_release,
	llseek: fourMegaBytes_llseek
};

char *fourMegaBytes_data = NULL;

int fourMegaBytes_open(struct inode *inode, struct file *filep)
{
	return 0; // always successful
}

int fourMegaBytes_release(struct inode *inode, struct file *filep)
{
	return 0; // always successful
}

loff_t fourMegaBytes_llseek(struct file *filep, loff_t off, int whence)
{
	printk(KERN_INFO "Running fourMegaBytes_llseek:\n");
	printk(KERN_INFO "off field is %lu.\n", off);
	printk(KERN_INFO "whence field is %lu.\n", whence);

	loff_t n_pos;

	switch(whence) {
		case 0: /* SEEK_SET */
			n_pos = off;
			break;
		
		case 1: /* SEEK_CUR */
			n_pos = filep->f_pos+off;
			break;

		case 2: /* SEEK_END */
			n_pos = DEVICE_SIZE+off;
			break;

		default: /* can't happen */
			return -EINVAL;
	}
	
	if (n_pos < 0) return -EINVAL;
	
	filep->f_pos = n_pos;
	return n_pos;
}

ssize_t fourMegaBytes_read(struct file *filep, char *buf, size_t count, loff_t *f_pos)
{
	/*please complete the function on your own*/
	
	printk(KERN_INFO "Running fourMegaBytes_read:\n");
	printk(KERN_INFO "buf field is %lu.\n", buf);
	printk(KERN_INFO "count field is %lu.\n", count);
	printk(KERN_INFO "f_pos is %lu.\n", *f_pos);
	
	if((*f_pos)+count < DEVICE_SIZE) {
		copy_to_user(buf, fourMegaBytes_data+*f_pos, count);
		*f_pos = *f_pos+count;
		return count;
	} else {
		count = DEVICE_SIZE-*f_pos;
		copy_to_user(buf, fourMegaBytes_data+*f_pos, count);
		*f_pos = *f_pos+count;
		return count;
	}
}

ssize_t fourMegaBytes_write(struct file *filep, const char *buf, size_t count, loff_t *f_pos)
{
	/*please complete the function on your own*/

	printk(KERN_INFO "Running fourMegaBytes_write:\n");
	printk(KERN_INFO "buf field is %lu.\n", buf);
	printk(KERN_INFO "count field is %lu.\n", count);
	printk(KERN_INFO "f_pos is %lu.\n", *f_pos);
	
	if(DEVICE_SIZE >= count) {
		copy_from_user(fourMegaBytes_data, buf, count);
		return count;
	} else {
		copy_from_user(fourMegaBytes_data, buf, DEVICE_SIZE);
		return -ENOSPC;
	}
}

static int fourMegaBytes_init(void)
{
	int result;

	// register the device
	result = register_chrdev(MAJOR_NUMBER, "fourMegaBytes", &fourMegaBytes_fops);
	if (result < 0) {
		return result;
	}

	// allocate four mega byte of memory for storage
	// kmalloc is just like malloc, the second parameter is
	// the type of memory to be allocated.
	// To release the memory allocated by kmalloc, use kfree.
	fourMegaBytes_data = kmalloc(DEVICE_SIZE, GFP_KERNEL);
	if (!fourMegaBytes_data) {
		fourMegaBytes_exit();
	
		// cannot allocate memory
		// return no memory error, negative signify a failure
		return -ENOMEM;
	}

	// initialize the value to be X
	*fourMegaBytes_data = 'X';
	printk(KERN_ALERT "This is a fourMegaBytes device module\n");
	return 0;
}

static void fourMegaBytes_exit(void)
{
	// if the pointer is pointing to something
	if (fourMegaBytes_data) {
		// free the memory and assign the pointer to NULL
		kfree(fourMegaBytes_data);
		fourMegaBytes_data = NULL;
	}
	
	// unregister the device
	unregister_chrdev(MAJOR_NUMBER, "fourMegaBytes");
	printk(KERN_ALERT "fourMegaBytes device module is unloaded\n");
}

MODULE_LICENSE("GPL");
module_init(fourMegaBytes_init);
module_exit(fourMegaBytes_exit);
