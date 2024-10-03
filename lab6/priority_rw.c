#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "priority_rw.h"

#define LOGGING 1

/*
typedef struct {
    int num_writers_waiting; // how many writers waiting
    int num_readers; //how many readers are on at the moment
    int num_writers; //should only be 0 or 1
    pthread_mutex_t m1; // mutex
    pthread_cond_t cond_writers; // wait queue for writers
    pthread_cond_t cond_readers; // wait queue for readers

} priority_rwlock;
*/

void priority_init(priority_rwlock* prw){
    //depois de tentar rodar para conseguir resultados, printar essas
    //adquiricoes é inviavel (fica muito texto)
    
    /*
    #ifdef LOGGING
    printf("prw: iniciando rwlock %p\n",prw);
    #endif
    */
    
    prw->num_readers = 0;
    prw->num_writers = 0;
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
    // free(prw);
}

void priority_rlock(priority_rwlock* prw){
    /*
    #ifdef LOGGING
        printf("prw thread %lu: trying to get read lock\n",pthread_self());
    #endif
    */

    pthread_mutex_lock(&prw->m1);
    while(prw->num_writers || prw->num_writers_waiting){
        #ifdef LOGGING
            printf("prw thread %lu: rlock blocked!: nw=%d, nww=%d\n",pthread_self(),prw->num_writers,prw->num_writers_waiting);
        #endif
        
        pthread_cond_wait(&prw->cond_readers,&prw->m1);
    }

    /*
    #ifdef LOGGING
        printf("prw thread %lu: rlock aquired!\n",pthread_self());
    #endif
    */

    prw->num_readers++;
    pthread_mutex_unlock(&prw->m1);
}

void priority_runlock(priority_rwlock* prw){
    pthread_mutex_lock(&prw->m1);
    /*
    #ifdef LOGGING
        printf("prw thread %lu: unlocking!\n",pthread_self());
    #endif
    */
    
    prw->num_readers--;
    if(prw->num_readers == 0){
        pthread_cond_signal(&prw->cond_writers);
    }
    pthread_mutex_unlock(&prw->m1);
}

void priority_wlock(priority_rwlock* prw){
    pthread_mutex_lock(&prw->m1);

    /*
    #ifdef LOGGING
        printf("prw thread %lu: trying to get wlock\n",pthread_self());
    #endif
    */
    
    prw->num_writers_waiting++;
    while(prw->num_writers || prw->num_readers){
        #ifdef LOGGING
            printf("prw thread %lu: wlock blocked!: nw=%d, nr=%d\n",pthread_self(),prw->num_writers,prw->num_readers);
        #endif
     
        pthread_cond_wait(&prw->cond_writers,&prw->m1);
    }

    /*
    #ifdef LOGGING
        printf("prw thread %lu: wlock aquired!\n",pthread_self());
    #endif
    */

    prw->num_writers_waiting--;
    prw->num_writers = 1;
}

void priority_wunlock(priority_rwlock* prw){
    /*
    #ifdef LOGGING
        printf("prw thread %lu: unlocking!\n",pthread_self());
    #endif
    */

    prw->num_writers = 0;
    if(prw->num_writers_waiting){
        pthread_cond_signal(&prw->cond_writers);
    } else {
        pthread_cond_broadcast(&prw->cond_readers);
    }
    pthread_mutex_unlock(&prw->m1);
}

