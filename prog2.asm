segment .data
	str1 db 'k = %d', 0xA, 0
segment .bss
	i resd 1
	k resd 1
	n resd 1
segment .code
	global _main
	extern _printf
_main:
	; n = 5
	mov dword[n], 5
	; k = 1
	mov dword[k], 1
	;for
	; i = 1
	mov dword[i], 1
	_cycle1:
		; k * i
		; помещаем 2 операнда в стек
		push dword[k]
		push dword[i]
		; помещаем в eax операнд k
		mov eax, dword[esp+4]
		; умножаем на операнд i
		imul dword[esp]
		; записываем результат умножения в k
		mov dword[k], eax
		; убираем 2 операнда с вершины стека
		add esp, 8
		; i++
		inc dword[i]
		; помещаем i в eax
		mov eax, dword[i]
		; сравниваем с n
		cmp eax, dword[n]
		; если i <= n, продолжаем цикл
		jle _cycle1
	; fmt.Print("k = " , k)
	push dword[k]
	push str1
	call _printf
	add esp, 8
	ret