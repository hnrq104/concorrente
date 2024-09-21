#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>

typedef struct{
    int t_id;
    int n_threads;
    int n;
}t_arg;

typedef struct{
    long double sum;
}t_ret;

void *calc(void *ptr){
    t_arg* param = (t_arg*) ptr;
    int id = param->t_id;
    int n = param->n;
    int n_threads = param->n_threads;
    
    long double factor = pow(16,(double) -id);
    long double fact_mult = pow(16,(double) -n_threads);
    // printf("factor: %LF, fact_mult = %LF\n",factor,fact_mult);

    long double sum = 0;
    for(long double i = id; i < n; i+=n_threads){
        long double values = (4/(8*i + 1)) - (2/(8*i + 4)) - (1/(8*i + 5)) - (1/(8*i + 6));
        sum += values*factor;
        factor *= fact_mult;
    }
    free(ptr);
    t_ret* r = malloc(sizeof(t_ret));
    r->sum = sum;

    pthread_exit((void*) r);

}

int main(int argc, char* argv[]){
    if(argc < 3){
        printf("uso: %s <n> <n-threads>\n",argv[0]);
        exit(1);
    }

    int n = atoi(argv[1]);
    int n_threads = atoi(argv[2]);

    pthread_t* tids = malloc(sizeof(pthread_t)*n_threads);
    for(int i = 0; i < n_threads; i++){
        t_arg* param = malloc(sizeof(t_arg));
        param->n = n;
        param->n_threads = n_threads;
        param->t_id = i;
        pthread_create(&tids[i],NULL,calc,(void*) param);
    }

    long double pi = 0;
    for(int i = 0; i < n_threads; i++){
        t_ret* ret_ptr;
        pthread_join(tids[i],(void*) &ret_ptr);
        pi += ret_ptr->sum;
        free(ret_ptr);
    }

    printf("%0.20Lf\n",pi);

    return 0;
}