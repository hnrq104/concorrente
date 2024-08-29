/* Disciplina: Computacao Concorrente */
/* Aluno: Henrique */
/* Lab3*/
/* Codigo: Multiplica duas matrizes sequencialmente*/
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

typedef struct{
    int linhas,colunas;
    float* m;
}mat;

// typedef struct{
//     int num_threads, id_thread;
//     mat A,B,C;
// }t_arg;


// typedef struct{
//     int l,c;
//     float* linhas;
//     float* colunas;
// }mat_seq; //duplica linhas e colunas para manter sequencialidade

void Multiplica(mat A, mat B, mat *C){
    int l = A.linhas;
    int c = B.colunas;
    int tam = A.colunas; // = B.linhas
    C->linhas = l;
    C->colunas = c;
    for(int i = 0; i < l; i++){
        for(int j = 0; j < c; j++){
            C->m[i*c + j] = 0;
            for(int k = 0; k < tam; k++){
                C->m[i*c + j] += A.m[i*tam + k]*B.m[k*c + j];
            }
        }
    }
}


int main(int argc, char* argv[]){
    //s arq1 arq2 arqsaida
    if(argc < 4){
        printf("Uso: %s <arq-matriz-entrada> <arq-matriz-entrada> <arq-saida>\n",argv[0]);
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
        printf("erro: as matrizes [%s] [%s] sao incompativeis\n",argv[1],argv[2]);
        return 2;
    }

    mat saida;
    saida.linhas = input[0].linhas;
    saida.colunas = input[1].colunas;
    saida.m = malloc(sizeof(float) * saida.linhas * saida.colunas);


    // int num_threads = stoi(argv[4]);
    // pthread_t *tids = malloc(sizeof(pthread_t) * num_threads);
    
    Multiplica(input[0],input[1],&saida);
    FILE* arq_saida = fopen(argv[3],"wb");
    fwrite(&saida.linhas,sizeof(int),1,arq_saida);
    fwrite(&saida.colunas,sizeof(int),1,arq_saida);
    fwrite(saida.m,sizeof(float), (size_t)saida.linhas*saida.colunas, arq_saida);
    fclose(arq_saida);

    return 0;
}   