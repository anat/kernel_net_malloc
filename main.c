#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>

#include "mem.h"

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Netmalloc, ?");



static int __init net_malloc_init(void)
{
  unsigned int id0, id1, id2;
  enum mem_error me;

  mem_init();

  me = mem_alloc(0x100, &id0);
  printk(KERN_INFO "%s - %u\n", mem_error_str[me], id0);

  return 0;
}

static void __exit net_malloc_exit(void)
{
  mem_exit();
}

module_init(net_malloc_init);
module_exit(net_malloc_exit);
