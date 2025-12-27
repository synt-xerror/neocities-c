#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>

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

typedef int (*cmd_func_t)(void);

typedef struct {
    const char *name;
    cmd_func_t func;
} command_entry;

int main(int argc, char *argv[]) {
    command_entry commands[] = {
        {"--info", fetch_neocities_info},
        {"--upload", upload_func}
    };

    if (argc < 2) {
        printf("No command provided.\n");
        return 0;
    }

    for (size_t i = 0; i < sizeof(commands)/sizeof(commands[0]); i++) {
        if (strcmp(argv[1], commands[i].name) == 0) {
            return commands[i].func();
        }
    }

    printf("Unknown command.\n");
    return 0;
}

