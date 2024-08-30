#!/bin/bash
echo NumThreads, Dimensao, TempoTotal, TempoInit, TempoMult, TempoFim > $1

TamMatrix=(500 1000 2000)
NThreads=(1 2 4 8 16)

mkdir -p testes
for i in {1..5}; do
# Vou testar para matrizes diferentes, por isso estou gerando mais vezes
    echo Rodada $i !
    for n in ${TamMatrix[@]}; do
        echo Gerando matrizes de dimensão $n
        ./cods-lab3/geraMatrizBinario $n $n testes/mat$n-1.bin
        ./cods-lab3/geraMatrizBinario $n $n testes/mat$n-2.bin
        for t in ${NThreads[@]}; do
            echo Rodando mult_mat_conc com n = $n e $t threads
            ./mult_mat_conc testes/mat$n-1.bin testes/mat$n-2.bin testes/mult$n.bin $t >> $1
        done
    done
done