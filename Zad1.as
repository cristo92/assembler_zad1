section .text
global parse, unparse

extern malloc

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
		push dword 1
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



