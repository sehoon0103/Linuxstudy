#include <linux/module.h>

static int __init comento_module_init(void)
{
	printk(KERN_DEBUG "%d\n", __func__);
        return 0;
}

static void __exit comento_module_exit(void)
{
        printk(KERN_DEBUG "%d\n", __func__);
 
}


module_init(comento_module_init);
module_exit(comento_module_exit);

MODULE_AUTHOR("Hello<hello@comento.com");
MODULE_DESCRIPTION("Example module");
MODULE_LICENSE("GPL v2");

