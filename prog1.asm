;prog1
segment .data
	const1 dd 4.0
	const2 dd 0.0
	const3 dd 2.0
	const4 dd 2.0
	const5 dd 2.0
	minus_one dd -1.0
	str1 db 'x1 = %f', 0xA, 0
	str2 db 'x2 = %f', 0xA, 0
	str3 db 'x = %f', 0xA, 0
	str4 db 'No', 0xA, 0
segment .bss
	a resd 1
	b resd 1
	c resd 1
	D resd 1
	x1 resd 1
	x2 resd 1
segment .code
	global _main
	extern _printf
_main:
	; иницилизация переменных
	mov dword[a], __float32__(1.0)
	mov dword[b], __float32__(3.0)
	mov dword[c], __float32__(1.0)
	
	; D = b*b - 4 * a * c
	; b*b
	fld dword[b]
	fld dword[b]
	fmul
	;4*a*c
	fld dword[const1]
	fld dword[a]
	fmul
	fld dword[c]
	fmul
	;b*b - 4*a*c
	fsub
	;Сохраняем результат в D
	fstp dword[D]
	; if
	; помещаем 0 в eax
	mov eax, dword[const2]
	; сравниваем D с 0
	cmp dword[D], eax
	; if D > 0
	jg _if1_cond1
	; else if D == 0
	je _if1_cond2
	; else
	jmp _if1_cond3
	; D > 0
	_if1_cond1:
		; x1 = -b + sqrt(D)
		fld dword[b]
		fld dword[minus_one]
		fmul
		fld dword[D]
		fsqrt
		fadd
		fstp dword[x1]
		; x2 = -b - sqrt(D)
		fld dword[b]
		fld dword[minus_one]
		fmul
		fld dword[D]
		fsqrt
		fsub
		fstp dword[x2]
		; x1 = x1/2/a
		fld dword[x1]
		fdiv dword[const3]
		fdiv dword[a]
		fstp dword[x1]
		; x2 = x2/2/a
		fld dword[x2]
		fdiv dword[const4]
		fdiv dword[a]
		fstp dword[x2]
		; fmt.Print("x1 = " , x1)
		sub esp, 8
		fld dword[x1]
		fstp qword[esp]
		push str1
		call _printf
		add esp, 12
		; fmt.Print(" x2 = " , x2)
		sub esp, 8
		fld dword[x2]
		fstp qword[esp]
		push str2
		call _printf
		add esp, 12
		; выход из if
		jmp _if1_endif
	; D == 0
	_if1_cond2:
		; x1 = -b
		fld dword[b]
		fmul dword[minus_one]
		fstp dword[x1]
		; x1/2/a
		fld dword[x1]
		fdiv dword[const5]
		fdiv dword[a]
		fstp dword[x1]
		; fmt.Print("x = " , x1)
		sub esp, 8
		fld dword[x1]
		fstp qword[esp]
		push str3
		call _printf
		add esp, 12
		; выход из if
		jmp _if1_endif
	_if1_cond3:
		push str4
		call _printf
		add esp, 4
		; выход из if
		jmp _if1_endif
	_if1_endif:
	; завершение работы программы
	ret