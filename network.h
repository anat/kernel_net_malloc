#ifndef		NETW_NETM_H
#define		NETW_NETM_H


typedef enum e_packetType
  {
    ALLOC,
    FREE,
    READ,
    WRITE
  } packetType;


typedef struct s_nmHeader
{
  packetType type;
  unsigned int id;
  int err;
  unsigned int size;
  unsigned int offset;
} __attribute__ ((packed)) nmHeader;


#endif
