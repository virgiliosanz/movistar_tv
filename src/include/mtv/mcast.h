#ifndef __multicast_h__
#define __multicast_h__

// Para leer del multicast de movistar:
// Hay que crear la estructura mcast y abrir el socket
// Según se vayan leyendo ficheros se llamará al callback:
// mcast_proccess_file_cb
// Se recibirán los datos del espacio de memoria "ctx" que se pase a
// mcast_process_files

struct mcast_file {
	uint8_t  type;
	uint32_t id;
	uint32_t size;
	char    *data;
};

struct mcast;

struct mcast *mcast_alloc();
void mcast_free(struct mcast *mcast);
void mcast_open(struct mcast *mcast, const char *ip, const int port);
void mcast_close(struct mcast *mcast);
void mcast_debug(struct mcast *mcast);

typedef bool (*mcast_proccess_file_cb) (const struct mcast_file *file, void *ctx);
void mcast_proccess_files(struct mcast *mcast, mcast_proccess_file_cb cb, void *ctx);


#endif

