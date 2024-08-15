/* Aluno: Henrique Lima Cardoso */
/* Laboratório: 1 */
/* Código: Cria um vetor de N elementos inicializados com 0 e os incrementa em 1 usando M threads*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

typedef struct {
    int N,M,idThread;
    int *vec;
}t_arg;

void *Soma(void *args){
    t_arg arg = *((t_arg*)args);
    int alteracoes = arg.N/arg.M;
    //As primeiras N%M threads, somarão 1 elemento a mais.
    if(arg.idThread < arg.N%arg.M) alteracoes++;
    
    int start = arg.idThread * (arg.N/arg.M) /*+ min(arg.idThread,arg.N%arg.M)*/;
    if(arg.idThread < arg.N%arg.M){
        start += arg.idThread;
    } else start += arg.N%arg.M;
    
    for(int i = 0; i < alteracoes; i++){
        arg.vec[start + i]++;
    }
    free(args);
    printf("Thread[%d] finalizou sua operação\n",arg.idThread);
    pthread_exit(NULL);
}

//Verifica retorna 0 se o vetor for composto somente de 1's, e retorna 1 caso contrário. 
int Verifica(int* arr, int N){
    printf("--Verificando se o vetor está correto...\n");
    int err = 0;
    for(int i = 0; i < N; i++){
        if(arr[i] != 1){
            printf("--ERRO: Pos arr[%d] = %d\n",i,arr[i]);
            err = 1;
        }
    }
    return err;
}

int main(int argc, char *argv[]){
    if(argc < 3){
        printf("ERRO: Informe os inteiros N e M: %s <N> <M>\n",argv[0]);
        return 1;
    }
    t_arg *args;
    int N = atoi(argv[1]);
    int M = atoi(argv[2]);
    int *arr = calloc(N,sizeof(int)); //calloc inicializa com 0 por padrão!
    pthread_t *tid_sistema = malloc(M*sizeof(pthread_t));

    for(int i = 0; i < M; i++){
        printf("--Aloca e preenche argumentos para thread %d\n", i);
        args = malloc(sizeof(t_arg));
        if (args == NULL) {
            printf("--ERRO: malloc()\n"); 
            return 1;
        }
        args->idThread = i; 
        args->N = N;
        args->M = M;
        args->vec = arr;
        printf("--Cria a thread %d\n", i);
        if (pthread_create(&tid_sistema[i], NULL, Soma, (void*) args)) {
            printf("--ERRO: pthread_create()\n"); 
            return 2;
        }
    }

    //espera todas as threads terminarem
    for (int i=0; i<M; i++) {
        if (pthread_join(tid_sistema[i], NULL)) {
            printf("--ERRO: pthread_join() da thread %d\n", i); 
        } 
    }
    printf("--Threads foram unidas...\n");
    /*Essa parte foi colocada depois para testar N grande, antes eu só printava o vetor (e debugguei assim!)*/
    if(Verifica(arr,N)){
        printf("O vetor não foi somado corretamente :(\n");
    } else printf("O vetor foi somado corretamente!\n");

    return 0;    
}