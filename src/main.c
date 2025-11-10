#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <jansson.h>

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

int fetch_neocities_info() {
    const char *user = getenv("NEOCITIES_USER");
    const char *pass = getenv("NEOCITIES_PASS");
    if (!user || !pass) {
        fprintf(stderr, "Variáveis NEOCITIES_USER ou NEOCITIES_PASS não definidas!\n");
        return 1;
    }

    char auth[256];
    snprintf(auth, sizeof(auth), "%s:%s", user, pass);

    CURL *curl = curl_easy_init();
    if (!curl) {
        fprintf(stderr, "Erro ao inicializar cURL\n");
        return 1;
    }

    char infourl[256];
    snprintf(infourl, sizeof(infourl), "https://neocities.org/api/info?sitename=%s", user);

    curl_easy_setopt(curl, CURLOPT_URL, infourl);
    curl_easy_setopt(curl, CURLOPT_USERPWD, auth);

    struct response resp = { .data = malloc(1), .len = 0 };
    resp.data[0] = '\0';

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &resp);

    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        fprintf(stderr, "Erro na requisição: %s\n", curl_easy_strerror(res));
        curl_easy_cleanup(curl);
        free(resp.data);
        return 1;
    }
    curl_easy_cleanup(curl);

    json_error_t error;
    json_t *obj = json_loads(resp.data, 0, &error);
    free(resp.data);

    if (!obj) {
        fprintf(stderr, "Erro ao parsear JSON: %s\n", error.text);
        return 1;
    }

    json_t *result_obj = json_object_get(obj, "result");
    const char *result_str = json_string_value(result_obj);

    if (strcmp(result_str, "error") == 0) {
        const char *err_type = json_string_value(json_object_get(obj, "error_type"));
        const char *err_msg = json_string_value(json_object_get(obj, "message"));

        if (strcmp(err_type, "invalid_auth") == 0) {
            printf("Usuário ou senha incorretos!\n");
        } else {
            printf("Erro! %s\n", err_msg);
        }
        json_decref(obj);
        return 1;
    }

    if (strcmp(result_str, "success") == 0) {
        json_t *info_obj = json_object_get(obj, "info");
        printf("\nSitename: %s\n", json_string_value(json_object_get(info_obj, "sitename")));
        printf("Hits: %d\n", (int)json_integer_value(json_object_get(info_obj, "hits")));
        printf("Created at: %s\n", json_string_value(json_object_get(info_obj, "created_at")));
        printf("Last updated: %s\n", json_string_value(json_object_get(info_obj, "last_updated")));
        printf("Domain: %s\n", json_string_value(json_object_get(info_obj, "domain")));

        json_t *tags = json_object_get(info_obj, "tags");
        printf("Tags: ");
        size_t index;
        json_t *tag;
        json_array_foreach(tags, index, tag) {
            printf("#%s%s", json_string_value(tag), (index < json_array_size(tags) - 1) ? ", " : ".");
        }
        printf("\n\n");
    }

    json_decref(obj);
    return 0;
}

int main(int argc, char *argv[]) {
    printf("\nNeocities C CLI\n");

    if (argc < 2 || strcmp(argv[1], "--info") != 0) {
        printf("\nnothing to do.\n");
        return 0;
    }

    return fetch_neocities_info();
}
