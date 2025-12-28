#include "main.h"          // Header local do projeto (declarações próprias: struct response, write_callback, etc.)
#include <stdio.h>         // printf, fprintf
#include <stdlib.h>        // getenv, malloc, free
#include <curl/curl.h>     // libcurl: HTTP/HTTPS client
#include <jansson.h>       // Jansson: parsing e manipulação de JSON
#include <string.h>

// Função responsável por buscar informações do site do usuário no Neocities
int fetch_neocities_info() {

    // Lê variáveis de ambiente com credenciais
    // NEOCITIES_USER e NEOCITIES_PASS são usadas para autenticação HTTP Basic
    const char *user = getenv("NEOCITIES_USER");
    const char *pass = getenv("NEOCITIES_PASS");
    // const char *enc  = getenv("NEOCITIES_PASS_ENC"); // Não usado aqui (possível uso futuro)

    // Verificação básica: se usuário ou senha não existirem, aborta
    if (!user || !pass) {
        fprintf(stderr, "You're not logged!\n");
        return 1;
    }

    // Buffer para "user:password", formato exigido por CURLOPT_USERPWD
    char auth[256];
    snprintf(auth, sizeof(auth), "%s:%s", user, pass);

    // Inicializa um handle do libcurl
    CURL *curl = curl_easy_init();
    if (!curl) {
        fprintf(stderr, "Erro ao inicializar cURL\n");
        return 1;
    }

    // Monta a URL da API do Neocities para obter informações do site
    char infourl[256];
    snprintf(
        infourl,
        sizeof(infourl),
        "https://neocities.org/api/info?sitename=%s",
        user
    );

    // Define a URL da requisição
    curl_easy_setopt(curl, CURLOPT_URL, infourl);

    // Define autenticação HTTP Basic (user:pass)
    curl_easy_setopt(curl, CURLOPT_USERPWD, auth);

    // Estrutura usada para armazenar a resposta HTTP em memória
    // resp.data será expandido pelo write_callback
    struct response resp = { .data = malloc(1), .len = 0 };
    resp.data[0] = '\0'; // Garante string vazia inicial

    // Função chamada pelo libcurl sempre que dados são recebidos
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);

    // Ponteiro passado para o write_callback (estado da resposta)
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &resp);

    // Executa a requisição HTTP de forma síncrona
    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        fprintf(stderr, "Erro na requisição: %s\n", curl_easy_strerror(res));
        curl_easy_cleanup(curl);
        free(resp.data);
        return 1;
    }

    // Libera o handle do curl (a resposta já está em resp.data)
    curl_easy_cleanup(curl);

    // Estrutura para capturar erros de parsing JSON
    json_error_t error;

    // Converte a string JSON recebida em um objeto JSON manipulável
    json_t *obj = json_loads(resp.data, 0, &error);
    free(resp.data); // Não é mais necessária após o parsing

    // Caso o JSON seja inválido
    if (!obj) {
        fprintf(stderr, "Erro ao parsear JSON: %s\n", error.text);
        return 1;
    }

    // Obtém o campo "result" do JSON raiz
    json_t *result_obj = json_object_get(obj, "result");
    const char *result_str = json_string_value(result_obj);

    // Caso a API tenha retornado erro
    if (strcmp(result_str, "error") == 0) {

        // Tipo de erro retornado pela API
        const char *err_type =
            json_string_value(json_object_get(obj, "error_type"));

        // Mensagem de erro legível
        const char *err_msg =
            json_string_value(json_object_get(obj, "message"));

        // Tratamento específico para erro de autenticação
        if (strcmp(err_type, "invalid_auth") == 0) {
            printf("Usuário ou senha incorretos!\n");
        } else {
            printf("Erro! %s\n", err_msg);
        }

        // Diminui o contador de referência do objeto JSON
        json_decref(obj);
        return 1;
    }

    // Caso a requisição tenha sido bem-sucedida
    if (strcmp(result_str, "success") == 0) {

        // Objeto "info" contém os dados do site
        json_t *info_obj = json_object_get(obj, "info");

        // Impressão direta dos campos retornados
        printf("\nSitename: %s\n",
               json_string_value(json_object_get(info_obj, "sitename")));

        printf("Hits: %d\n",
               (int)json_integer_value(json_object_get(info_obj, "hits")));

        printf("Created at: %s\n",
               json_string_value(json_object_get(info_obj, "created_at")));

        printf("Last updated: %s\n",
               json_string_value(json_object_get(info_obj, "last_updated")));

        printf("Domain: %s\n",
               json_string_value(json_object_get(info_obj, "domain")));

        // Array de tags associadas ao site
        json_t *tags = json_object_get(info_obj, "tags");

        printf("Tags: ");
        size_t index;
        json_t *tag;

        // Itera sobre o array JSON "tags"
        json_array_foreach(tags, index, tag) {
            printf(
                "#%s%s",
                json_string_value(tag),
                (index < json_array_size(tags) - 1) ? ", " : "."
            );
        }
        printf("\n\n");
    }

    // Libera o objeto JSON principal
    json_decref(obj);

    return 0;
}
