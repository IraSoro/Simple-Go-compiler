package main

import "fmt"

func main() {
	var len, len2 int
	var array1 = []int{2,4,1,5,3,6}
	len = 6
	len2 = len - 1
	binary_sorting(array1 , 0 , len2)
	fmt.Print(array1)
}


func binary_sorting(array []int, b int, N int){
	var left, right, lefrig, oporniy, temp, val int
	left = b
	right = N
	lefrig = left+right
	val = lefrig/2
	oporniy = array[val]
	for left <= right{
		for array[left] > oporniy{
			left ++
		}
		for array[right] < oporniy{
			right--
		}
		if left<=right{
			temp = array[left]
			array[left] = array[right]
			array[right] = temp
			left++
			right--
		}
	}
	
	if b<right{
		binary_sorting(array, b, right)
	}
	if N>left{
		binary_sorting(array, left, N)
	}
}


