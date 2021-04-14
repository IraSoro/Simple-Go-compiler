package main

import "fmt"
import "os"
import "math"


func main() {
	var a, b, c, D, x1, x2 float64
	//fmt.Print("Введите a,b,c\n")
    //fmt.Fscan(os.Stdin, &a) 
	//fmt.Fscan(os.Stdin, &b) 
	//fmt.Fscan(os.Stdin, &c) 
	a = 1
	b = 3
	c = 1
	D = b*b-4*a*c	
	if D>0{
		x1 = -b+math.Sqrt(D)
		x2 = -b-math.Sqrt(D)
		x1 = x1/2/a
		x2 = x2/2/a
		fmt.Print("x1 = " , x1)
		fmt.Print(" x2 = " , x2)
	} 
	else if D==0 {
		x1 = -b
		x1 = x1/2/a
		fmt.Print("x = " , x1)
	} 
	else{
		fmt.Print("No" )
	}
	
}
