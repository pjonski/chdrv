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
#include <linux/device.h>

#define FILENAME "chdrv"
static dev_t my_dev;
static struct cdev my_cdev;
static struct class *chdrv_class=NULL;
static char input[50];

void make_uppercase(char* text)
{
	int index=0;
	while(text[index]!='\0')
	{
		if(text[index]>=97 && text[index] <=122 && (index==0 || text[index-1]==' '))
			text[index]-=32;
		index++;
	}
}
ssize_t my_write(struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{
	copy_from_user(input, buf,count);
	make_uppercase(input);
	return count;
}

ssize_t my_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{
	if(strlen(input)==0)
		printk("No text stored in driver, use echo to send string.<V1.1>>\n");
	int str_len=strlen(input);
	int copied_bytes;
	if (input[*f_pos] == '\0') {
        return 0;
    	}
	copied_bytes = str_len-(*f_pos);
	copied_bytes = (copied_bytes<count) ? copied_bytes : count;
	copy_to_user(buf, &input[*f_pos], copied_bytes);
	*f_pos+=copied_bytes;
	return copied_bytes;
}


struct file_operations my_fops = {
	.owner = THIS_MODULE,
	.read = my_read,
	.write = my_write,
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
	
	chdrv_class=class_create(THIS_MODULE, FILENAME);
	device_create(chdrv_class, NULL, my_dev, NULL, FILENAME);	

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
