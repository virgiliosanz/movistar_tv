#ifndef __movistartv_h__
#define __movistartv_h__

#include <stdbool.h>

/* http://www.embedded.com/design/programming-languages-and-tools/4215552/Seventeen-steps-to-safer-C-code */

enum _mtv_errors {
    MTV_SUCCESS = 0,
    MTV_UNKNOWN_ERROR,
    MTV_INVALID_INPUT,
    MTV_FILE_NOT_FOUND,
    MTV_NETWORK_ERROR,
    MTV_PARSING_ERROR,
    MTV_ALREADY_INIT,

    MTV_NOT_IMPLEMENTED,
    MTV_LAST_ERROR
};
typedef enum _mtv_errors mtv_error_t;

struct _mtv_errors_desc {
    int   code;
    char *message;
} mtv_error_desc[] = {
    { MTV_SUCCESS,         "No error" },
    { MTV_UNKNOWN_ERROR,   "Unknown error" },
    { MTV_INVALID_INPUT,   "Invalid input" },
    { MTV_FILE_NOT_FOUND,  "File not found" },
    { MTV_NETWORK_ERROR,   "Network error" },
    { MTV_PARSING_ERROR,   "Parsing error" },
    { MTV_ALREADY_INIT,    "Object already initialized" },

    { MTV_NOT_IMPLEMENTED, "Not Implemented Yet" },
    { MTV_LAST_ERROR,      "Not a real error." },
};


struct _mtv_conf_t {
    bool initilized;

    unsigned short demarcation;
    char         **tvpackages;
    char          *cast_grp_start;
    unsigned short mcast_port;
};
typedef struct _mtv_conf_t mtv_conf_t;

mtv_error_t mtv_load_conf(mtv_conf_t *conf);

#endif

