#ifndef NETW_NETM_H
#define NETW_NETM_H

#include <linux/types.h>

enum
{
  PT_ALLOC,
  PT_FREE,
  PT_READ,
  PT_WRITE
};

/** Response **/
struct nm_packet_rp /* Response */
{
  unsigned char error; /* If this field is different from MEM_SUCCESS, the packet must be dropped */
  unsigned int  id;
  unsigned int  data_len; /* If this field is equal to zero, there is no data */
  unsigned char data[0];
} __attribute__ ((packed));

/** Request **/
struct nm_packet_rq /* Request */
{
  unsigned char packet_type;
  unsigned char packet_data[0]; /* This field contains structures below */
} __attribute__ ((packed));

/* Allocation */
struct nm_packet_alloc_rq /* Request */
{
  unsigned int  size;
} __attribute__ ((packed));


/* Free */
struct nm_packet_free_rq /* Request */
{
  unsigned int  id;
} __attribute__ ((packed));

/* Read */
struct nm_packet_read_rq /* Request */
{
  unsigned int  id;
  unsigned int  off;
  unsigned int  size;
} __attribute__ ((packed));

/* Write */
struct nm_packet_write_rq /* Request */
{
  unsigned int  id;
  unsigned int  off;
  unsigned int  size;
  unsigned char data[0];
} __attribute__ ((packed));

struct nm_packet_rp *handle_packet(struct nm_packet_rq *rq, size_t packet_size);

#endif
