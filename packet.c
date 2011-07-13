#include "packet.h"
#include "mem.h"

static struct nm_packet_rp *handle_alloc_packet(struct nm_packet_alloc_rq *rq, size_t packet_size)
{
  struct nm_packet_rp *rp;

  if (sizeof(struct nm_packet_rq) + sizeof(struct nm_packet_alloc_rq) != packet_size)
    return NULL;

  if ((rp = kmalloc(sizeof(*rp), GFP_KERNEL)) == NULL)
    return NULL;

  rp->error = mem_alloc(rq->size, &rp->id);
  rp->data_len = 0;
  return rp;
}

static struct nm_packet_rp *handle_free_packet(struct nm_packet_free_rq *rq, size_t packet_size)
{
  struct nm_packet_rp *rp;

  if (sizeof(struct nm_packet_rq) + sizeof(struct nm_packet_free_rq) != packet_size)
    return NULL;

  if ((rp = kmalloc(sizeof(*rp), GFP_KERNEL)) == NULL)
    return NULL;

  rp->error = mem_free(rq->id);
  rp->id = rq->id;
  rp->data_len = 0;
  return rp;
}

static struct nm_packet_rp *handle_read_packet(struct nm_packet_read_rq *rq, size_t packet_size)
{
  enum mem_error error;
  struct nm_packet_rp *rp;
  void *buf;

  if (sizeof(struct nm_packet_rq) + sizeof(struct nm_packet_read_rq) != packet_size)
    return NULL;

  printk(KERN_INFO "offset = %d\n", rq->off);
  error = mem_read(rq->id, &buf, rq->off, rq->size);
  if (error != MEM_SUCCESS)
  {
    if ((rp = kmalloc(sizeof(*rp), GFP_KERNEL)) == NULL)
      return NULL;

    rp->error = error;
    rp->id = rq->id;
    rp->data_len = 0;
    return rp;
  }

  if ((rp = kzalloc(sizeof(*rp) + rq->size, GFP_KERNEL)) == NULL)
    return NULL;

  rp->error     = MEM_SUCCESS;
  rp->id        = rq->id;
  rp->data_len  = rq->size;
  memcpy(rp->data, buf, rq->size);
  return rp;
}

static struct nm_packet_rp *handle_write_packet(struct nm_packet_write_rq *rq, size_t packet_size)
{
  struct nm_packet_rp *rp;

  //  printk(KERN_INFO "(%ld + %ld + %d) = %ld\n", sizeof(struct nm_packet_rq),
  //	 sizeof(struct nm_packet_write_rq),
  //	 rq->size,
  //	 packet_size
  //	 );
  if (sizeof(struct nm_packet_rq) + sizeof(struct nm_packet_write_rq) + rq->size != packet_size)
    return NULL;

  if ((rp = kmalloc(sizeof(*rp), GFP_KERNEL)) == NULL)
    return NULL;

  rp->error     = mem_write(rq->id, rq->data, rq->off, rq->size);
  rp->id        = rq->id;
  rp->data_len  = 0;
  return rp;
}

struct nm_packet_rp *handle_packet(struct nm_packet_rq *rq, size_t packet_size)
{
  int type = rq->packet_type;

  rq = ((struct nm_packet_rq *) ((char *)rq) + 1);
  switch (type)
  {
    case PT_ALLOC: return handle_alloc_packet((struct nm_packet_alloc_rq *)rq, packet_size);
    case PT_FREE:  return handle_free_packet ((struct nm_packet_free_rq *)rq,  packet_size);
    case PT_READ:  return handle_read_packet ((struct nm_packet_read_rq *)rq,  packet_size);
    case PT_WRITE: return handle_write_packet((struct nm_packet_write_rq *)rq, packet_size);
    default: return NULL;
  }
}
