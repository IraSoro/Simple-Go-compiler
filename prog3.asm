segment .data
	array1 dd 2, 4, 1, 5, 3, 6
	sz dd 6
	res db '%d', 0xA, 0
	str1 db '%d ', 0
segment .bss
	len resd 1
	len2 resd 1
	array resd 1
	b resd 1
	N resd 1
	left resd 1
	right resd 1
	lefrig resd 1
	oporniy resd 1
	temp resd 1
	val resd 1
	print_i resd 1
segment .code
	global _main
	extern _printf
_main:
	; len=6
	mov dword[len], 6
	mov eax, dword[len]
	; len2 = len - 1
	sub eax, 1
	mov dword[len2], eax
	
	; помещаем параметры в стек
	push array1
	push 0
	push dword[len2]
	; вызываем сортировку
	call _binary_sorting
	add esp, 12
	
	mov dword[print_i], 0
	_print_arr_cycle:
		mov edi, dword[print_i]
		mov eax, [array1+4*edi]
		push eax
		push str1
		call _printf
		add esp, 8
		inc dword[print_i]
		mov eax, dword[print_i]
		cmp eax, dword[sz]
		jl _print_arr_cycle
	ret
_binary_sorting:
	; получение параметров
	; берем N
	mov eax, [esp+4]
	mov dword[N], eax
	; берем b
	mov eax, [esp+8]
	mov dword[b], eax
	; берем array
	mov eax, [esp+12]
	mov dword[array], eax
	
	; начало тела функции
	; left = b
	mov eax, dword[b]
	mov dword[left], eax
	; right = N
	mov eax, dword[N]
	mov dword[right], eax
	; lefrig = left + right
	mov eax, dword[left]
	add eax, dword[right]
	mov dword[lefrig], eax
	; val = lefrig/2
	mov eax, dword[lefrig]
	mov bh, 2
	div bh
	mov byte[val], al
	; oporniy = array[val]
	mov edi, dword[val]
	mov ebx, dword[array]
	mov eax, dword[ebx+4*edi]
	mov dword[oporniy], eax

	; for left <= right
	jmp _cycle1
	_cycle1:
	; условие left <= right
	_cycle1_condition:
		mov eax, dword[left]
		cmp eax, dword[right]
		jle _cycle1_block
		jmp _cycle1_end
	; блок первого for
	_cycle1_block:
		; fщr array[left] > oporniy
		jmp _cycle2
		_cycle2:
		; условие array[left] > oporniy
		_cycle2_condition:
			; проверка условия
			mov edi, dword[left]
			mov ebx, dword[array]
			mov eax, dword[ebx+4*edi]
			cmp eax, dword[oporniy]
			jg _cycle2_block
			jmp _cycle2_end
		_cycle2_block:
			; блок for
			inc dword[left]	
			jmp _cycle2
		_cycle2_end:
		; for array[right] < oporniy
		jmp _cycle3
		_cycle3:
		; условие array[right] < oporniy
		_cycle3_condition:
			; проверка условия
			mov edi, dword[right]
			mov ebx, dword[array]
			mov eax, dword[ebx+4*edi]
			cmp eax, dword[oporniy]
			jl _cycle3_block
			jmp _cycle3_end
		_cycle3_block:
			; блок for
			dec dword[right]	
			jmp _cycle3
		_cycle3_end:
		; if left <= right
		jmp _if1_cond1
		_if1_cond1:
			; условие if
			mov eax, dword[left]
			cmp eax, dword[right]
			jle _if1_cond1_block
			jmp _if1_end
		_if1_cond1_block:
			; блок if
			; temp = array[left]
			mov edi, dword[left]
			mov ebx, dword[array]
			mov eax, dword[ebx+4*edi]
			mov dword[temp], eax
			; array[left] = array[right]
			; берем array[right]
			mov edi, dword[right]
			mov ebx, dword[array]
			mov eax, [ebx+4*edi]
			; присваиваем array[left]
			mov edi, dword[left]
			mov ebx, dword[array]
			mov [ebx+4*edi], eax
			; array[right] = temp
			mov edi, dword[right]
			mov ebx, dword[array]
			mov eax, dword[temp]
			mov [ebx+4*edi], eax
			; left++
			inc dword[left]
			; right--
			dec dword[right]
			jmp _if1_end
		_if1_end:
		jmp _cycle1
	_cycle1_end:
	
	; if b < right
	jmp _if2_cond
	_if2_cond:
		mov eax, dword[b]
		cmp eax, dword[right]
		jl _if2_cond_block
		jmp _if2_end
	_if2_cond_block:
		; binary_sorting(array, b, right)
		; помещаем параметры функции в стек
		push dword[array]
		push dword[b]
		push dword[right]
		call _binary_sorting
		add esp, 12
	_if2_end:
	; if N > left
	jmp _if3_cond
	_if3_cond:
		mov eax, dword[N]
		cmp eax, dword[left]
		jg _if3_cond_block
		jmp _if3_end
	_if3_cond_block:
		; binary_sorting(array, b, right)
		; помещаем текущее значение параметров в стек
		push dword[array]
		push dword[left]
		push dword[N]
		call _binary_sorting
		add esp, 12
	_if3_end:
	ret
	