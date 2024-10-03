#eu não sei fazer makefile :)

all: objFolder atv3

atv3: objects/priority_rw.o objects/list_int.o cods-lab6/timer.h
	gcc atividade3.c objects/priority_rw.o objects/list_int.o -Wall -Wextra -O3 -o atv3

objFolder:
	@ mkdir -p objects

objects/priority_rw.o: priority_rw.c priority_rw.h
	gcc priority_rw.c -c -Wall -Wextra -O3 -o objects/priority_rw.o

objects/list_int.o: cods-lab6/list_int.c cods-lab6/list_int.h
	gcc cods-lab6/list_int.c -c -Wall -Wextra -O3 -o objects/list_int.o

