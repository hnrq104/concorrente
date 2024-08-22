/* Disciplina: Computacao Concorrente */
/* Aluno: Henrique */
/* Lab2:  */
/* Codigo: Gera dois vetores de entrada do tipo float com N elementos aleatórios */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX 1000 //valor maximo de um elemento do vetor
//descomentar o define abaixo caso deseje imprimir uma versao do vetor gerado no formato texto
// #define TEXTO 

int main(int argc, char*argv[]) {
    float *vetor_u, *vetor_v; //vetores gerados
    long int n; //qtde de elementos do vetor que serao gerados
    double prod_interno=0; //soma total dos elementos gerados
    FILE * descritorArquivo; //descritor do arquivo de saida
    size_t ret; //retorno da funcao de escrita no arquivo de saida

    //recebe os argumentos de entrada
    if(argc < 3) {
            fprintf(stderr, "Digite: %s <dimensao> <arquivo saida>\n", argv[0]);
            return 1;
    }
    n = atoi(argv[1]);

    //aloca memoria para o vetor
    vetor_u = (float*) malloc(sizeof(float) * n);
    if(!vetor_u) {
        fprintf(stderr, "Erro de alocacao da memoria do primeiro vetor\n");
        return 2;
    }

    vetor_v = (float*) malloc(sizeof(float) * n);
    if(!vetor_v){
        fprintf(stderr, "Erro de alocacao da memoria do segundo vetor\n");
        return 2;
    }

    //preenche o vetor com valores float aleatorios
    srand(time(NULL));
    for(long int i=0; i<n; i++) {
        vetor_u[i] = (rand() % MAX)/3.0 * (2*(rand()%2) - 1);
        vetor_v[i] = (rand() % MAX)/3.0 * (2*(rand()%2) - 1);
        prod_interno += vetor_u[i]*vetor_v[i];
    }

    //imprimir na saida padrao o vetor gerado
    #ifdef TEXTO
    fprintf(stdout, "%ld\n", n);
    fprintf(stdout, "Vetor u: ");
    for(long int i=0; i<n; i++) {
        fprintf(stdout, "%f ",vetor_u[i]);
    }
    fprintf(stdout, "\nVetor v: ");
    for(long int i=0; i<n; i++) {
        fprintf(stdout, "%f ",vetor_v[i]);
    }
    
    fprintf(stdout, "\n");
    fprintf(stdout, "produto interno = %lf\n", prod_interno);
    #endif

    //escreve o vetor no arquivo
    //abre o arquivo para escrita binaria
    descritorArquivo = fopen(argv[2], "wb");
    if(!descritorArquivo) {
        fprintf(stderr, "Erro de abertura do arquivo\n");
        return 3;
    }
    //escreve a dimensao
    ret = fwrite(&n, sizeof(long int), 1, descritorArquivo);
    //escreve os elementos do vetor
    ret = fwrite(vetor_u, sizeof(float), n, descritorArquivo);
    if(ret < n) {
        fprintf(stderr, "Erro de escrita no  arquivo\n");
        return 4;
    }
    
    ret = fwrite(vetor_v, sizeof(float), n, descritorArquivo);
    if(ret < n) {
        fprintf(stderr, "Erro de escrita no  arquivo\n");
        return 4;
    }

    //escreve o somatorio
    ret = fwrite(&prod_interno, sizeof(double), 1, descritorArquivo);
    if(ret < 1) {
        fprintf(stderr, "Erro de escrita no  arquivo\n");
        return 4;
    }
    
    //finaliza o uso das variaveis
    fclose(descritorArquivo);
    free(vetor_u);
    free(vetor_v);
    return 0;
} 
