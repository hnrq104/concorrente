# Laboratório 6

Disciplina: Computacao Concorrente
Professora: Silvana Rossetto
Aluno: Henrique 

Codigo: Implementação de um RW lock com prioridade para writers.

Decisões:
    Se colocarmos o valor inicial da lista maior (1000) e não limitarmos o tamanho dos elementos inseridos nela, cada operação de read
    demora significativamente mais e já se torna clara a diferença entre usar um Mutex normal para um ReadWrite Lock (não é o caso para o debugging, mas explica o fenômeno estranho que vimos no laboratório).

    Alterei para que seja 50% escrita, 50% leitura.

Para compilar o programa com/sem logging basta comentar/descomentar o `#define LOGGING 1` em `priority_rw.c` e escrever `make` no terminal.

Um exemplo que prova que nossa prioridade está funcionando pode ser visto aqui (trocarei os nomes das threads para facilitar):

```console
1   prw thread 6: wlock blocked!: nr=1, nrw=1, nww=3
2   prw thread 2: last reader!: nrw=1 nww=3, liberando somente writer!
3   prw thread 0: rlock blocked!: nr=0, nrw=1, nww=3
4   prw thread 4: wlock signaled!: nr=0, nrw=1, nww=3
5   prw thread 4: wunlocking, signalling writers!: nr=0, nrw=1, nww=2
6   prw thread 4: wunlocking, signalling writers!: nr=0, nrw=1, nww=2
7   prw thread 2: rlock blocked!: nr=0, nrw=2, nww=2
8   prw thread 6: wlock signaled!: nr=0, nrw=2, nww=2
9   prw thread 6: wunlocking, signalling writers!: nr=0, nrw=2, nww=1
10  prw thread 6: wunlocking, signalling writers!: nr=0, nrw=2, nww=1
11  prw thread 6: rlock blocked!: nr=0, nrw=3, nww=1
12  prw thread 8: wlock signaled!: nr=0, nrw=3, nww=1
13  prw thread 8: wunlocking, signalling readers!: nr=0, nrw=3, nww=0
```
```
Na linha 1, a thread 6 se adiciona a `number_writers_waiting` e se bloqueia, pois há um leitor.
Na linha 2, o leitor (thread 2) percebe que era o último na estrutura, havia 1 leitor e 3 escritores esperando, ele libera um escritor.
Na linha 3, o leitor (thread 0) é ativado novamente (por um sinal anterior), mas percebe que há escritores esperando e se bloqueia novamente
na linha 4, a thread 4 é liberada para escrever e termina na linha 5.
Antes que qualquer outra thread escritora apareça, a própria thread 4 pega novamente o lock, termina a escrita, e sinaliza para as mesmas threads anteriores que elas podem escrever.
na linha 7, um novo leitor aparece e se bloqueia.
na linha 8, uma escritora (thread 6) é sinalizada e começa a escrever.
na linha 9 e 10, ela termina, pega sequencialmente o lock para escrita, e termina novamente, sinalizando a única thread escritora esperando.
na linha 11, a thread 6 se bloqueia para uma escrita.
na linha 12, a thread 8 - que estava esperando para escrever, adquire o lock
na linha 13 ela termina e sinaliza todos os leitores para se desbloquearem.
```

Outro bom exemplo pode ser visto em output.txt (tive que rodar tantas vezes para dar esse azar!)
