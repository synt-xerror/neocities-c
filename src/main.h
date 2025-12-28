#ifndef MAIN_H
#define MAIN_H
// Include guard:
// Impede múltiplas inclusões deste header no mesmo processo de compilação.

#include <stddef.h> // size_t
// Necessário para o tipo size_t, usado em tamanhos e contadores de memória.

// Declarações externas de variáveis globais.
// O `extern` indica que a definição real existe em outro arquivo .c.
extern const char *user;
extern const char *pass;

// Estrutura usada para armazenar dados recebidos dinamicamente,
// geralmente como resposta de uma requisição HTTP.
struct response {
    char *data;   // Buffer dinâmico contendo os dados acumulados
    size_t len;   // Quantidade de bytes válidos em data
};

// Protótipo da função de callback usada pelo libcurl.
// Essa função será chamada repetidamente conforme dados chegam.
size_t write_callback(
    void *ptr,      // Ponteiro para os dados recebidos
    size_t size,    // Tamanho de cada elemento
    size_t nmemb,   // Número de elementos
    void *userdata  // Ponteiro de contexto definido pelo usuário
);

#endif
// Fim do include guard
