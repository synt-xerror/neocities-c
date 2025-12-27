#ifndef MAIN_H
#define MAIN_H

#include <stddef.h> // para size_t

extern const char *user;
extern const char *pass;

struct response {
    char *data;
    size_t len;
};

size_t write_callback(void *ptr, size_t size, size_t nmemb, void *userdata);

#endif
