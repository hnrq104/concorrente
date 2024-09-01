#!/bin/bash
#mandar o nome do arquivo em $1 (log.csv)!
EXPECTED_ARGS=1
if [ $# -ne $EXPECTED_ARGS ]; then
    echo "uso: $0 <saida.csv>"
    exit 1
fi

echo NumThreads,Dimensao,TempoTotal,TempoInit,TempoMult,TempoFim > $1

TamMatrix=(250 500 1000 2000)
NThreads=(1 2 4 8 12 16)

mkdir -p testes
# Vou testar para matrizes diferentes, por isso estou gerando mais vezes
for n in ${TamMatrix[@]}; do
    echo Gerando matrizes de dimensão $n
    ./cods-lab3/geraMatrizBinario $n $n testes/mat$n-1.bin
    ./cods-lab3/geraMatrizBinario $n $n testes/mat$n-2.bin
    for t in ${NThreads[@]}; do
        for i in {1..5}; do
            echo Rodando mult_mat_conc com n = $n e $t threads vez $i
            ./mult_mat_conc testes/mat$n-1.bin testes/mat$n-2.bin testes/mult$n.bin $t >> $1
        done
    done
done
