#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>

#include "mem.h"

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Netmalloc, ?");



static int __init net_malloc_init(void)
{
  unsigned int id0, id1, id2;
  enum mem_error err;
  char *text;

  mem_init();

  err = mem_alloc(0x100, &id0);
  printk(KERN_INFO "%s - %u\n", mem_error_str[err], id0);

  err = mem_alloc(0x100, &id1);
  printk(KERN_INFO "%s - %u\n", mem_error_str[err], id1);

  err = mem_alloc(0x100, &id2);
  printk(KERN_INFO "%s - %u\n", mem_error_str[err], id2);

  err = mem_free(id1);
  printk(KERN_INFO "%s\n", mem_error_str[err]);

  err = mem_free(id1);
  printk(KERN_INFO "%s\n", mem_error_str[err]);

  err = mem_write(id1, "hello", 0x1, strlen("hello"));
  printk(KERN_INFO "%s\n", mem_error_str[err]);

  err = mem_write(id2, "hello", 0x1, strlen("hello"));
  printk(KERN_INFO "%s\n", mem_error_str[err]);

  err = mem_read(id2, &text, 0x2, 4);
  printk(KERN_INFO "%s, %s\n", mem_error_str[err], text);

  return 0;
}

static void __exit net_malloc_exit(void)
{
  mem_exit();
}

module_init(net_malloc_init);
module_exit(net_malloc_exit);
