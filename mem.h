#ifndef _MEM_H_
#define _MEM_H_

#include <linux/list.h>
#include <linux/slab.h>
#include <linux/kthread.h>
#include <asm/atomic.h>

enum mem_error
{
  MEM_SUCCESS,
  MEM_ALLOC_FAILED,
  MEM_INVALID_ID,
  MEM_OVERFLOW,
  MEM_ENUM_SIZE
};

extern const char *mem_error_str[MEM_ENUM_SIZE];

struct              mem_struct
{
  unsigned int      mem_id;
  void *            mem_addr;
  size_t            mem_sz;
  struct list_head  mem_node;
};

void            mem_init(void);
void            mem_exit(void);
enum mem_error  mem_alloc(size_t sz, unsigned int *id);
enum mem_error  mem_free(unsigned int id);
enum mem_error  mem_read(unsigned int id, void **buf, unsigned long off, size_t sz);
enum mem_error  mem_write(unsigned int id, const void *buf, unsigned long off, size_t sz);

#endif /* _MEM_H_ */
