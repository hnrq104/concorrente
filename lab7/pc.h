#include <semaphore.h>

//VERSÃO MAIS SIMPLES (um char de cada vez)

//pc serve para produtor consumidor!
typedef struct{
    int tam_buffer;
    char* buffer;
    int in; //thread consumidora pega daqui!
    int out; //thread produtora produz aqui!
    
    //tem como fazer de maneira geral, enviando ponteiros para funcoes (horroroso)

    //produtor preenche tudo e retira depois
    sem_t sem_produtor;
    sem_t sem_consumidor;
}pc_t;

//inicializa produtor_consumidor, com um buffer e o tamanho dele
void pc_init(pc_t* pc, int tam_buffer, char* buffer);

//destroy produtor_consumidor, liberando os semaforos, nao da free no buffer
void pc_destroy(pc_t *pc);

//insere um elemento de cada vez
void pc_insere(pc_t *pc, char c);

//remove um elemento de cada vez
char pc_retira(pc_t *pc);