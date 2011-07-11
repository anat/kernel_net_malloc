#include "mem.h"

const char *mem_error_str[] =
{
  "Success",
  "Allocation failed",
  "Invalid ID",
  "Overflow"
};

LIST_HEAD(mem_list);
atomic_t           mem_id;
wait_queue_head_t  mem_queue;
spinlock_t         mem_lock;

static struct mem_struct *    __net_malloc_find(unsigned int id)
{
  struct mem_struct *         ms;

  spin_lock(&mem_lock);
  list_for_each_entry(ms, &mem_list, mem_node)
  {
    if (ms->mem_id == id)
      break;
  }
  spin_unlock(&mem_lock);

  if (ms->mem_id != id)
    return NULL;

  return ms;
}

void mem_init(void)
{
  init_waitqueue_head(&mem_queue);
  spin_lock_init(&mem_lock);
  atomic_set(&mem_id, 0);
}

void mem_exit(void)
{
  struct mem_struct   *tmp, *ms;

  spin_lock(&mem_lock);
  list_for_each_entry_safe(ms, tmp, &mem_list, mem_node)
  {
    list_del(&ms->mem_node);
    kfree(ms->mem_addr);
    kfree(ms);
  }
  spin_unlock(&mem_lock);
}

enum mem_error        mem_alloc(size_t sz, unsigned int *id)
{
  struct mem_struct   *ms = NULL;

  if ((ms = kzalloc(sizeof(*ms), GFP_KERNEL)) == NULL)
    goto err;

  if ((ms->mem_addr = kzalloc(sz, GFP_KERNEL)) == NULL)
    goto err;

  INIT_LIST_HEAD(&ms->mem_node);
  *id = atomic_read(&mem_id);
  ms->mem_id = *id;
  atomic_inc(&mem_id);
  ms->mem_sz = sz;

  spin_lock(&mem_lock);
  list_add_tail(&mem_list, &ms->mem_node);
  spin_unlock(&mem_lock);

  return MEM_SUCCESS;

err:
  if (ms != NULL)
  {
    if (ms->mem_addr != NULL)
      kfree(ms->mem_addr);
    kfree(ms);
  }
  return MEM_ALLOC_FAILED;
}

enum mem_error        mem_free(unsigned int id)
{
  struct mem_struct   *tmp, *ms;
  enum mem_error      res = MEM_INVALID_ID;

  spin_lock(&mem_lock);
  list_for_each_entry_safe(ms, tmp, &mem_list, mem_node)
  {
    if (ms->mem_id == id)
    {
      list_del(&ms->mem_node);
      kfree(ms->mem_addr);
      kfree(ms);
      res = MEM_SUCCESS;
      break;
    }
  }
  spin_unlock(&mem_lock);
  return res;
}

enum mem_error        mem_read(unsigned int id, void **buf, unsigned long off, size_t sz)
{
  struct mem_struct   *ms = NULL;

  if ((*buf = kzalloc(sz, GFP_KERNEL)) == NULL)
    return MEM_ALLOC_FAILED;

  if ((ms = __net_malloc_find(id)) == NULL)
    return MEM_INVALID_ID;

  if (off > ms->mem_sz || sz > ms->mem_sz || off + sz > ms->mem_sz)
    return MEM_OVERFLOW;

  memcpy(*buf, ms->mem_addr + off, sz);
  return MEM_SUCCESS;
}

enum mem_error        mem_write(unsigned int id, const void *buf, unsigned long off, size_t sz)
{
  struct mem_struct   *ms = NULL;

  if ((ms = __net_malloc_find(id)) == NULL)
    return MEM_INVALID_ID;

  if (off > ms->mem_sz || sz > ms->mem_sz || off + sz > ms->mem_sz)
    return MEM_OVERFLOW;

  memcpy(ms->mem_addr + off, buf, sz);
  return MEM_SUCCESS;
}
