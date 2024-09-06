#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
typedef long long ll;
typedef struct {
	ll *v;
	int id_thread, n_threads, vet_size;
	int mod;
  }t_arg;

void *preenche_vet(void *arg){
	t_arg *t = (t_arg*) arg;
	int start = t->id_thread*(t->vet_size)/(t->n_threads);
	int quant = (t->vet_size)/(t->n_threads);
	if(t->id_thread < t->vet_size%t->n_threads){
		quant += 1;
		start += t->id_thread;
	}
	srand(time(NULL));
	for(int i = start; i < quant; i++){
		t->v[i] = rand()%t->mod;
	}
	free(arg);
	pthread_exit(NULL);	
}

int main(int argc, char* argv[]){
	if(argc < 4){
		printf("uso: %s <numeros> <mod> <arq-saida> <op: n_threads>\n",argv[0]);
		return 1;
	}
	int num = atoi(argv[1]);
	int mod = atoi(argv[2]);
	int n_thread = 1;
	if(argc >= 5){
		n_thread = atoi(argv[4]);
	}
	
	pthread_t *tids = malloc(sizeof(pthread_t)*n_thread);
	ll* vet = malloc(sizeof(ll)*num);
	for(int i = 0; i < n_thread; i++){
		t_arg *t = malloc(sizeof(t_arg));
		t->id_thread = i;
		t->n_threads = n_thread;
		t->v = vet;
		t->vet_size = num;
		t->mod = mod;
		pthread_create(&tids[i],NULL,preenche_vet,(void*) t);
    }
	for(int i = 0; i < n_thread; i++){
		pthread_join(tids[i],NULL);
	}

	FILE* arq = fopen(argv[3],"wb");
	fwrite(&num,sizeof(int),1,arq);
	fwrite(vet,sizeof(ll),num,arq);
	fclose(arq);
	return 0;
}
