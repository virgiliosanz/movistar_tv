#ifndef __multicast_h__
#define __multicast_h__

// 3*4 + 3 + 1 = 16 -> 999.999.999.999\0
#define MCAST_IP_CHARS (16)

struct mcast {
	char               ip[MCAST_IP_CHARS];
	int                port;
	int                socket;
	struct sockaddr_in addr_in;
	struct sockaddr   *addr;
	socklen_t          addrlen;

};

struct mcast_file {
	uint8_t  type;
	uint32_t id;
	uint32_t size;
	char    *data;
};

struct mcast *mcast_alloc();
void mcast_free(struct mcast *mcast);
void mcast_open(struct mcast *mcast, const char *ip, const int port);
void mcast_close(struct mcast *mcast);

typedef bool (*mcast_proccess_file_cb) (const struct mcast_file *file, void *ctx);
void mcast_proccess_files(struct mcast *mcast, mcast_proccess_file_cb cb, void *ctx);


#endif

