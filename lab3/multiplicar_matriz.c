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

void Multiplica_Guardando(mat A, mat B, mat *C){
    int l = A.linhas;
    int c = B.colunas;
    int tam = A.colunas; // = B.linhas
    C->linhas = l;
    C->colunas = c;
    for(int j = 0; j < c; j++){
        float* B_coluna = malloc(sizeof(float)*tam);
        if(!B_coluna){
            printf("erro: ao mallocar espaco continguo para coluna %d da segunda matriz\n",j);
            exit(4);
        }
        for(int k = 0; k < tam; k++){
            B_coluna[k] = B.m[k*c + j];
        }
        for(int i = 0; i < l; i++){
            C->m[i*c + j] = 0;
            for(int k = 0; k < tam; k++){
                C->m[i*c + j] += A.m[i*tam + k]*B_coluna[k];
            }
        }
        free(B_coluna);
    }
}


int main(int argc, char* argv[]){
    //s arq1 arq2 arqsaida
    if(argc < 4){
        printf("Uso: %s <arq-matriz-entrada> <arq-matriz-entrada> <arq-saida>\n",argv[0]);
        return 5;
    }

    FILE* arq_mat_1 = fopen(argv[1],"rb");
    if(!arq_mat_1){
        printf("erro: ao abrir arquivo %s\n",argv[1]);
        return 1;
    }
    FILE* arq_mat_2 = fopen(argv[2],"rb");
    if(!arq_mat_1){
        printf("erro: ao abrir arquivo %s\n",argv[1]);
        return 1;
    }

    FILE* files[2] = {arq_mat_1,arq_mat_2};
    mat input[2];

    for(int i = 0; i < 2; i++){
        int l,c;
        if(fread(&l,sizeof(int),1,files[i]) < 1){
            printf("erro: ao ler num de linhas da mat %d\n",i);
            return 1;
        }
        if(fread(&c,sizeof(int),1,files[i]) < 1){
            printf("erro: ao ler num de colunas da mat %d\n",i);
            return 1;    
        }
        input[i].m = malloc(l*c*sizeof(float));        
        if(!input[i].m){
            printf("erro: malloc para matriz %d\n",i);
            return 2;
        }
        if(fread(input[i].m,sizeof(float),(size_t) l*c,files[i]) < (size_t) l*c){
            printf("erro: ao ler valores da mat %d\n",i);
        }
        input[i].linhas = l;
        input[i].colunas = c;
        fclose(files[i]);
    }
    
    if(input[0].colunas != input[1].linhas){
        printf("erro: as matrizes [%s] [%s] sao incompativeis\n",argv[1],argv[2]);
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


    // int num_threads = stoi(argv[4]);
    // pthread_t *tids = malloc(sizeof(pthread_t) * num_threads);
    
    Multiplica(input[0],input[1],&saida);
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

    return 0;
}   