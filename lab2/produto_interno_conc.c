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
    float prod_f; // para salvar o valor em float também!
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
    float prod_float = 0;
    for(long int i = 0; i < quant; i++){
        prod_parcial += arg->u[init + i] * arg->v[init + i];
        prod_float += arg->u[init + i] * arg->v[init + i];
    }
    
    t_ret* r = malloc(sizeof(t_ret));
    if(r == NULL){
        fprintf(stderr,"--ERRO: malloc() para retorno da thread %d\n",arg->id);
    } else {
        r->prod = prod_parcial;
        r->prod_f = prod_float;
    }
    free(args_thread);
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
    if(fread(u,sizeof(float),n,arq) < (size_t) n){
        fprintf(stderr,"--ERRO: fread() para vetor u\n");
        return 2;
    }
    //tenta ler o segundo vetor
    if(fread(v,sizeof(float),n,arq) < (size_t) n){
        fprintf(stderr,"--ERRO: fread() para vetor u\n");
        return 2;
    }

    //tenta ler produto interno do arq
    double prod_arq;
    if(fread(&prod_arq, sizeof(double),1,arq) < 1){
        fprintf(stderr,"--ERRO: fread() para prod_interno no arq\n");
    }

    for(size_t i = 0; i < num_threads; i++){
        //aloca argumentos para thread
        t_arg* arg_thread = malloc(sizeof(t_arg));
        if(arg_thread == NULL){
            fprintf(stderr,"--ERRO: malloc() args para thread %ld\n",i);
            exit(1);
        }
        arg_thread->id = i;
        arg_thread->num_t = num_threads;
        arg_thread->N = n;
        arg_thread->u = u;
        arg_thread->v = v;
        if(pthread_create(&tids[i], NULL, Produto, (void*) arg_thread)){
            fprintf(stderr,"--ERRO: pthread_create() na thread %ld\n",i);
        }
    }

    //alta precisao
    double prod_conc = 0;
    double prod_seq = 0;

    //baixa precisao
    float prod_conc_f = 0;
    float prod_seq_f = 0;

    //calcula o prod_concorrente a partir dos retornos
    for(size_t i = 0; i < num_threads; i++){
        t_ret *r = NULL;
        if(pthread_join(tids[i],(void*) &r)){
            fprintf(stderr,"--ERRO: pthread_join() da thread %ld\n",i);
        }
        prod_conc += r->prod;
        prod_conc_f += r->prod_f;
        free(r);
    }

    //calcula o prod_sequencial
    for(long i = 0; i < n; i++){
        prod_seq += u[i]*v[i];
        prod_seq_f += u[i]*v[i];
    }

    //variacao relativa
    double var_relativa = fabs((prod_seq - prod_conc)/prod_seq);
    float var_relativa_f = fabsf((prod_seq_f - prod_conc_f)/prod_seq_f);

    //resultados
    printf("Valor no arquivo:     %lf\n",prod_arq);
    printf("Sequencial  (double): %lf\n",prod_seq);
    printf("Sequencial  (float) : %f\n",prod_seq_f);
    printf("Concorrente (double): %lf\n",prod_conc);
    printf("Concorrente (float) : %f\n",prod_conc_f);
    printf("Variancia   (double): %lf\n",var_relativa);
    printf("Variancia   (float) : %f\n",var_relativa_f);
    return 0;
}