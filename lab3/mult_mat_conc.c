/* Disciplina: Computacao Concorrente */
/* Aluno: Henrique */
/* Lab3*/
/* Codigo: Multiplica duas matrizes concorrentemente*/
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>

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
        if(!coluna_B){
            fprintf(stderr,"erro: malloc continuo para coluna %d da segunda matriz\n",j);
            exit(4);
        }
        for(int k = 0; k < tam; k++){
            coluna_B[k] = B.m[k*c + j];
        }
        
        for(int i = 0; i < l; i++){
            C->m[i*c + j] = 0;
            for(int k = 0; k < tam; k++){
                C->m[i*c + j] += A.m[i*tam + k] * coluna_B[k];
            }
        }
        free(coluna_B);
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
    struct timeval start,finish;
    gettimeofday(&start,0);

    if(argc < 5){
        printf("Uso: %s <arq-matriz-entrada> <arq-matriz-entrada> <arq-saida> <n-threads>\n",argv[0]);
        return 5;
    }

    FILE* arq_mat_1 = fopen(argv[1],"rb");
    if(!arq_mat_1){
        fprintf(stderr,"erro: ao abrir arquivo %s\n",argv[1]);
        return 1;
    }
    FILE* arq_mat_2 = fopen(argv[2],"rb");
    if(!arq_mat_1){
        fprintf(stderr,"erro: ao abrir arquivo %s\n",argv[1]);
        return 1;
    }
    FILE* files[2] = {arq_mat_1,arq_mat_2};
    mat input[2];

    for(int i = 0; i < 2; i++){
        int l,c;
        if(fread(&l,sizeof(int),1,files[i]) < 1){
            fprintf(stderr,"erro: ao ler num de linhas da mat %d\n",i);
            return 1;
        }
        if(fread(&c,sizeof(int),1,files[i]) < 1){
            fprintf(stderr,"erro: ao ler num de colunas da mat %d\n",i);
            return 1;    
        }
        input[i].m = malloc(l*c*sizeof(float));        
        if(!input[i].m){
            fprintf(stderr,"erro: malloc para matriz %d\n",i);
            return 2;
        }
        if(fread(input[i].m,sizeof(float),(size_t) l*c,files[i]) < (size_t) l*c){
            fprintf(stderr,"erro: ao ler valores da mat %d\n",i);
        }
        input[i].linhas = l;
        input[i].colunas = c;
        fclose(files[i]);
    }
    
    if(input[0].colunas != input[1].linhas){
        fprintf(stderr,"erro: as matrizes [%s] [%s] sao incompativeis\n",argv[1],argv[2]);
        return 3;
    }

    mat saida;
    saida.linhas = input[0].linhas;
    saida.colunas = input[1].colunas;
    saida.m = malloc(sizeof(float) * saida.linhas * saida.colunas);
    if(!saida.m){
        printf("erro: malloc para a matriz de saida\n");
        return 2;
    }

    int num_threads = atoi(argv[4]);
    pthread_t *tids = malloc(sizeof(pthread_t) * num_threads);
    if(!tids){
        printf("erro: malloc para os tids\n");
        return 2;
    }
    //aqui a gente finaliza a contagem de tempo da inicialização
    gettimeofday(&finish,0);
    double elapsed_init = finish.tv_sec - start.tv_sec + (finish.tv_usec - start.tv_usec)*1e-6;

    gettimeofday(&start,0);
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
    //aqui finalizamos a contagem para a multiplicacao
    gettimeofday(&finish,0);
    double elapsed_processo = finish.tv_sec - start.tv_sec + (finish.tv_usec - start.tv_usec)*1e-6;
    gettimeofday(&start,0);

    FILE* arq_saida = fopen(argv[3],"wb");
    if(!arq_saida){
        printf("erro: ao abrir o arquivo de saida %s\n",argv[3]);
        return 1;
    }
    if(fwrite(&saida.linhas,sizeof(int),1,arq_saida) < 1){
        printf("erro: ao escrever linhas da matriz de saida\n");
        return 1;
    }
    if(fwrite(&saida.colunas,sizeof(int),1,arq_saida) < 1){
        printf("erro: ao escrever colunas da matriz de saida\n");
        return 1;
    }

    size_t saida_tam = saida.linhas*saida.colunas;
    if(fwrite(saida.m,sizeof(float), saida_tam, arq_saida) < saida_tam){
        printf("erro: ao escrever valores da matriz de saida\n");
        return 1;    
    }
    fclose(arq_saida);

    free(input[0].m);
    free(input[1].m);
    free(saida.m);
    
    gettimeofday(&finish,0);
    double elapsed_finalizacao = finish.tv_sec - start.tv_sec + (finish.tv_usec - start.tv_usec)*1e-6;
    double elapsed_total = elapsed_init + elapsed_processo + elapsed_finalizacao;
    
    //printa os tempos! 
    //Num de threads, dimensoes matrizes, total, init, mult, fim
    printf("%d, %d, %lf, %lf, %lf, %lf\n",num_threads,saida.colunas, 
        elapsed_total, elapsed_init,elapsed_processo, elapsed_finalizacao);

    return 0;
}   