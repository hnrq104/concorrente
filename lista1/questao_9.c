#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

int aux;
pthread_cond_t pergunta;
pthread_mutex_t mut;
int true_aux = 0;

void* T0(void* args){
    pthread_mutex_lock(&mut);
    true_aux = 1;
    printf("thread 0: olá, você está acessando a variável 'aux' agora?\n");
    pthread_cond_signal(&pergunta);
    pthread_cond_wait(&pergunta,&mut);
    printf("thread 0: certo, então vou alterá-la, tá?\n");
    pthread_cond_signal(&pergunta);
    pthread_cond_wait(&pergunta,&mut);
    printf("thread 0: terminei a alteração da variável 'aux'\n");
    pthread_cond_signal(&pergunta);
    pthread_mutex_unlock(&mut);
    pthread_exit(NULL);
}

void* T1(void* args){
    pthread_mutex_lock(&mut);
    if(true_aux == 0) pthread_cond_wait(&pergunta,&mut);
    printf("thread 1: oi, não estou!\n");
    pthread_cond_signal(&pergunta);
    pthread_cond_wait(&pergunta,&mut);
    printf("thread 1: tudo bem\n");
    pthread_cond_signal(&pergunta);
    pthread_cond_wait(&pergunta,&mut);
    printf("thread 1: perfeito, recebido!\n");
    pthread_mutex_unlock(&mut);
    pthread_exit(NULL);
}

int main(void){
    pthread_t tids[2];
    pthread_mutex_init(&mut,NULL);
    pthread_cond_init(&pergunta,NULL);
    pthread_create(&tids[0],NULL,T0,NULL);
    pthread_create(&tids[1],NULL,T1,NULL);
    pthread_join(tids[0],NULL);
    pthread_join(tids[1],NULL);
    pthread_cond_destroy(&pergunta);
    pthread_mutex_destroy(&mut);
    exit(0);
}