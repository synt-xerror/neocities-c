#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <jansson.h>

#include "main.h"
#include "info.h"

struct response {
    char *data;
    size_t len;
};

size_t write_callback(void *data, size_t size, size_t nmemb, void *userdata) {
    struct response *resp = (struct response *)userdata;
    size_t chunk_size = size * nmemb;

    char *tmp = realloc(resp->data, resp->len + chunk_size + 1);
    if (!tmp) {
        fprintf(stderr, "Erro de memória\n");
        return 0;
    }
    resp->data = tmp;
    memcpy(resp->data + resp->len, data, chunk_size);
    resp->len += chunk_size;
    resp->data[resp->len] = '\0';
    return chunk_size;
}

int main(int argc, char *argv[]) {
    printf("\nNeocities C CLI\n");

    if (argc < 2 || strcmp(argv[1], "--info") != 0) {
        printf("\nnothing to do.\n");
        return 0;
    }

    return fetch_neocities_info();
}
