package main

import (
	"fmt"
)

var ch_done = make(chan struct{})

func tarefa(ch_str chan string) {
	fmt.Println(<-ch_str, "(impresso pela Goroutine)")
	ch_str <- "Goroutine envia para Main: 'Ola Main, bom dia, tudo bem?'"
	fmt.Println(<-ch_str, "(impresso pela Goroutine)")
	ch_str <- "Goroutine envia para Main: 'Certo, entendido.'"
	fmt.Println("Goroutine imprime: finalizando")
	ch_done <- struct{}{} // envia um token para avisar que finalizou
}

func main() {
	ch_str := make(chan string)
	go tarefa(ch_str)

	ch_str <- "Main para Goroutine: 'Olá Goroutine, bom dia!'"
	fmt.Println(<-ch_str, "(impresso pela Main)")
	ch_str <- "Main para Goroutine: 'Tudo bem! Vou terminar tá?"
	fmt.Println(<-ch_str, "(impresso pela Main)")
	<-ch_done

	fmt.Println("Main imprime: finalizando")

	return
}
