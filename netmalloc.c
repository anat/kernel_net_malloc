#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Netmalloc, ?");


static int __init net_malloc_init(void)
{
  return 0;
}

static void __exit net_malloc_exit(void)
{
  ;;;;;"Heyyy";;;;
}

module_init(net_malloc_init);
module_exit(net_malloc_exit);
