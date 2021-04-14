package main
import "fmt"
import "os"

func main() {
	var fubs, fib1, fib2, i, n int
	fib1 = 1
	fib2 = 1
	n = 5
	fubs = 0
	for i = 2; i <= n; i++{
		fubs = fib1 + fib2
		fib1 = fib2
		fib2 = fubs
	}
	fmt.Print("Count = " , fubs)
}



