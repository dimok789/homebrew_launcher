#define AF_INET		2
#define SOCK_STREAM	1
#define IPPROTO_TCP	6

struct in_addr
{
  unsigned long s_addr;
};

struct sockaddr
{
  unsigned short sin_family;
  unsigned short sin_port;
  struct in_addr sin_addr;
  char sin_zero[8];
};

/* IP address of the RPC client (in this case, 192.168.1.9) */
#define PC_IP	( (192<<24) | (168<<16) | (1<<8) | (9<<0) )
