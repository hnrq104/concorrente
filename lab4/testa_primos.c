#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
typedef long long ll;
typedef struct {
	pthread_mutex_t *m;
	int *global_index, tam_vet;
	ll *vet;
}t_arg;

typedef struct{
	int primos_lidos;
}t_ret;

int primalidade(ll n){
	if(n <= 1) return 0;
	if(n == 2) return 1;
	if(n%2 == 0) return 0;
	for(int i = 3; i < sqrt(n) + 1; i+=2){
		if(n%i) return 0;
	}	
	return 1;
}

void *testa_primos(void* arg_ptr){
	t_arg *t = (t_arg*) arg_ptr;
	int conta_primos = 0;
	while(1){
		pthread_mutex_lock(t->m);
		int i = *(t->global_index);
		*(t->global_index) += 1;
		pthread_mutex_unlock(t->m);
		if(i >= t->tam_vet) break;
		conta_primos += primalidade(t->vet[i]);
	}
	t_ret *r = malloc(sizeof(t_ret));
	r->primos_lidos = conta_primos;
	pthread_exit((void*) r);
}

int main(int argc, char* argv[]){
	if(argc < 3){
		printf("uso: %s <arq-entrada> <n-threads>\n",argv[0]);
		return 1;
	}
	FILE* arq = fopen(argv[1],"rb");
	if(arq == NULL){
		fprintf(stderr,"erro: ao abrir arq %s\n",argv[1]);
	}

	//lê vetor no arquivo
	int tam_vet;
	fread(&tam_vet,sizeof(int),1,arq);
	ll *vet = malloc(sizeof(ll)*tam_vet);
	fread(vet,sizeof(ll),tam_vet,arq);
	fclose(arq);

	//determina tids do sistema
	int n_thread = atoi(argv[2]);
	pthread_t *tids = malloc(sizeof(pthread_t)*n_thread);

	//inicializa mutex
	pthread_mutex_t mut;
	pthread_mutex_init(&mut, NULL);
	int i_global = 0;
	
	t_arg *t = malloc(sizeof(t_arg)); // o mesmo argumento é para todos
	t->global_index = &i_global;
	t->m = &mut;
	t->tam_vet = tam_vet;
	t->vet = vet;
	
	for(int i = 0; i < n_thread; i++){
		pthread_create(&tids[i],NULL,testa_primos,(void*) t);
	}

	int total_primos = 0;
	for(int i = 0; i < n_thread; i++){
		t_ret *r = NULL;
		if(pthread_join(tids[i],(void*) &r)){
			printf("erro: em pthread_join %d\n",i);
		}
		total_primos += r->primos_lidos;
		free(r);
	}
	free(t);
	
	printf("total de primos: %d\n",total_primos);
	return 0;
}
