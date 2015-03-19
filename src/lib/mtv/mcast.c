#include <mtv/all.h>

mcast_s *
mcast_alloc()
{
	mcast_s *mcast;
	mcast = malloc(sizeof(mcast_s));
	error_if(NULL == mcast, error, "Error Allocating Memory");

	memset(&mcast->ip, sizeof(mcast->ip), 0);
	mcast->socket = 0;
	mcast->port = 0;

	return mcast;

error:
	return NULL;
}

void
mcast_free(mcast_s *mcast)
{
	if (!mcast) return;
	free(mcast);
}

void
mcast_open(mcast_s *mcast, const char *ip, const int port)
{
        int rv = 0;

	error_if(mcast == NULL, error, "Params Error");
	error_if(ip == NULL, error, "Params Error");

	strncpy(mcast->ip, ip, MCAST_IP_CHARS);
	mcast->port = port;

	trace("Creating multicast socket on %s:%d", mcast->ip, mcast->port);
        mcast->socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        error_if(mcast->socket < 0, error, "Cannot create socket");


        unsigned int reuse = 1; // allow multiple sockets on same port number
        rv = setsockopt(
        		mcast->socket,
        		SOL_SOCKET,
        		SO_REUSEADDR,
        		&reuse,
        		sizeof(reuse));
        error_if(rv < 0, error, "Cannot set socket options");

        // bind to receive address
        struct sockaddr_in addr;
        memset(&addr, 0, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = htonl(INADDR_ANY);
        addr.sin_port = htons(mcast->port);
        rv = bind(mcast->socket, (struct sockaddr*)&addr, sizeof(addr));
        error_if(rv < 0, error, "Cannot bind socket");

        // use setsockopt() to request that the kernel join a multicast group
        struct ip_mreq mreq;
        mreq.imr_multiaddr.s_addr = inet_addr(mcast->ip);
        mreq.imr_interface.s_addr = htonl(INADDR_ANY);
        rv = setsockopt(
        		mcast->socket,
        		IPPROTO_IP,
        		IP_ADD_MEMBERSHIP,
        		&mreq,
        		sizeof(mreq));
        error_if(rv < 0, error, "Cannot join group");

        trace("Multicast Connection OK");

	return;

error:
	mcast->socket = 0;
	return;
}

void
mcast_close(mcast_s *mcast)
{
	close(mcast->socket);
	return;
}


void
mcast_proccess_files(mcast_s *mcast, mcast_proccess_file_cb cb, void *ctx)
{
	//TODO: Write mcast_proccess_files
	return;
}
