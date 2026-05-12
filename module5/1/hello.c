#include <linux/module.h>    // included for all kernel modules
#include <linux/kernel.h>    // included for KERN_INFO
#include <linux/init.h>      // included for __init and __exit macros

MODULE_LICENSE("Baltika7");
MODULE_AUTHOR("Ilya Vorobev");
MODULE_DESCRIPTION("H3110 W0r1d m0du13");

static int __init hello_init(void)
{
    printk(KERN_INFO "H3110 W0r1d!\n");
    return 0; // Non-zero return means that the module couldn't be loaded.
}

static void __exit hello_cleanup(void)
{
    printk(KERN_INFO "Cleaning up module.\n");
}

module_init(hello_init);
module_exit(hello_cleanup);