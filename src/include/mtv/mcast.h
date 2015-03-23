#ifndef __multicast_h__
#define __multicast_h__

// 3*4 + 3 + 1 = 16 -> 999.999.999.999\0
#define MCAST_IP_CHARS (16)

struct _mcast_s {
	char               ip[MCAST_IP_CHARS];
	int                port;
	int                socket;
	struct sockaddr_in addr_in;
	struct sockaddr   *addr;
	socklen_t          addrlen;

};
typedef struct _mcast_s mcast_s;

struct _mcast_file_s {
	uint8_t  type;
	uint32_t id;
	uint32_t size;
	char    *data;
};
typedef struct _mcast_file_s mcast_file_s;

mcast_s *mcast_alloc();
void mcast_free(mcast_s *mcast);
void mcast_open(mcast_s *mcast, const char *ip, const int port);
void mcast_close(mcast_s *mcast);

typedef bool (*mcast_proccess_file_cb) (const mcast_file_s *file, void *ctx);
void mcast_proccess_files(mcast_s *mcast, mcast_proccess_file_cb cb, void *ctx);


#endif

