#include <stdlib.h>
#include <stdio.h>
#include <semaphore.h>
#include "pc.h"

/**
//pc serve para produtor consumidor!
typedef struct{
    int tam_buffer;
    char* buffer;
    int in; //thread consumidora pega daqui!
    int out; //thread produtora produz aqui!
    
    //se fosse com void*, deveria enviar o sizeof(type)
    //ou enviar uma funcao para void
    //tam tipo_elem

    //produtor preenche tudo e retira depois
    sem_t sem_produtor;
    sem_t sem_consumidor;
}pc_t;
*/

//FAZER COM CHUNKS DEPOIS!

void pc_init(pc_t* pc, int tam_buffer, char* buffer){
    pc->tam_buffer = tam_buffer;
    pc->buffer = buffer;
    
    //posso fazer com um mutex simples também (deve ficar bem mais rápido)
    //iniciando o semaforo_cons com 1 e sempre preenchendo ao máximo.
    sem_init(&pc->sem_produtor,0,tam_buffer);
    sem_init(&pc->sem_consumidor,0,0); 

    pc->out = 0;
    pc->in = 0;
}

void pc_destroy(pc_t *pc){
    sem_destroy(&pc->sem_produtor);
    sem_destroy(&pc->sem_consumidor);
}

void pc_insere(pc_t *pc, char c){
    sem_wait(&pc->sem_produtor);
    pc->buffer[pc->out] = c;
    pc->out = (pc->out+1)%pc->tam_buffer;
    sem_post(&pc->sem_consumidor);
}

char pc_retira(pc_t *pc){
    sem_wait(&pc->sem_consumidor);
    char c = pc->buffer[pc->in];
    pc->in = (pc->in + 1)%pc->tam_buffer;
    sem_post(&pc->sem_produtor);
    return c;
}
