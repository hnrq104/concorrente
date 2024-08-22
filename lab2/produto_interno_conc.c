/* Disciplina: Computacao Concorrente */
/* Aluno: Henrique */
/* Lab2:  */
/* Codigo: Calcula o produto interno de dois vetores de forma concorrente */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>

typedef struct{
    long int N;
    int id,num_t;
    float *u,*v;
}t_arg;

typedef struct{
    double prod;
}t_ret;

void *Produto(void* args_thread){
    t_arg* arg = (t_arg*) args_thread;

    //determina onde comeca e quantos elementos serao calculados
    long int init = arg->id * (arg->N/arg->num_t);
    long int quant = arg->N/arg->num_t;
    if(arg->id < arg->N % arg->num_t){
        init += arg->id;
        quant++;
    } else init += arg->N % arg->num_t;

    //calcula a produto vetorial da parte (podemos fazer isso pois é funcional linear!)
    double prod_parcial = 0;
    for(long int i = 0; i < quant; i++){
        prod_parcial += arg->u[init + i] * arg->v[init + i];
    }
    
    free(args_thread);
    t_ret* r = malloc(sizeof(t_ret));
    if(r == NULL){
        fprintf(stderr,"--ERRO: malloc() para retorno da thread %d\n",arg->id);
    } else r->prod = prod_parcial;

    pthread_exit((void*) r);
}

int main(int argc, char* argv[]){
    if(argc < 3){
        fprintf(stderr,"--ERRO: Informe o numero de threads e o nome do arquivo: %s <T> <arq_path>\n",argv[0]);
        return 1;
    }

    size_t num_threads = atoi(argv[1]);
    pthread_t *tids = malloc(sizeof(pthread_t)*num_threads); 
    if(tids == NULL){
        fprintf(stderr,"--ERRO: malloc() para tids\n");
        return 2;
    }

    FILE* arq = fopen(argv[2],"rb");
    if(arq == NULL){
        fprintf(stderr,"--ERRO: nao foi possivel abrir %s\n",argv[2]);
        return 2;
    }

    //tenta ler dimensao do arquivo
    long int n;
    if(fread(&n,sizeof(long int),1,arq) < 1){
        fprintf(stderr,"--ERRO: fread() ao ler dimesão\n");
        return 2;
    }

    //tenta alocar os dois vetores
    float *u = malloc(n*sizeof(float));
    float *v = malloc(n*sizeof(float));
    if(u == NULL || v == NULL){
        fprintf(stderr,"--ERRO: malloc() para u or v\n");
        return 2;    
    }

    //tenta ler o primeiro vetor
    if(fread(u,sizeof(float),n,arq) < n){
        fprintf(stderr,"--ERRO: fread() para vetor u\n");
        return 2;
    }
    //tenta ler o segundo vetor
    if(fread(v,sizeof(float),n,arq) < n){
        fprintf(stderr,"--ERRO: fread() para vetor u\n");
        return 2;
    }

    //tenta ler produto interno do arq
    double prod_arq;
    if(fread(&prod_arq, sizeof(double),1,arq) < 1){
        fprintf(stderr,"--ERRO: fread() para prod_interno no arq\n");
    }

    for(int i = 0; i < num_threads; i++){
        //aloca argumentos para thread
        t_arg* arg_thread = malloc(sizeof(t_arg));
        if(arg_thread == NULL){
            fprintf(stderr,"--ERRO: malloc() args para thread %d\n",i);
            exit(1);
        }
        arg_thread->id = i;
        arg_thread->num_t = num_threads;
        arg_thread->N = n;
        arg_thread->u = u;
        arg_thread->v = v;
        if(pthread_create(&tids[i], NULL, Produto, (void*) arg_thread)){
            fprintf(stderr,"--ERRO: pthread_create() na thread %d\n",i);
        }
    }

    double prod_conc = 0;
    double prod_seq = 0;
    //calcula o prod_concorrente a partir dos retornos
    for(int i = 0; i < num_threads; i++){
        t_ret *r = NULL;
        if(pthread_join(tids[i],(void*) &r)){
            fprintf(stderr,"--ERRO: pthread_join() da thread %d\n",i);
        }
        prod_conc += r->prod;
        free(r);
    }

    //calcula o prod_sequencial
    for(long int i = 0; i < n; i++){
        prod_seq += u[i]*v[i];
    }

    //variacao relativa
    double var_relativa = fabs((prod_seq - prod_conc)/prod_seq);
    
    //resultados
    printf("\n");
    printf("valor registrado no arquivo (prod_arq): %lf\n",prod_arq);
    printf("valor calculado sequencialmente (prod_seq): %lf\n",prod_seq);
    printf("valor calculado concorrentemente (prod_conc): %lf\n",prod_conc);
    printf("valor calculado concorrentemente (prod_conc): %lf\n",prod_conc);
    printf("variacao relativa calculada entre conc e seq: %lf\n",var_relativa);

    return 0;
}