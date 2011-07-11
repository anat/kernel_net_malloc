
/* struct */
#include "network.h"
/* socket() */
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
/* exit() */
#include <unistd.h>
#include <stdio.h>

int nm_alloc(int s, unsigned int size);
void nm_free(int s, unsigned int id);
void nm_read(int s, unsigned int id, void * data, int offset, unsigned int size);
void nm_write(int s, unsigned int id, void * data, int offset, unsigned int size);



void play(int s)
{
  int id;
  char buf[46] = {0};

  id = nm_alloc(s, 100);

  nm_write(s, id, "Sale CON (ref: Carmageddon)\n", 0, 28);

  nm_write(s, id, "You damned sucker\n", 27, 18);

  nm_read(s, id, buf, 0, 46);
  write(1, buf, 46);

  nm_free(s, id);
}

int main(int ac, char **av)
{
  nmHeader h;
  int s;
  struct sockaddr_in sin;  

  s = socket(AF_INET, SOCK_STREAM, 0);

  sin.sin_family = AF_INET;
  sin.sin_port = htons(1338);
  sin.sin_addr.s_addr = inet_addr("127.0.0.1");

  if (!connect(s, (struct sockaddr *)&sin, sizeof (struct sockaddr_in)) == 0)
    {
      perror("Balls of steeeel");
      return 1;
    }

  play(s);
  return 0;
}

void errorSHIT(nmHeader * h)
{
  if (h->err)
    {
      write(2, "HAS ERROR\n", 10);
      printf("type = %s\n", (h->type == ALLOC ? "alloc" : (h->type == FREE ? "free" : (h->type == READ ? "read" : (h->type == WRITE ? "write" : "unknown!")))));
      printf("id = %u\nerr = %d\nsize = %u\noffset = %u\n", h->id, h->err, h->size, h->offset);
      _exit(1337);
    }
}

// alloc        -> SIZE                <- ID
int nm_alloc(int s, unsigned int size)
{ 
  int id;
  nmHeader h;
  h.type = ALLOC;
  h.size = size;
  write(s, &h, sizeof(h));
  read(s, &h, sizeof(h));
  errorSHIT(&h);
  id = h.id;

  printf("ID : %d\n", id);
  return id;
}

// free        -> ID                    <- ID, ERR
void nm_free(int s, unsigned int id)
{ 
 nmHeader h;
 h.type = FREE;
 h.id = id;
 write(s, &h, sizeof(h));
 read(s, &h, sizeof(h));
 errorSHIT(&h);
}

// read        -> ID, OFFSET, SIZE            <- ID, ERR, DATA
void nm_read(int s, unsigned int id, void * data, int offset, unsigned int size)
{
  nmHeader h;
  h.type = READ;
  h.id = id;
  h.offset = offset;
  h.size = size;
  write(s, &h, sizeof(h));
  read(s, &h, sizeof(h));
  read(s, data, size);
  errorSHIT(&h);
}

// write        -> ID, OFFSET, SIZE, DATA        <- ID,ERR
void nm_write(int s, unsigned int id, void * data, int offset, unsigned int size)
{
  nmHeader h;
  h.type = WRITE;
  h.id = id;
  h.offset = offset;
  h.size = size;
  write(s, &h, sizeof(h));
  write(s, data, size);
  read(s, &h, sizeof(h));
  errorSHIT(&h);
}
