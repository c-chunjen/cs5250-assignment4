#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/proc_fs.h>
#include <asm/uaccess.h>
#include <linux/ioctl.h> /* needed for the _IOW etc stuff used later */
#define MAJOR_NUMBER 61

#define DEVICE_SIZE 4*1024*1024
#define SCULL_MESSAGE_SIZE 256
#define SCULL_IOC_MAGIC 'k'
#define SCULL_IOC_MAXNR 4
#define SCULL_HELLO _IO(SCULL_IOC_MAGIC, 1)
#define SCULL_IOW _IOW(SCULL_IOC_MAGIC, 2, unsigned long)
#define SCULL_IOR _IOR(SCULL_IOC_MAGIC, 3, unsigned long)
#define SCULL_IOWR _IOWR(SCULL_IOC_MAGIC, 4, unsigned long)

/* forward declaration */
int fourMegaBytes_open(struct inode *inode, struct file *filep);
int fourMegaBytes_release(struct inode *inode, struct file *filep);
long ioctl_example(struct file *filp, unsigned int cmd, unsigned long arg);
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
	llseek: fourMegaBytes_llseek,
	unlocked_ioctl: ioctl_example
};

char *fourMegaBytes_data = NULL;
int write_count = 0;
char dev_msg[SCULL_MESSAGE_SIZE];

int fourMegaBytes_open(struct inode *inode, struct file *filep)
{
	return 0; // always successful
}

int fourMegaBytes_release(struct inode *inode, struct file *filep)
{
	return 0; // always successful
}

long ioctl_example(struct file *filp, unsigned int cmd, unsigned long arg)
{
	printk(KERN_INFO "Running ioctl_example:\n");
	printk(KERN_INFO "cmd field is %lu.\n", cmd);
	printk(KERN_INFO "arg field is %lu.\n", arg);
	
	int err = 0, tmp[SCULL_MESSAGE_SIZE];
	int retval = 0;
	
	/*
	* extract the type and number bitfields, and don't decode
	* wrong cmds: return ENOTTY (inappropriate ioctl) before access_ok()
	*/
	if(_IOC_TYPE(cmd) != SCULL_IOC_MAGIC) return -ENOTTY;
	if(_IOC_NR(cmd) > SCULL_IOC_MAXNR) return -ENOTTY;

	/*
	* the direction is a bitmask, and VERIFY_WRITE catches R/W
	* transfers. `Type' is user-oriented, while
	* access_ok is kernel-oriented, so the concept of "read" and
	* "write" is reversed
	*/
	if(_IOC_DIR(cmd) & _IOC_READ)
		err = !access_ok(VERIFY_WRITE, (void __user *)arg, _IOC_SIZE(cmd));
	else if(_IOC_DIR(cmd) & _IOC_WRITE)
		err = !access_ok(VERIFY_READ, (void __user *)arg, _IOC_SIZE(cmd));
	if (err) return -EFAULT;
	
	switch(cmd) {
		case SCULL_HELLO:
			printk(KERN_WARNING "From SCULL_HELLO:\n");
			printk(KERN_WARNING "hello\n");
			break;
		case SCULL_IOW:
			printk(KERN_WARNING "From SCULL_IOW:\n");
			if(copy_from_user(dev_msg, (char *)arg, SCULL_MESSAGE_SIZE))
				retval = -EFAULT;
			printk(KERN_WARNING "Set dev_msg from user_msg: dev_msg=\"%s\"\n", dev_msg);
			break;
		case SCULL_IOR:
			printk(KERN_WARNING "From SCULL_IOR:\n");
			if(copy_to_user((char *)arg, dev_msg, SCULL_MESSAGE_SIZE))
				retval = -EFAULT;
			printk(KERN_WARNING "Copy dev_msg to user_msg: dev_msg=\"%s\"\n", dev_msg);
			break;
		case SCULL_IOWR:
			printk(KERN_WARNING "From SCULL_IOWR:\n");
			
			if(copy_from_user(tmp, (char *)arg, SCULL_MESSAGE_SIZE))
				retval = -EFAULT;
			printk(KERN_WARNING "Set tmp from user_msg: tmp=\"%s\"\n", tmp);
			
			if(copy_to_user((char *)arg, dev_msg, SCULL_MESSAGE_SIZE))
				retval = -EFAULT;
			printk(KERN_WARNING "Copy dev_msg(old) to user_msg: dev_msg=\"%s\"\n", dev_msg);
			
			memcpy(dev_msg,tmp,SCULL_MESSAGE_SIZE);
			printk(KERN_WARNING "Copy tmp to dev_msg(new): dev_msg=\"%s\"\n", dev_msg);
			
			break;
		default: /* redundant, as cmd was checked against MAXNR */
			return -ENOTTY;
	}

	return retval;
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
	
	printk(KERN_INFO "write_count is %lu.\n", write_count);
	
	if(*f_pos <= write_count) {
		if(*f_pos+count > write_count) {
			count = write_count-*f_pos;
		}
		if(copy_to_user(buf, fourMegaBytes_data+*f_pos, count))
			return -EFAULT;
	}
	
	*f_pos = *f_pos+count;
	return count;
}

ssize_t fourMegaBytes_write(struct file *filep, const char *buf, size_t count, loff_t *f_pos)
{
	/*please complete the function on your own*/

	printk(KERN_INFO "Running fourMegaBytes_write:\n");
	printk(KERN_INFO "buf field is %lu.\n", buf);
	printk(KERN_INFO "count field is %lu.\n", count);
	printk(KERN_INFO "f_pos is %lu.\n", *f_pos);
	
	if((*f_pos)+count < DEVICE_SIZE) {
		write_count = count-(*f_pos);
	} else {
		write_count = DEVICE_SIZE-(*f_pos);
		count = DEVICE_SIZE-(*f_pos);
		/*
		*Should return -ENOSPC for out of space usage
		*/
		//return -ENOSPC;
	}
	
	if(copy_from_user(fourMegaBytes_data+*f_pos, buf, count))
		return -EFAULT;
	
	*f_pos = *f_pos+count;
	return count;
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
	memset(fourMegaBytes_data, 'X', DEVICE_SIZE);  
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
