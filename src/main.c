#include <stdio.h>     // printf, fprintf
#include <stdlib.h>    // malloc, realloc, free
#include <string.h>    // strcmp, memcpy
#include <curl/curl.h> // libcurl (necessário para callbacks e tipos)

#include "main.h"      // Declarações globais do projeto (upload_func, etc.)
#include "info.h"      // Declaração de fetch_neocities_info()

// Estrutura usada para acumular a resposta HTTP em memória.
// É preenchida incrementalmente pelo write_callback.
struct response {
    char *data;   // Buffer dinâmico com os dados recebidos
    size_t len;   // Quantidade atual de bytes válidos em data
};

// Callback chamado pelo libcurl sempre que um novo bloco de dados chega.
// Não garante tamanho fixo nem chamada única.
size_t write_callback(void *data, size_t size, size_t nmemb, void *userdata) {

    // userdata aponta para a struct response passada via CURLOPT_WRITEDATA
    struct response *resp = (struct response *)userdata;

    // Tamanho real do bloco recebido
    size_t chunk_size = size * nmemb;

    // Realoca o buffer para caber os novos dados + '\0'
    char *tmp = realloc(resp->data, resp->len + chunk_size + 1);
    if (!tmp) {
        // Se realloc falhar, aborta a escrita
        // Retornar 0 sinaliza erro ao libcurl
        fprintf(stderr, "Erro de memória\n");
        return 0;
    }

    // Atualiza o ponteiro com o novo buffer
    resp->data = tmp;

    // Copia o bloco recebido para o final do buffer atual
    memcpy(resp->data + resp->len, data, chunk_size);

    // Atualiza o tamanho total acumulado
    resp->len += chunk_size;

    // Garante que o buffer seja uma string C válida
    resp->data[resp->len] = '\0';

    // Retornar o número de bytes processados indica sucesso
    return chunk_size;
}

// Tipo de ponteiro para função que representa um comando CLI.
// Todas as funções de comando retornam int e não recebem argumentos.
typedef int (*cmd_func_t)(void);

// Estrutura que associa:
// - nome do comando (string digitada no terminal)
// - função que implementa esse comando
typedef struct {
    const char *name;
    cmd_func_t func;
} command_entry;

int main(int argc, char *argv[]) {

    // Tabela de comandos suportados pelo programa.
    // Facilita adicionar novos comandos sem alterar a lógica principal.
    command_entry commands[] = {
        {"--info",   fetch_neocities_info},
        // {"--upload", upload_func}
    };

    // Verifica se ao menos um argumento foi passado
    if (argc < 2) {
        printf("No command provided.\n");
        return 0;
    }

    // Percorre a tabela de comandos procurando correspondência
    for (size_t i = 0; i < sizeof(commands) / sizeof(commands[0]); i++) {

        // Compara o argumento passado com o nome do comando
        if (strcmp(argv[1], commands[i].name) == 0) {

            // Executa a função associada ao comando
            // e retorna imediatamente o código dela
            return commands[i].func();
        }
    }

    // Se nenhum comando conhecido foi encontrado
    printf("Unknown command.\n");
    return 0;
}
