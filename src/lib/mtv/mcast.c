#include <mtv/all.h>

#define MCAST_MSG_BUFSIZE (1536)

// Ver página 59
struct _mcast_message {
	uint8_t end;
        uint32_t size;
        uint8_t type;
        uint32_t id;
        uint16_t chunk_num;
        uint8_t total_chunks;
        uint8_t sep;
        // char data[MCAST_MSG_BUFSIZE - 12 + 1];
        char *data;
};

static void _unpack(uint8_t *data, struct _mcast_message *mc);

mcast_s *
mcast_alloc()
{
	mcast_s *mcast;
	mcast = malloc(sizeof(mcast_s));
	error_if(NULL == mcast, error, "Error Allocating Memory");

	memset(&mcast->ip, 0, MCAST_IP_CHARS);
	mcast->socket  = 0;
	mcast->port    = 0;
        mcast->addr    = (struct sockaddr *) &mcast->addr_in;
        mcast->addrlen = sizeof(mcast->addr_in);

	return mcast;

error:
	return NULL;
}

void
mcast_free(mcast_s *mcast)
{
	if (!mcast) return;

	mcast_close(mcast);
	free(mcast);
}

void
mcast_open(mcast_s *mcast, const char *ip, const int port)
{
	error_if(mcast == NULL, error, "Params Error");
	error_if(ip == NULL, error, "Params Error");

	strncpy(mcast->ip, ip, MCAST_IP_CHARS);
	mcast->port = port;

	trace("Creating multicast socket on %s:%d", mcast->ip, mcast->port);
        mcast->socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        error_if(mcast->socket < 0, error, "Cannot create socket");

        int             rv = 0;
        unsigned int reuse = 1; // allow multiple sockets on same port number
        rv = setsockopt(mcast->socket,
        		SOL_SOCKET,
        		SO_REUSEADDR,
        		&reuse,
        		sizeof(reuse));
        error_if(rv < 0, error, "Cannot set socket options");

        // bind to receive address
        memset(&mcast->addr_in, 0, mcast->addrlen);
        mcast->addr_in.sin_family      = AF_INET;
        mcast->addr_in.sin_addr.s_addr = htonl(INADDR_ANY);
        mcast->addr_in.sin_port        = htons(mcast->port);
        rv = bind(mcast->socket, mcast->addr, mcast->addrlen);
        error_if(rv < 0, error, "Cannot bind socket");

        // use setsockopt() to request that the kernel join a multicast group
        struct ip_mreq mreq;
        mreq.imr_multiaddr.s_addr = inet_addr(mcast->ip);
        mreq.imr_interface.s_addr = htonl(INADDR_ANY);
        rv = setsockopt(mcast->socket,
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
	if (!mcast) return;

	close(mcast->socket);
}


void
mcast_proccess_files(mcast_s *mcast, mcast_proccess_file_cb cb, void *ctx)
{
	struct _mcast_message msg;
	uint8_t data[MCAST_MSG_BUFSIZE];
	sbuf_s *sbuf;

	sbuf = sbuf_new();
	error_if(NULL == sbuf, error, "Memory error");

	// Read til first file from begining
	trace("Reading socket for first complete file...");
	msg.end = 0;
	while (!msg.end) {
		memset(data, 0, MCAST_MSG_BUFSIZE);
		recvfrom(mcast->socket,
			 data,
			 MCAST_MSG_BUFSIZE,
			 0,
			 mcast->addr,
			 &mcast->addrlen);
		_unpack(data, &msg);

		if (msg.end) break;
	}
	mcast_file_s last         = {
		.type = msg.type,
		.id   = msg.id,
		.size = msg.size,
		.data = NULL};
	trace("Last file type is %u - %u", last.type, last.id);

	size_t       cnt          = 0;
	size_t       total_bytes  = 0;
	size_t       bytes        = 0;
	bool         keep_reading = false;
	mcast_file_s file;

	trace("Start reading files");
	while (1) {
		memset(data, 0, MCAST_MSG_BUFSIZE);
		bytes = recvfrom(mcast->socket,
			         data,
			         MCAST_MSG_BUFSIZE,
			         0,
			         mcast->addr,
			         &mcast->addrlen);
	        error_if(bytes < 0, error, "Error reading from socket");
		if (bytes == 0) {
			warn("0 bytes read from socket!!!! finishing reading");
	        	break;
	        }
		total_bytes += bytes;


		_unpack(data, &msg);
		sbuf_appendstr(sbuf, msg.data);
		if (msg.end) {
        		trace("end: %u size: %u type: %u id: %u chunk_num: %u total_chunks= %u sep: %u",
              		      msg.end, msg.size, msg.type, msg.id, msg.chunk_num, msg.total_chunks, msg.sep);

			file.type = msg.type;
			file.id   = msg.id;
			file.size = msg.size;
			file.data = sbuf_ptr(sbuf);
			keep_reading = cb(&file, ctx);

			if (!keep_reading) break;

			sbuf_reset(sbuf);
			cnt ++;

			// finish reading?
			if (msg.type == last.type && msg.id == last.id) break;
		}
	}

	trace("End reading from socket -> %lu bytes in %lu files",
	      total_bytes, cnt);

error:
	if (sbuf) sbuf_delete(sbuf);
	return;
}

/*
#Struct of header - first 12 bytes
# end   xmlsize   type   ?  id        chunk# *10   total chunks     \0
# --   --------   -----  ------  ----  ---------  -------------      --
# 00   00 00 00    F1    X 0 00   00     00 00          00           00
#FIXME: XMLsize print is incorrect
data = socket.recv(1500)
chunk = {}
chunk["end"] = struct.unpack('B',data[:1])[0]
chunk["size"] = struct.unpack('>HB',data[1:4])[0]
chunk["filetype"] = struct.unpack('B',data[4:5])[0]
chunk["fileid"] = struct.unpack('>H',data[5:7])[0]&0x0fff
chunk["chunk_number"] = struct.unpack('>H',data[8:10])[0]/0x10
chunk["chunk_total"] = struct.unpack('B',data[10:11])[0]
chunk["data"] = data[12:]
*/
static void
_unpack(uint8_t *data, struct _mcast_message *mc)
{
        mc->end = (data[0] << 0);
        mc->size = (data[3] << 0) | (data[2] << 8) | (data[1] << 16);
        mc->type = (data[4] << 0);
        mc->id = ((data[6] << 0) | (data[5] << 8)) & 0x0fff;
        mc->chunk_num = ((data[9] << 0) | (data[8] << 8)) & 0x10;
        mc->total_chunks = (data[10] << 0);
        mc->data = (char *)&data[12];
        // Last chunk has 4 non data bytes
        if (mc->end)
        	mc->data[strlen(mc->data) - 5] = 0;
/*
        trace("end: %u size: %u type: %u id: %u chunk_num: %u total_chunks= %u sep: %u",
              mc->end, mc->size, mc->type, mc->id, mc->chunk_num, mc->total_chunks, mc->sep);
        trace("\nDATA (%lu): >>>>>>>>%s<<<<<<\n", strlen(mc->data), mc->data);
*/
}

