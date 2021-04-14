package main
import "fmt"
import "os"


func main() {
	var i, k, n int
	n = 5
	//fmt.Fscan(os.Stdin, &n)
	k = 1
	for i=1; i <= n; i++ {
		k = k*i
	}
	fmt.Print("k = " , k)	
}
