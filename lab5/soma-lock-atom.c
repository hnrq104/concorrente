/* Disciplina: Programacao Concorrente */
/* Prof.: Silvana Rossetto */
/* Aluno: Henrique Cardoso*/
/* Codigo: Comunicação entre threads usando variável compartilhada e exclusao mutua com bloqueio */

#include <stdio.h>
#include <stdlib.h> 
#include <pthread.h>

#define MOD 10 // É muito interessante testar com valores diferentes de MOD sugiro 1000!
#define QTD_MULTIPLOS 20

long int soma = 0; // variavel compartilhada entre as threads
int printamos = 0; // booleana
long int quem_somou = -1; // isso aqui é só um caviat para printar quem fez a ultima soma
pthread_mutex_t mutex; // variavel de lock para exclusao mutua
pthread_mutex_t mutex_print; // lock para condicao de printar
pthread_cond_t cond_print; //condicao de printar 
pthread_cond_t cond_soma; // condicao de somar

/*Depois de brincar um pouco, percebi que basta um condicional!
  Continuei usando dois para facilitar o entendimento.*/


//funcao executada pelas threads
void *ExecutaTarefa (void *arg) {
  long int id = (long int) arg;
  printf("Thread : %ld esta executando...\n", id);

  for (int i=0; i<100000; i++) {
     //--entrada na SC
     pthread_mutex_lock(&mutex);
     //--SC (seção critica)
     if(soma%MOD == 0 && soma < MOD*QTD_MULTIPLOS){
      pthread_mutex_lock(&mutex_print);
      printamos = 1;
      quem_somou = id;
      pthread_cond_signal(&cond_print);
      pthread_cond_wait(&cond_soma,&mutex_print);
      pthread_mutex_unlock(&mutex_print);
     }
     soma++; //incrementa a variavel compartilhada 
     //--saida da SC
     pthread_mutex_unlock(&mutex);
  }
  printf("Thread : %ld terminou!\n", id);
  pthread_exit(NULL);
}

//funcao executada pela thread de log
void *extra (void *args) {
  printf("Extra : esta executando...\n");
  pthread_mutex_lock(&mutex_print);
  for(int i = 0; i < QTD_MULTIPLOS; i++){
    if(printamos == 0) pthread_cond_wait(&cond_print,&mutex_print);
    printf("soma = %ld; ultima thread = %ld\n",soma,quem_somou);
    printamos = 0;
    pthread_cond_signal(&cond_soma);
  }
  pthread_mutex_unlock(&mutex_print);
  printf("Extra : terminou!\n");
  pthread_exit(NULL);
}

//fluxo principal
int main(int argc, char *argv[]) {
   pthread_t *tid; //identificadores das threads no sistema
   int nthreads; //qtde de threads (passada linha de comando)

   //--le e avalia os parametros de entrada
   if(argc<2) {
      printf("Digite: %s <numero de threads>\n", argv[0]);
      return 1;
   }
   nthreads = atoi(argv[1]);

   //--aloca as estruturas
   tid = (pthread_t*) malloc(sizeof(pthread_t)*(nthreads+1));
   if(tid==NULL) {puts("ERRO--malloc"); return 2;}

   //--inicilaiza o mutex (lock de exclusao mutua)
   pthread_mutex_init(&mutex, NULL);
   pthread_mutex_init(&mutex_print,NULL);

   //--inicializa o cond
   pthread_cond_init(&cond_print,NULL);
   pthread_cond_init(&cond_soma,NULL);


   //--cria thread de log
   if (pthread_create(&tid[nthreads], NULL, extra, NULL)) {
      printf("--ERRO: pthread_create()\n"); exit(-1);
   }

   //--cria as threads
   for(long int t=0; t<nthreads; t++) {
     if (pthread_create(&tid[t], NULL, ExecutaTarefa, (void *)t)) {
       printf("--ERRO: pthread_create()\n"); exit(-1);
     }
   }

   //--espera todas as threads terminarem
   for (int t=0; t<nthreads+1; t++) {
     if (pthread_join(tid[t], NULL)) {
         printf("--ERRO: pthread_join() \n"); exit(-1); 
     } 
   } 

   //--finaliza o mutex
   pthread_mutex_destroy(&mutex);
   
   printf("Valor de 'soma' = %ld\n", soma);

   return 0;
}
