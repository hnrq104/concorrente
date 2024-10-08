//Programa concorrente que cria e faz operacoes sobre uma lista de inteiros

#include <stdio.h>
#include <stdlib.h>
#include "cods-lab6/list_int.h"
#include "cods-lab6/timer.h"
#include <pthread.h>

#include "priority_rw.h" //minha prw

#define QTDE_OPS 1000 //quantidade de operacoes sobre a lista (insercao, remocao, consulta)
#define QTDE_INI 1000 //quantidade de insercoes iniciais na lista
#define MAX_VALUE 1000000 //valor maximo a ser inserido

//IMPORTANTE! MUDAR MAX_VALUE faz o que queremos!

//lista compartilhada iniciada 
struct list_node_s* head_p = NULL; 
//qtde de threads no programa
int nthreads;
//lock de exclusao mutua
// pthread_mutex_t mutex;

priority_rwlock prw;

//tarefa das threads
void* tarefa(void* arg) {
    long int id = (long int) arg;
    int op;
    int in, out, read; 
    in=out=read = 0; 
    srand(time(NULL));
    //realiza operacoes de consulta (98%), insercao (1%) e remocao (1%)
    for(long int i=id; i<QTDE_OPS; i+=nthreads) {
        op = rand()%4; // só para o logging!
        if(op>1) {
            // pthread_mutex_lock(&mutex);
            
            priority_rlock(&prw);

            Member(i%MAX_VALUE, head_p);   /* Ignore return value */
            // pthread_mutex_unlock(&mutex);

            priority_runlock(&prw);

            read++;
        } else if(op==1) {
	        // pthread_mutex_lock(&mutex);  

            priority_wlock(&prw);

            Insert(i%MAX_VALUE, &head_p);  /* Ignore return value */
	        // pthread_mutex_unlock(&mutex);

            priority_wunlock(&prw);

	        in++;
        } else if(op==0) {
	        // pthread_mutex_lock(&mutex);

            priority_wlock(&prw);

            Delete(i%MAX_VALUE, &head_p);  /* Ignore return value */
	        // pthread_mutex_unlock(&mutex);     
	        
            priority_wunlock(&prw);
            out++;
        }
    }
    //registra a qtde de operacoes realizadas por tipo
    printf("Thread %ld: in=%d out=%d read=%d\n", id, in, out, read);
    pthread_exit(NULL);
}

/*-----------------------------------------------------------------*/
int main(int argc, char* argv[]) {
    pthread_t *tid;
    double ini, fim, delta;
    
    //verifica se o numero de threads foi passado na linha de comando
    if(argc<2) {
        printf("Digite: %s <numero de threads>\n", argv[0]); return 1;
    }
    nthreads = atoi(argv[1]);

    //insere os primeiros elementos na lista
    for(int i=0; i<QTDE_INI; i++)
        Insert(i%MAX_VALUE, &head_p);  /* Ignore return value */
    
    //aloca espaco de memoria para o vetor de identificadores de threads no sistema
    tid = malloc(sizeof(pthread_t)*nthreads);
    if(tid==NULL) {  
        printf("--ERRO: malloc()\n"); return 2;
    }

    //inicializa a variavel mutex
    // pthread_mutex_init(&mutex, NULL);

    //incializa a minha prw
    priority_init(&prw);
    
    //tomada de tempo inicial
    GET_TIME(ini);
    //cria as threads
    for(long int i=0; i<nthreads; i++) {
        if(pthread_create(tid+i, NULL, tarefa, (void*) i)) {
            printf("--ERRO: pthread_create()\n"); return 3;
        }
    }
    
    //aguarda as threads terminarem
        for(int i=0; i<nthreads; i++) {
            if(pthread_join(*(tid+i), NULL)) {
                printf("--ERRO: pthread_join()\n"); return 4;
            }
        }

    //tomada de tempo final
    GET_TIME(fim);
    delta = fim-ini;
    printf("Tempo: %lf\n", delta);

    //libera o mutex
    // pthread_mutex_destroy(&mutex);

    //libera o prw
    priority_destroy(&prw);

    //libera o espaco de memoria do vetor de threads
    free(tid);
    //libera o espaco de memoria da lista
    Free_list(&head_p);

    return 0;
}  /* main */

