#include <pthread.h>

typedef struct {
    int num_writers_waiting; // how many writers waiting
    int num_readers; //how many readers are on at the moment
    
    int num_readers_waiting; // a fim de logging
    int num_writers; //variavel desnecessária!
    
    
    pthread_mutex_t m1; // mutex
    pthread_cond_t cond_writers; // wait queue for writers
    pthread_cond_t cond_readers; // wait queue for readers

} priority_rwlock;

void priority_init(priority_rwlock* prw);
void priority_destroy(priority_rwlock* prw);
void priority_rlock(priority_rwlock* prw);
void priority_runlock(priority_rwlock* prw);
void priority_wlock(priority_rwlock* prw);
void priority_wunlock(priority_rwlock* prw);

