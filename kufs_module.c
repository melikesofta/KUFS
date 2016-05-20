//Written by Meric Melike Softa and Firtina Kucuk on 20.05.2016
//for the 3rd class project of Comp304 - Operating Systems

//The following link was used as a tutorial and some of the
//codes we use might be based on that:
//http://kukuruku.co/hub/nix/writing-a-file-system-in-linux-kernel

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>


int kufs_init(void)
{
    printk(KERN_INFO "Loading Module KUFS\n");
    return 0;
}

void kufs_exit(void) {

	  printk(KERN_INFO "Removing Module KUFS\n");
	  printk("\n");

}

module_init( kufs_init);
module_exit( kufs_exit);

MODULE_LICENSE( "GPL");
MODULE_DESCRIPTION( "Project 3 for COMP304");
MODULE_AUTHOR("msofta&fkucuk");
