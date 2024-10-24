package main

import (
	"flag"
	"fmt"
	"math"
	"os"
	"runtime"
)

func Primo(n int) bool {
	if n <= 1 {
		return false
	}
	if n == 2 {
		return true
	}
	if n%2 == 0 {
		return false
	}
	for i := 3; float64(i) < math.Sqrt(float64(n))+1; i += 2 {
		if n%i == 0 {
			return false
		}
	}
	return true
}

var nMaximoFlag = flag.Int("n", 100000, "num max de teste")
var nGophersFlag = flag.Int("m", runtime.GOMAXPROCS(0), "num max de goroutines")

func main() {
	flag.Parse()
	n, m := *nMaximoFlag, *nGophersFlag

	if flag.NFlag() < 2 {
		fmt.Printf("%s : use -h para ver flags\n", os.Args[0])
		fmt.Printf("Rodando o programa com valores padrão: n = %d, m = %d\n", n, m)
	}

	canalNumeros := make(chan int, m)
	canalContagem := make(chan int, m)

	// começa gophers
	for i := 0; i < m; i++ {
		go func(j int) {
			conta := 0
			ultimoNum := 0
			testados := 0
			for k := range canalNumeros {
				testados++
				ultimoNum = k
				if Primo(k) {
					conta++
				}
			}
			fmt.Printf("gopher %d : ultimo num %d : testados %d\n", j, ultimoNum, testados)
			canalContagem <- conta
		}(i)
	}

	for i := 1; i <= n; i++ {
		canalNumeros <- i
	}

	close(canalNumeros)

	total := 0
	for i := 1; i <= m; i++ {
		total += <-canalContagem
	}

	fmt.Printf("Pi(%d) = %d funcao PI de gauss :) \n ", n, total)

	return
}
