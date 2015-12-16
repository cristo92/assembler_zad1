section .text
global parse, unparse, suma, aless, roznica, aadd_into, asub, iloczyn

extern malloc, memcpy, memset

;	struct bcd {
;		int n;  	// liczba cyfr
;		char *str; 	// cstring przechowujący cyfry pakowane po 2
;	}

parse:
	; === STACK ===
	; str		<--- ebp + 8
	; ret_addr	<--- ebp + 4
	; ebp 		<--- ebp
	; n  		<--- ebp - 4
	; *c		<--- ebp - 8
	; *ptr		<--- ebp - 12
	; *ret 		<--- ebp - 16
	; mul		<--- ebp - 20

	push ebp
	mov ebp, esp
	sub esp, 20
	push ebx
	push edi
	push esi

	;	while(*c != 0) {
	;		n++;
	;		c++;
	;	}
	xor eax, eax 		; n
	mov esi, [ebp + 8]	; c
	;	if(*str == '-') {
	;		mul = -1;
	;		str++;
	;	}
	mov dword [ebp - 20], 1
	cmp byte [esi], '-'
	jne W1
		inc esi
		mov dword [ebp + 8], esi
		mov dword [ebp - 20], -1
	jmp W1
L1:
	inc eax
	inc esi
W1:
	mov byte dl, [esi]
	cmp byte dl, 0
	jne L1
O1:
	mov [ebp - 4], eax

	;	ptr = (char*)malloc((n + 3) / 2 + 4);
	;	ret = ptr;
	;	*(int*)ptr = n;
	;	ptr += 4;
	add eax, 3
	shr eax, 1
	add eax, 4
	push eax
	call malloc
	mov [ebp - 12], eax
	mov [ebp - 16], eax
	mov edi, eax
	pop eax

	;	c = str + n;
	mov esi, [ebp + 8]
	mov edx, esi
	mov eax, [ebp - 4]
	add esi, eax
	mov ebx, [ebp - 20]
	imul dword ebx, eax
	mov dword [edi], ebx
	add edi, 4

	; esi := c
	; edi := ptr // ret + 4
	; edx := str
	; eax := n
	; Free Reg: eax, ebx, ecx

L2:
	dec esi
	; 	*ptr = *c - '0';
	mov byte al, [esi]		
	sub byte al, '0'		; al := *c - '0'

	;	if (c == str) {
	;		ptr++;
	;		break;
	;	}
	cmp esi, edx
	jne IF_F2
		mov byte [edi], al
		inc edi
		jmp O2
IF_F2:

	dec esi
	; 	*ptr += *c - '0';
	mov byte ah, [esi]	
	sub byte ah, '0'
	shl byte ah, 4
	add byte ah, al
	mov byte [edi], ah
	inc edi
W2:
	cmp esi, edx
	jne L2
O2:
	mov byte [edi], 0
	mov eax, [ebp - 16]

out:
	pop esi
	pop edi
	pop ebx
	add esp, 20
	pop ebp
	ret 

unparse:
	push ebp
	mov ebp, esp
	; str 		<--- ebp + 8
	; ret_addr	<--- ebp + 4
	; ebp		<--- ebp
	; n			<--- ebp - 4
	; c 		<--- ebp - 8
	; ptr 		<--- ebp - 12
	; ret 		<--- ebp - 16
	; str 		<--- ebp - 20

	sub esp, 20
	push ebx
	push esi
	push edi

	mov esi, [ebp + 8]	; esi := str
	mov ebx, [esi] 		; ebx := n
	test ebx, ebx
	jne L3
		; n == 0
		push dword 2
		call malloc
		pop ebx
		; eax := ptr
		; *ptr = '0'
		mov byte [eax], '0'
		mov byte [eax + 1], 0
		jmp exit
L3:
	js L4
		; n > 0
		mov [ebp - 4], ebx
		inc ebx
		push ebx
		call malloc
		pop ebx
		mov edi, eax
		mov [ebp - 16], edi
		jmp O3
L4:
		; n < 0
		xor ecx, ecx
		sub ecx, ebx
		; ecx := 0 - n
		mov [ebp - 4], ecx
		add ecx, 2
		push ecx
		call malloc
		pop ebx
		; ebx := n + 2
		; edi := ptr
		mov edi, eax
		mov [ebp - 16], edi
		mov byte [edi], '-'
		inc edi
		dec ebx
O3:
	; edi := ptr
	; ebx := n + 1
	mov esi, [ebp + 8]
	add esi, 4
	; esi := str + 4
	mov [ebp - 20], esi
	shr ebx, 1 		; ebx := (n + 1) / 2
	add esi, ebx	; esi := str + (n + 1) / 2
	mov ebx, [ebp - 4]

	mov edx, [ebp - 20]

	; edi := ptr
	; esi := c
	; ebx := n
	; edx := str
	test ebx, 0x1
	je L5
		; n & 1
		dec esi
		; *ptr = ((*c) & 0xf) + '0';
		mov byte cl, [esi]
		and byte cl, 0xf
		add byte cl, '0'
		mov byte [edi], cl
		inc edi
L5:
	cmp dword ebx, 1
	jle O4
L6:
	; n > 1
	dec esi

	; *ptr = (((*c) >> 4) & 0xf) + '0';
	mov cl, [esi]
	shr cl, 4
	and cl, 0xf
	add cl, '0'
	mov [edi], cl
	inc edi

	; *ptr = ((*c) & 0xf) + '0';
	mov cl, [esi]
	and cl, 0xf
	add cl, '0'
	mov [edi], cl
	inc edi
W3:
	cmp esi, edx
	je O4
	jmp L6
O4:
	mov byte [edi], 0
	mov eax, [ebp - 16]
exit:
	pop edi
	pop esi
	pop ebx
	add esp, 20
	pop ebp
	ret

; najpierw tylko dla dodatnich
_suma:
	push ebp
	mov ebp, esp
	; s2		<--- ebp + 12
	; s1		<--- ebp + 8
	; ret_addr	<--- ebp + 4
	; ebp		<--- ebp
	; n1		<--- ebp - 4
	; n2		<--- ebp - 8
	; ret 		<--- ebp - 12
	; ptr		<--- ebp - 16
	; m1		<--- ebp - 20
	; m2		<--- ebp - 24
	; s1		<--- ebp - 28
	; s2		<--- ebp - 32
	sub esp, 32
	push ebx
	push esi
	push edi

	mov edi, [ebp + 8]
	mov esi, [ebp + 12]
	mov eax, [edi]
	mov ebx, [esi]
	add edi, 4
	add esi, 4
	; edi := s1
	; esi := s2
	; eax := n1
	; ebx := n2
	; ecx := t/tc
	cmp eax, ebx
	jge L7
		; if(n1 < n2)
		xchg eax, ebx
		xchg esi, edi
L7:
	mov [ebp - 4], eax
	mov [ebp - 8], ebx
	mov [ebp - 28], edi
	mov [ebp - 32], esi
	add eax, 2
	shr eax, 1
	add eax, 5
	push eax
	call malloc
	add esp, 4 

	mov [ebp - 12], eax
	mov edi, eax
	add edi, 4
	mov esi, [ebp - 28]
	mov ebx, [ebp - 4]
	inc ebx
	shr ebx, 1
	mov [ebp - 20], ebx
	push ebx
	push esi
	push edi
	call memcpy
	pop edi
	add esp, 8
	
	mov ebx, [ebp - 8]
	inc ebx
	shr ebx, 1
	mov ecx, ebx
	mov esi, [ebp - 32]
	mov [ebp - 16], edi
	; ecx := i := m2
	; ebx := m2
	; edi := ptr
	; esi := s2
	; free: eax, edx
	clc
	pushf
	jmp W4
L8:
	mov ah, [esi]
	mov al, [edi]
	popf
	adc al, ah
	daa
	pushf
	mov [edi], al
	dec ecx
	inc edi
	inc esi
W4:
	test ecx, ecx
	jne L8

	mov ecx, ebx
	mov ebx, [ebp - 20]
	; ecx := i := m2
	; ebx := m1

	jmp W5
L9:
	mov al, [edi]
	popf
	adc al, 0
	daa
	pushf
	mov [edi], al
	inc edi
	inc ecx
W5:
	cmp ecx, ebx
	jl L9

	mov ebx, [ebp - 4]

	popf
	jnc L10
		stc
		mov al, [edi]
		adc al, 0
		daa
		mov [edi], al
		inc ebx
		inc edi
L10:

	mov byte [edi], 0
	mov eax, [ebp - 12]
	mov [eax], ebx

	pop edi
	pop esi
	pop ebx
	add esp, 32
	pop ebp
	ret

aless:
	push ebp
	mov ebp, esp
	; b
	; a
	; ret_addr
	; ebp
	push ebx
	push edi
	push esi

	; eax := n
	; ebx := n2
	; edi := a
	; esi := b
	; ecx := i
	; free: edx
	mov edi, [ebp + 8]
	mov esi, [ebp + 12]
	mov eax, [edi]
	mov ebx, [esi]
	cmp eax, 0
	jge L11
		imul eax, -1
L11:
	cmp ebx, 0
	jge L12
		imul ebx, -1
L12:
	cmp eax, ebx
	jg R0
	cmp ebx, eax
	jg R1
L13:
	add edi, 4
	add esi, 4
	inc eax
	shr eax, 1

	mov ecx, eax
	add edi, ecx
	add esi, ecx
	jmp W6
L14:
	mov dl, [edi]
	mov dh, [esi]
	cmp dl, dh
	je W6

	cmp dl, dh
	jae R0
	jmp R1
W6:
	dec ecx
	dec edi
	dec esi
	test ecx, ecx
	jns L14

R0:
	mov eax, 0
	jmp exit2
R1:
	mov eax, 1
exit2:
	pop esi
	pop edi
	pop ebx
	pop ebp
	ret

; najpierw tylko dla dodatnich
_roznica:
	push ebp
	mov ebp, esp
	; s2
	; s1
	; ret_addr
	; ebp		<--- ebp
	; mul		<--- -4
	; n1		<--- -8
	; ret 		<--- -16
	; m1		<--- -20
	sub esp, 20
	push ebx
	push edi
	push esi

	; edi := s1
	; esi := s2
	; ebx := mul
	mov ebx, 1
	mov edi, [ebp + 8]
	mov esi, [ebp + 12]
	push esi
	push edi
	call aless
	add esp, 8
	test eax, eax
	je L15
		; s1 < s2
		xchg edi, esi
		mov ebx, -1
L15:
	mov [ebp - 4], ebx
	add esi, 4

	; edi := s1
	; esi := s2
	; ebx := m1
	mov ebx, [edi]
	mov [ebp - 8], ebx
	inc ebx
	shr ebx, 1
	mov [ebp - 20], ebx
	add ebx, 5
	push ebx
	call malloc
	add esp, 4
	mov [ebp - 16], eax
		;	char *ptr = ret + 4;
		;	memcpy(ptr, s1, m1);
	sub ebx, 5
	push ebx
	add edi, 4
	push edi
	mov edi, eax
	add edi, 4
	push edi
	call memcpy
	add esp, 12
	; Actual state
	; eax := ret
	; edi := ptr
	; esi := s2
	; ebx := m1

	; New state
	; ebx := m2
	; ecx := i
	; edi := ptr
	; esi := s2
	; free: eax, edx
	mov ebx, [ebp + 12]
	mov ebx, [ebx]
	inc ebx
	shr ebx, 1
	xor ecx, ecx

	clc
	pushf
	jmp W7
L16:
	mov al, [esi]
	popf
	adc al, 0
	daa
	pushf
	test al, al
	je L17
	mov ah, al
	mov al, 0x99
	sub al, ah
	popf
	adc al, 1
	daa
	mov ah, [edi]
	xchg ah, al
	adc al, ah
	daa
	mov [edi], al
	cmc
	pushf
L17:
	inc ecx
	inc edi
	inc esi
W7:
	cmp ecx, ebx
	jl L16

	mov ebx, [ebp - 20]
	; ebx := m1
	; ecx := i
	; edi := ptr
	; free: eax, edx, esi
	jmp W8
L18:
	popf
	jnc O5
	clc
	mov ah, 0x99
	mov al, [edi]
	adc al, ah
	daa
	cmc
	pushf
	mov [edi], al
	inc edi
	inc ecx
W8:
	cmp ecx, ebx
	jl L18
O5:
	popf

	mov edi, [ebp - 16]
	add edi, 4
	mov ecx, ebx
W9:
	dec ecx
	js O6
	mov esi, edi
	add esi, ecx
	mov al, [esi]
	test al, al
	jne O6
	jmp W9
O6:

	; ecx := i
	; ebx := m1
	; edi := ptr
	; edx := n1
	; free: eax, esi

	cmp ecx, 0
	jns L19
		; i < 0
		mov edx, 0
		jmp O7
L19:
	; *(ptr + i)  != 0
	mov esi, edi
	add esi, ecx
	mov al, [esi]
	cmp byte al, 0x9
	jle L20
	mov edx, ecx
	inc edx
	shl edx, 1
	jmp O7
L20:
	; else
	mov edx, ecx
	shl edx, 1
	inc edx
O7:
	; ebx := mul
	; edi := ptr
	; eax := ret
	; edx := n1
	add edi, ebx
	mov byte [edi], 0
	mov eax, [ebp - 16]
	mov ebx, [ebp - 4]
	imul edx, ebx
	mov [eax], edx

	pop esi
	pop edi
	pop ebx
	add esp, 20
	pop ebp
	ret

suma:
;	push ebp
;	mov ebp, esp
;	mov ecx, [ebp + 8]
;	mov edx, [ebp + 12]
;	push edx
;	push ecx
;	call _suma
;	add esp, 8
;	pop ebp
;	ret

	; s2
	; s1
	; ret_addr
	; ebp
	push ebp
	mov ebp, esp
	push ebx
	push edi
	push esi

	; edi := s1
	; esi := s2
	; ebx := n1
	; ecx := n2
	mov edi, [ebp + 8]
	mov esi, [ebp + 12]
	mov ebx, [edi]
	mov ecx, [esi]

	cmp ebx, 0
	jle L22
		cmp edx, 0
		jle L21
		; n1 >=0 && n2 >=0
		push esi
		push edi
		call _suma
		add esp, 8
		jmp exit3
	L21:
		; n1 >= 0 && n2 < 0
		mov ebx, ecx
		imul ecx, -1
		mov [esi], ecx
		push esi
		push edi
		call _roznica
		add esp, 8
		mov [esi], ebx
		jmp exit3
L22:
	mov edx, 0
	sub edx, ebx
	mov [edi], edx
	cmp ecx, 0
	jl L23
		; n1 < 0 && n2 >=0
		push esi
		push edi
		call _roznica
		add esp, 8
		mov edx, [eax]
		imul edx, -1
		mov [eax], edx
		jmp L24
L23:
	; n1 < 0 && n2 < 0
	push ecx
	imul ecx, -1
	mov [esi], ecx
	push esi
	push edi
	call _suma
	add esp, 8
	pop ecx
	mov [esi], ecx
	mov edx, [eax]
	imul edx, -1
	mov [eax], edx
L24:
	mov [edi], ebx
exit3:
	pop esi
	pop edi
	pop ebx
	pop ebp
	ret

roznica:
	push ebp
	mov ebp, esp
	; s2
	; s1
	; ret_addr
	; ebp
	push edi
	push esi
	push ebx

	; edi := s1
	; esi := s2
	; ebx := n1
	; edx := n2
	mov edi, [ebp + 8]
	mov esi, [ebp + 12]
	mov ebx, [edi]
	mov edx, [esi]

	cmp dword ebx, 0
	jl L26
		cmp dword edx, 0
		jl L25
			; n1 >= 0 && n2 >=0 
			push esi
			push edi
			call _roznica
			add esp, 8
			jmp exit4
L25:
			; n1 >= 0 && n2 < 0
			push edx
			imul edx, -1
			mov [esi], edx
			push esi
			push edi
			call _suma
			add esp, 8
			pop edx
			mov [esi], edx
			jmp exit4
L26:
	push ebx
	imul ebx, -1
	mov [edi], ebx
	cmp dword edx, 0
	jl L27
		; n1 < && n2 >= 0
		push esi
		push edi
		call _suma
		add esp, 8
		mov ecx, [eax]
		imul ecx, -1
		mov [eax], ecx
		jmp L28
L27:
		; n1 < 0 && n2 < 0
		push edx
		imul edx, -1
		mov [esi], edx
		push esi
		push edi
		call _roznica
		add esp, 8
		pop edx
		mov [esi], edx
		mov ecx, [eax]
		imul ecx, -1
		mov [eax], ecx

L28:
	pop ebx
	mov [edi], ebx

exit4:
	pop ebx
	pop esi
	pop edi
	pop ebp
	ret

aadd_into:
	; n
	; src 
	; dest 
	; ret_addr
	; ebp
	push ebp
	mov ebp, esp
	push edi
	push esi
	push ebx

	mov edi, [ebp + 8]
	mov esi, [ebp + 12]
	mov ebx, [ebp + 16]
	; edi := dest
	; esi := src
	; ebx := n
	; ecx := i
	; free: eax, edx

	clc
	pushf
	xor ecx, ecx
	jmp W10
L29:
	mov al, [edi]
	mov ah, [esi]
	popf
	adc al, ah
	daa 
	pushf
	mov [edi], al
	inc edi
	inc esi
	inc ecx
W10:
	cmp ecx, ebx
	jl L29
	popf

	pop ebx
	pop esi
	pop edi
	pop ebp
	ret

asub:
	; n
	; src 
	; dest 
	; ret_addr
	; ebp
	push ebp
	mov ebp, esp
	push edi
	push esi
	push ebx

	mov edi, [ebp + 8]
	mov esi, [ebp + 12]
	mov ebx, [ebp + 16]
	; edi := dest
	; esi := src
	; ebx := n
	; ecx := i
	; free: eax, edx

	clc
	pushf
	xor ecx, ecx
	jmp W11
L30:
	mov al, [edi]
	mov ah, [esi]
	popf
	sbb al, ah
	das 
	pushf
	mov [edi], al
	inc edi
	inc esi
	inc ecx
W11:
	cmp ecx, ebx
	jl L30
	popf

	pop ebx
	pop esi
	pop edi
	pop ebp
	ret

iloczyn:
	; s2
	; s1
	; ret_addr
	; ebp
	; mul 	<--- [ebp - 4]
	; n1	-8
	; n2	-12
	; ret 	-16
	; ptr   -20
	; acc	-24
	; s1	-28
	; s2	-32
	; -- zmienne --
	push ebp
	mov ebp, esp
	sub esp, 32
	push ebx
	push edi
	push esi

	; edi := s1
	; esi := s2
	; ebx := n1
	; ecx := n2
	; eax := mul
	mov edi, [ebp + 8]
	mov esi, [ebp + 12]
	mov ebx, [edi]
	mov ecx, [esi] 
	mov eax, 1

	test ebx, ebx
	jns L31
		; n1 < 0
		imul eax, -1
		imul ebx, -1
		mov [ebp - 8], ebx
L31:
	test ecx, ecx
	jns L32
		; n2 < 0
		imul eax, -1
		imul ecx, -1
L32:
	mov [ebp - 4], eax

	; free: eax, edx

	mov [ebp - 12], ecx
	push esi
	push edi
	call aless
	add esp, 8
	mov ecx, [ebp - 12]
	test eax, eax
	jz L33
		xchg edi, esi
		xchg ecx, ebx
L33:

	add edi, 4
	add esi, 4
	cmp ecx, 1
	jne L35
		; n2 == 1
		mov eax, [esi]
		test eax, eax
		jnz L35
			;  *(s2) == 0
			push zero_text
			call parse
			add esp, 4
			jmp exit5
L35:
	
	mov edx, 5
	add edx, ebx
	add edx, ecx
	push ecx
	push edx
	push edx
	call malloc
	add esp, 4
	pop edx
	pop ecx
	mov [ebp - 16], eax
	push edx
	push dword 0
	push eax
	call memset
	add esp, 12
	mov edx, [ebp - 16]
	mov ecx, [ebp - 12]
	; edi := s1
	; esi := s2
	; edx := ret
	; ebx := n1
	; ecx := n2
	; free: eax

	; *(int*)ret = n1 + n2;
	mov eax, ebx
	add eax, ecx
	mov [edx], eax
	; char *ptr = ret + 4;
	add edx, 4
	inc ebx
	shr ebx, 1
	inc ecx
	shr ecx, 1
	; edi := s1
	; esi := s2
	; edx := ptr
	; ebx := n1
	; ecx := n2
	; free: eax
D2:
	mov edx, [ebp - 20]
	mov ecx, [ebp - 12]
	push dword 72
	call malloc
	add esp, 4
	mov [ebp - 24], eax ; zapisuje acc
	; edi := s1
	; esi := s2
	; ebx := n1
	; eax := acc
	mov ecx, ebx
	add ecx, 2
	push ecx
	call malloc
	add esp, 4
	mov esi, [ebp - 24]
	mov [esi], eax

	mov [ebp - 28], edi
	mov [esp - 32], esi
	; edi := s1
	; esi := acc
	; ebx := n1
	push ebx
	push edi
	push dword [esi]
	call memcpy
	add esp, 12
	mov edx, [esi]
	add edx, ebx
	mov word [edx], 0
	add ebx, 2

	mov edi, esi
	add esi, 4
	mov ecx, 1
	; edi := acc[i - 1]
	; esi := acc[i]
	; ecx := i
	; ebx := n1
	; edx := s1
	jmp W12
L36:
	; acc[i] = malloc(n1);
	push ecx
	push ebx
	call malloc
	add esp, 4
	mov [esi], eax
	; memcpy(acc[i], acc[i - 1], n1);
	push ebx
	push dword [edi]
	push dword [esi]
	call memcpy
	add esp, 12
	; aadd_into(acc[i], s1, n1);
	push ebx
	push dword [ebp - 28]
	push dword [esi]
	call aadd_into
	add esp, 12
	pop ecx
	inc ecx
	add edi, 4
	add esi, 4
W12:
	cmp ecx, 10
	jl L36

D1:
	xchg ebx, ebx


exit5:
	pop esi
	pop edi
	pop ebx
	add esp, 32
	pop ebp
	ret

section .data
zero_text db "0", 0
