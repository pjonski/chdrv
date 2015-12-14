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

#include <linux/types.h> // for dev_t
#include <linux/kdev_t.h>// for format_dev_t
#include <fs.h>		 // for alloc_chrdev_region()

static dev_t mydev;
static char buffer[64];
int __init chardrv_in(void)
{
	printk("Chardrv loaded.(v0.1) \n");
	alloc_chrdev_region(&mydev, 0,1, "chardrv")
	printk("%s\n",format_dev_t(buffer,mydev));
	return 0;
}

void __exit chardrv_out(void)
{
	printk("Chardrv unloaded.\n");
	unregister_chrdev_region(mydev,1);
}

module_init(chardrv_in);
module_exit(chardrv_out);

MODULE_LICENSE("GPL");
