#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <net/sock.h>
#include "mem.h"

#define TCP_PORT 4567

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Netmalloc, ?");

struct task_struct *network_thread = NULL;
int network_is_running = true;

static int run_network(void *data)
{
  struct sockaddr_in sin, csin;
  struct msghdr msg;
  struct iovec iov;
  mm_segment_t oldfs;
  char buffer[0x200] = "Hello";
  int len, cc;
  struct socket *csock = data;

  printk(KERN_INFO "NetMalloc: creating client thread\n");
  while (network_is_running)
    {
      // TRAITER LES REQUEST / REPLY ICI
      memset(&msg, 0, sizeof(msg));
      msg.msg_iov = &iov;
      msg.msg_iovlen = 1;
      msg.msg_iov->iov_len = 10;
      msg.msg_iov->iov_base = buffer;
      oldfs = get_fs();
      set_fs(KERNEL_DS);
      cc = sock_recvmsg(csock, &msg, 10, 0);
      printk(KERN_INFO "%d bytes received (%s)\n", cc, buffer);
      set_fs(oldfs);

      memset(&msg, 0, sizeof(msg));
      msg.msg_iov = &iov;
      msg.msg_iovlen = 1;
      msg.msg_flags = MSG_DONTWAIT;
      msg.msg_iov->iov_len = strlen(buffer);
      msg.msg_iov->iov_base = buffer;
      oldfs = get_fs();
      set_fs(KERNEL_DS);
      cc = sock_sendmsg(csock, &msg, strlen(buffer));
      printk(KERN_INFO "%d bytes sent\n", cc);
      set_fs(oldfs);
    }
  sock_release(csock);
  printk(KERN_INFO "NetMalloc: closing client thread\n");
  return 0;
}

static int networker(void *data)
{
  struct socket *sock = NULL, *csock = NULL;
  struct sockaddr_in sin, csin;
  int len;

  printk(KERN_INFO "NetMalloc: creating main thread\n");
  if (sock_create(PF_INET, SOCK_STREAM, IPPROTO_TCP, &sock) < 0)
    {
      printk(KERN_ERR "NetMalloc: cannot create socket\n");
      goto end;
    }

  if (sock_create(PF_INET, SOCK_STREAM, IPPROTO_TCP, &csock) < 0)
    {
      printk(KERN_ERR "NetMalloc: cannot create socket\n");
      goto end;
    }

  memset(&sin, 0, sizeof(sin));
  sin.sin_family = AF_INET;
  sin.sin_port = htons(TCP_PORT);
  sin.sin_addr.s_addr = htonl(INADDR_ANY);

  if (sock->ops->bind(sock, (struct sockaddr *)&sin, sizeof(sin)) < 0)
  {
    printk(KERN_ERR "NetMalloc: cannot bind socket\n");
    goto end;
  }

  sock->sk->sk_reuse = 1;

  if (sock->ops->listen(sock, 1) < 0)
  {
    printk(KERN_ERR "NetMalloc: cannot listen\n");
    goto end;
  }

  while (network_is_running)
    {
      if (sock->ops->accept(sock, csock, 0) < 0)
	{
	  printk(KERN_ERR "NetMalloc: cannot accept incoming connection\n");
	  goto end;
	}

      csock->ops->getname(csock, (struct sockaddr *) &csin, &len, 2);
      printk(KERN_INFO "--- 0x%x is connected ---\n",
	     htonl(csin.sin_addr.s_addr));

      if (!kthread_run(run_network, csock, "c_networker"))
	{
	  printk(KERN_ERR "NetMalloc: Unable to create client thread\n");
	  return 0;
	}

      if (sock_create(PF_INET, SOCK_STREAM, IPPROTO_TCP, &csock) < 0)
	{
	  printk(KERN_ERR "NetMalloc: cannot create socket\n");
	  goto end;
	}
      //break;
    }

 end:
  if (csock != NULL)
    sock_release(csock);

  if (sock != NULL)
    sock_release(sock);

  printk(KERN_INFO "NetMalloc: closing main thread\n");
  //while (1);
  return 0;
}


static int __init net_malloc_init(void)
{
  mem_init();
  
  if (!(network_thread = kthread_run(networker, NULL, "networker")))
  {
    printk(KERN_ERR "NetMalloc: Unable to create main thread\n");
    return 0;
  }
  /*
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
*/
  return 0;
}

static void __exit net_malloc_exit(void)
{
  if (network_thread != NULL)
  {
    network_is_running = 0;
    kthread_stop(network_thread);
  }
  mem_exit();
}

module_init(net_malloc_init);
module_exit(net_malloc_exit);
