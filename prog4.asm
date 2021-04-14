segment .data
	str1 db 'Count = %d', 0xA, 0
segment .bss
	fubs resd 1
	fib1 resd 1
	fib2 resd 1
	i resd 1
	n resd 1
segment .code
	global _main
	extern _printf
_main:
	; fib1 = 1
	mov dword[fib1], 1
	; fib2 = 1
	mov dword[fib2], 1
	; n = 5
	mov dword[n], 5
	; fubs = 0
	mov dword[fubs], 0
	; for
	; i = 2
	mov dword[i], 2
	_cycle1:
		; fib + fib2
		push dword[fib1]
		push dword[fib2]
		mov eax, dword[esp+4]
		add eax, dword[esp]
		add esp, 8
		; fubs = 
		mov dword[fubs], eax
		; fib1 = fib2
		mov eax, dword[fib2]
		mov dword[fib1], eax
		; fib2 = fubs
		mov eax, dword[fubs]
		mov dword[fib2], eax
		; i <= n
		inc dword[i]
		mov eax, dword[i]
		cmp eax, dword[n]
		jle _cycle1
	; fmt.Print("Count = " , fubs)
	push dword[fubs]
	push str1
	call _printf
	add esp, 8
	; выход
	ret