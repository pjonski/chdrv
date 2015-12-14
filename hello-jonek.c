/******************************************************************************
 *
 *   Copyright (C) 2011  Intel Corporation. All rights reserved.
 *
 *   This program is free software;  you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; version 2 of the License.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY;  without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See
 *   the GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program;  if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 *****************************************************************************/

#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>

#include <linux/types.h>  // for dev_t
#include <linux/kdev_t.h> // for format_dev_t
#include <linux/fs.h>	  // for alloc_chrdev_region()
#include <linux/cdev.h>   // for cdev functions, obviously
#include <linux/uaccess.h>// for copy_to_user
#include <linux/string.h> // for strlen()
static dev_t my_dev;
struct cdev my_cdev;

static char output[] = "Volenti non fit iniuria.\n";

ssize_t my_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{

/*
if (output[*f_pos] == '\0') {
        printk(KERN_INFO "End of string, returning zero.\n");
        return 0;
    }
	printk("%d\n",strlen(output)-1);
    copy_to_user(buf, &output[*f_pos], strlen(output)-1);
    *f_pos += 1;
    return 1;  // returned a single character


*/
	int str_len=strlen(output);
	int not_copied_bytes;
	printk("%d %d\n",str_len, *f_pos);
	not_copied_bytes= copy_to_user(buf, output+(*f_pos), str_len);

	printk("%d\n",not_copied_bytes);
	if(!not_copied_bytes)
		return 0;
	*f_pos+=not_copied_bytes;
	return 1;

}


struct file_operations my_fops = {
	.owner = THIS_MODULE,
	.read = my_read,
};
static char buffer[64];



int __init chardrv_in(void)
{
	printk("Chardrv loaded.(v0.1) \n");
	alloc_chrdev_region(&my_dev, 0,1, "chardrv");
	printk("%s\n",format_dev_t(buffer,my_dev));

	cdev_init(&my_cdev,&my_fops);
	my_cdev.owner=THIS_MODULE;
	cdev_add(&my_cdev, my_dev, 1);

	return 0;
}

void __exit chardrv_out(void)
{
	printk("Chardrv unloaded.\n");
	cdev_del(&my_cdev);
	unregister_chrdev_region(my_dev,1);
}

module_init(chardrv_in);
module_exit(chardrv_out);

MODULE_LICENSE("GPL");
