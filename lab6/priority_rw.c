#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "priority_rw.h"

// #define LOGGING 1


void priority_init(priority_rwlock* prw){
    //depois de tentar rodar para conseguir resultados, printar essas
    //adquiricoes é inviavel (fica muito texto)
    
    #ifdef LOGGING
    printf("prw: iniciando rwlock %p\n",prw);
    prw->num_readers_waiting = 0;
    prw->num_writers = 0;
    #endif

    
    prw->num_readers = 0;
    prw->num_writers_waiting = 0;
    pthread_cond_init(&prw->cond_readers,NULL);
    pthread_cond_init(&prw->cond_writers,NULL);
    pthread_mutex_init(&prw->m1,NULL);


}

void priority_destroy(priority_rwlock* prw){
    #ifdef LOGGING
    printf("prw: destruindo rwlock %p\n",prw);
    #endif
    
    pthread_cond_destroy(&prw->cond_readers);
    pthread_cond_destroy(&prw->cond_writers);
    pthread_mutex_destroy(&prw->m1);
}

void priority_rlock(priority_rwlock* prw){
    pthread_mutex_lock(&prw->m1);
    prw->num_readers_waiting++;
    while(prw->num_writers_waiting){
        #ifdef LOGGING
            printf("prw thread %lu: rlock blocked!: nr=%d, nrw=%d, nww=%d\n",pthread_self(),prw->num_readers,prw->num_readers_waiting,prw->num_writers_waiting);
        #endif
        
        pthread_cond_wait(&prw->cond_readers,&prw->m1);
    }
    prw->num_readers_waiting--;
    prw->num_readers++;
    pthread_mutex_unlock(&prw->m1);
}

void priority_runlock(priority_rwlock* prw){
    pthread_mutex_lock(&prw->m1);
    prw->num_readers--;
    if(prw->num_readers == 0 && prw->num_writers_waiting){
        #ifdef LOGGING
            printf("prw thread %lu: last reader!: nrw=%d nww=%d, liberando somente writer!\n",pthread_self(),prw->num_readers_waiting,prw->num_writers_waiting);
        #endif
        pthread_cond_signal(&prw->cond_writers);
    }
    pthread_mutex_unlock(&prw->m1);
}

void priority_wlock(priority_rwlock* prw){
    pthread_mutex_lock(&prw->m1);
    prw->num_writers_waiting++;
    while(prw->num_readers){
        #ifdef LOGGING
            printf("prw thread %lu: wlock blocked!: nr=%d, nrw=%d, nww=%d\n",pthread_self(),prw->num_readers,prw->num_readers_waiting,prw->num_writers_waiting);
        #endif
        pthread_cond_wait(&prw->cond_writers,&prw->m1);
        #ifdef LOGGING
            printf("prw thread %lu: wlock signaled!: nr=%d, nrw=%d, nww=%d\n",pthread_self(),prw->num_readers,prw->num_readers_waiting,prw->num_writers_waiting);
        #endif
    }
    prw->num_writers_waiting--;
}

void priority_wunlock(priority_rwlock* prw){
    if(prw->num_writers_waiting){
        #ifdef LOGGING
            printf("prw thread %lu: wunlocking, signalling writers!: nr=%d, nrw=%d, nww=%d\n",pthread_self(),prw->num_readers,prw->num_readers_waiting,prw->num_writers_waiting);
        #endif
        pthread_cond_signal(&prw->cond_writers);
    } else if(prw->num_readers_waiting) {
        #ifdef LOGGING
            printf("prw thread %lu: wunlocking, signalling readers!: nr=%d, nrw=%d, nww=%d\n",pthread_self(),prw->num_readers,prw->num_readers_waiting,prw->num_writers_waiting);
        #endif
        pthread_cond_broadcast(&prw->cond_readers);
    }
    pthread_mutex_unlock(&prw->m1);
}

