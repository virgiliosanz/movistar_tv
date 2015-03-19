#ifndef __multicast_h__
#define __multicast_h__

// 3*4 + 3 + 1 = 16
#define MCAST_IP_CHARS (16)
#define MCAST_MSG_BUFSIZE (1500)

struct _mcast_s {
	char  ip[MCAST_IP_CHARS];
	int   port;
	int   socket;
};
typedef struct _mcast_s mcast_s;

mcast_s *mcast_alloc();
void mcast_free(mcast_s *mcast);
void mcast_open(mcast_s *mcast, const char *ip, const int port);
void mcast_close(mcast_s *mcast);

typedef void (*mcast_proccess_file_cb) (const char *file, void *ctx);
void mcast_proccess_files(mcast_s *mcast, mcast_proccess_file_cb cb, void *ctx);


#endif

