/* Disciplina: Programacao Concorrente */
/* Prof.: Silvana Rossetto */
/* Aluno: Henrique Lima Cardoso*/
/* Codigo: Implementação de produtor consumidor usando semaforos (da maneira mais simplificada possível) */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include "pc.h"

char *buffer1;
pc_t pc1;

char *buffer2;
pc_t pc2;

void *t1(void* arq_v){ //ptr para arquivo 
    FILE* arq = (FILE*) arq_v; // já foi aberto pela main
    //primeiro de maneira bobona! letra por letra
    char ch;
    do {
        ch = fgetc(arq);
        pc_insere(&pc1,ch);
    } while (ch != EOF); 
    fclose(arq);
    pthread_exit(NULL);
};

void *t2(void*){
    int n = 0;
    int i = 0;

    while(1){
        char ch = pc_retira(&pc1);
        pc_insere(&pc2,ch);
        i++;
        if(ch == EOF) break;
        if((n <= 10 && i == 2*n + 1) || (n>10 && i%10 == 0)){ //deve ter formas melhores de escrever isso daqui
            pc_insere(&pc2,'\n');
            i=0; 
            n++;
        }
    }
    pthread_exit(NULL);
};

void *t3(void*){
    char ch;
    while(1){
        ch = pc_retira(&pc2);
        if(ch == EOF) break;
        printf("%c",ch);
    };
    
    pthread_exit(NULL);
};

int main(int argc, char* argv[]){
    if(argc < 3){
        printf("uso: %s <tam_buffer> <arq-leitura>\n",argv[0]);
        exit(1);
    }
    int N = atoi(argv[1]);

    pthread_t tids[3];
    FILE *arq = fopen(argv[2],"r");
    if(arq == NULL){
        printf("erro: nao foi possivel abrir %s\n",argv[1]);
        exit(1);
    }

    buffer1 = (char *) malloc(sizeof(char)*N);
    if(buffer1 == NULL){
        printf("erro: nao foi possivel alocar espaco para buffer1\n");
        exit(2);
    }
    buffer2 = (char *) malloc(sizeof(char)*N);
    if(buffer2 == NULL){
        printf("erro: nao foi possivel alocar espaco para buffer2\n");
        exit(2);
    }
    
    pc_init(&pc1,N,buffer1);
    pc_init(&pc2,N,buffer2);

    if(pthread_create(&tids[0],NULL,t1,(void*) arq)){
        printf("erro: nao foi possivel criar t1\n");
        exit(2);
    }

    if(pthread_create(&tids[1],NULL,t2,NULL)){
        printf("erro: nao foi possivel criar t2\n");
        exit(2);
    }

    if(pthread_create(&tids[2],NULL,t3,NULL)){
        printf("erro: nao foi possivel criar t3\n");
        exit(2);
    }

    for(int i = 0; i < 3; i++){
        if(pthread_join(tids[i],NULL)){
            printf("erro: join da thread %d\n",i);
        }
    }


    return 0;
}