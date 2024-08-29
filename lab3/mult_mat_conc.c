/* Disciplina: Computacao Concorrente */
/* Aluno: Henrique */
/* Lab3*/
/* Codigo: Multiplica duas matrizes sequencialmente*/
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

typedef struct{
    int linhas,colunas;
    float* m;
}mat;

typedef struct{
    int num_threads, id_thread;
    mat A,B,*C;
}t_arg;

t_arg* thread_arg(int num_threads, int id_thread, mat A, mat B, mat *C){
    t_arg* arg = malloc(sizeof(t_arg));
    if(arg){
        arg->num_threads = num_threads;
        arg->id_thread = id_thread;
        arg->A = A;
        arg->B = B;
        arg->C = C;
    };
    return arg;
}


void *Multiplica_Thread_Guardando(void* v_arg){
    //A minha thread vai guardar o vetor coluna de B(para ser mais rápido)
    // é mais rápido sim :)
    t_arg* arg = (t_arg*) v_arg;
    mat A = arg->A,B = arg->B ,*C = arg->C;
    int l = A.linhas, c = B.colunas;
    int tam = A.colunas;
    C->linhas = l;
    C->colunas = c;
    
    for(int j = arg->id_thread; j < c; j += arg->num_threads){
        float* coluna_B = malloc(sizeof(float) * tam);
        for(int k = 0; k < tam; k++){
            coluna_B[k] = B.m[k*c + j];
        }
        
        for(int i = 0; i < l; i++){
            C->m[i*c + j] = 0;
            for(int k = 0; k < tam; k++){
                C->m[i*c + j] += A.m[i*tam + k] * coluna_B[k];
            }
        }
    }
    // printf("thread %d acabou\n",arg->id_thread);
    free(arg);
    pthread_exit(NULL);
}

void *Multiplica_Thread(void* v_arg){
    //Sem guardar o vetor, fazendo acessos longinquos
    t_arg* arg = (t_arg*) v_arg;
    mat A = arg->A,B = arg->B ,*C = arg->C;
    int l = A.linhas, c = B.colunas;
    int tam = A.colunas;
    C->linhas = l;
    C->colunas = c;
    
    for(int j = arg->id_thread; j < c; j += arg->num_threads){
        for(int i = 0; i < l; i++){
            C->m[i*c + j] = 0;
            for(int k = 0; k < tam; k++){
                C->m[i*c + j] += A.m[i*tam + k] * B.m[k*c + j];
            }
        }
    }
    free(v_arg);
    pthread_exit(NULL);
}


int main(int argc, char* argv[]){
    clock_t init = clock();
    //s arq1 arq2 arqsaida
    if(argc < 5){
        printf("Uso: %s <arq-matriz-entrada> <arq-matriz-entrada> <arq-saida> <n-threads>\n",argv[0]);
        return 1;
    }

    FILE* arq_mat_1 = fopen(argv[1],"rb");
    FILE* arq_mat_2 = fopen(argv[2],"rb");

    FILE* files[2] = {arq_mat_1,arq_mat_2};
    mat input[2];

    for(int i = 0; i < 2; i++){
        int l,c;
        fread(&l,sizeof(int),1,files[i]);
        fread(&c,sizeof(int),1,files[i]);
        input[i].m = malloc(l*c*sizeof(float));        
        fread(input[i].m,sizeof(float),(size_t) l*c,files[i]);
        input[i].linhas = l;
        input[i].colunas = c;
        fclose(files[i]);
    }
    
    if(input[0].colunas != input[1].linhas){
        fprintf(stderr,"erro: as matrizes [%s] [%s] sao incompativeis\n",argv[1],argv[2]);
        return 2;
    }

    mat saida;
    saida.linhas = input[0].linhas;
    saida.colunas = input[1].colunas;
    saida.m = malloc(sizeof(float) * saida.linhas * saida.colunas);

    int num_threads = atoi(argv[4]);
    pthread_t *tids = malloc(sizeof(pthread_t) * num_threads);
    
    init = clock() - init;
    printf("tempo para inicializar: %lf\n",(double)init /CLOCKS_PER_SEC);

    clock_t process = clock();
    for(int i = 0; i < num_threads; i++){
        t_arg *arg = thread_arg(num_threads,i,input[0],input[1],&saida);
        if(!arg){
            fprintf(stderr,"erro: nao foi possivel alocar espaco para thread %d\n",i);
            return 3;
        }
        if(pthread_create(&tids[i],NULL,Multiplica_Thread_Guardando,(void*) arg)){
            fprintf(stderr,"erro: erro em pthread_create para %d\n",i);
            return 3;
        }
    }
    for(int i = 0; i < num_threads; i++){
        if(pthread_join(tids[i],NULL)){
            fprintf(stderr,"erro: erro em pthread_join para %d\n",i);
        }
    }

    process = clock() - process;
    printf("tempo para multiplicar: %lf\n",(double)process/CLOCKS_PER_SEC);

    clock_t fin = clock();
    FILE* arq_saida = fopen(argv[3],"wb");
    fwrite(&saida.linhas,sizeof(int),1,arq_saida);
    fwrite(&saida.colunas,sizeof(int),1,arq_saida);
    fwrite(saida.m,sizeof(float), (size_t)saida.linhas*saida.colunas, arq_saida);
    fclose(arq_saida);
    fin = clock() - fin;
    printf("tempo para finalizar: %lf\n",(double)fin/CLOCKS_PER_SEC);
    clock_t total = init + process + fin;
    printf("tempo total do programa: %lf\n",(double)total/CLOCKS_PER_SEC);
    
    return 0;
}   