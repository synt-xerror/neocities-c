#ifndef INFO
#define INFO
// ^ Guardas de inclusão (include guards)
// Evitam que este header seja processado mais de uma vez
// durante a compilação, o que causaria redefinições.

// Declaração da função que busca informações do Neocities.
// O `void` indica explicitamente que a função não recebe parâmetros.
int fetch_neocities_info(void);

#endif
// ^ Fim do include guard
