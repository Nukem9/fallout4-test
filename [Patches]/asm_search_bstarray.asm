; //////////////////////////////////////////
; /*
; * Copyright (c) 2022 Perchik71 <email:perchik71@outlook.com>
; *
; * Permission is hereby granted, free of charge, to any person obtaining a copy of this
; * software and associated documentation files (the "Software"), to deal in the Software
; * without restriction, including without limitation the rights to use, copy, modify, merge,
; * publish, distribute, sublicense, and/or sell copies of the Software, and to permit
; * persons to whom the Software is furnished to do so, subject to the following conditions:
; *
; * The above copyright notice and this permission notice shall be included in all copies or
; * substantial portions of the Software.
; *
; * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
; * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
; * PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE
; * FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
; * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
; * DEALINGS IN THE SOFTWARE.
; */
; //////////////////////////////////////////

.code 
	ALIGN 16
	PUBLIC __CKF4FIXES_search_64bit
	__CKF4FIXES_search_64bit PROC

	push rbp
	sub rsp, 40h
	mov r9d, [rcx + 10h]
	cmp r9d, r8d

	jbe short label_quit1

	mov r11, [rcx]
	mov ecx, r9d
	mov eax, r8d
	sub ecx, r8d
	lea r10, [rax * 8h]
	add r11, r10
	cmp ecx, 32h

	jae short label_begin1

	mov rcx, [rdx]

label_test1:
	mov eax, r8d
	cmp [r11 + rax * 8h], rcx

	je short label_quit2

	inc r8d
	cmp r8d, r9d

	jb short label_test1

label_quit1:
	or eax, -1
	add rsp, 40h
	pop rbp
	ret 

label_quit2:
	mov eax, r8d
	add rsp, 40h
	pop rbp
	ret 

label_begin1:
	mov [rsp + 50h],rbx
	xor eax, eax
	mov [rsp + 58h],rsi
	add r10, r11
	mov rsi, [rdx]
	mov ebp, 0FFFFFFFFh
	mov [rsp + 60h], rdi
	movaps [rsp + 30h], xmm6
	movaps [rsp + 20h], xmm7
	mov edi, ecx
	movaps [rsp + 10h], xmm8
	movaps [rsp], xmm9
	movq xmm9, rsi
	shr rdi, 4h
	punpcklqdq xmm9, xmm9
	test rdi, rdi

	je label_res1

label_calc:
	movdqa xmm0, [r10]
	movdqa xmm8, xmm9
	movdqa xmm7, xmm9
	pcmpeqq xmm8, xmm0
	movdqa xmm0, [r10 + 10h]
	movdqa xmm6, xmm9
	movdqa xmm5, xmm9
	pcmpeqq xmm7, xmm0
	movdqa xmm0, [r10 + 20h]
	movdqa xmm4, xmm9
	movdqa xmm3, xmm9
	pcmpeqq xmm6, xmm0
	movdqa xmm0, [r10 + 30h]
	movdqa xmm2, xmm9
	movdqa xmm1, xmm9
	pcmpeqq xmm5, xmm0
	movdqa xmm0, [r10 + 40h]
	pcmpeqq xmm4, xmm0
	movdqa xmm0, [r10 + 50h]
	pcmpeqq xmm3, xmm0
	movdqa xmm0, [r10 + 60h]
	pcmpeqq xmm2, xmm0
	movdqa xmm0, [r10 + 70h]
	pcmpeqq xmm1, xmm0
	movmskpd ecx, xmm1
	movsxd rbx, ecx
	shl rbx, 2h
	movmskpd ecx, xmm2
	movsxd rdx, ecx
	or rbx, rdx
	movmskpd ecx, xmm3
	shl rbx, 2h
	movsxd rdx, ecx
	or rbx, rdx
	movmskpd ecx, xmm4
	shl rbx, 2h
	movsxd rdx, ecx
	or rbx, rdx
	movmskpd ecx, xmm5
	shl rbx, 2h
	movsxd rdx, ecx
	or rbx, rdx
	movmskpd ecx, xmm6
	shl rbx, 2h
	movsxd rdx, ecx
	or rbx, rdx
	movmskpd ecx, xmm7
	shl rbx, 2h
	movsxd rdx, ecx
	or rbx, rdx
	movmskpd ecx, xmm8
	shl rbx, 2h
	movsxd rdx, ecx
	or rbx, rdx

	jne short label_res2

	sub r10, 0FFFFFFFFFFFFFF80h
	inc rax
	cmp rax, rdi

	jb label_calc

label_res1:
	shl eax, 4h
	add eax, r8d
	cmp eax, r9d

	jae short label_q1

label_test2:
	cmp [r11 + rax * 8h], rsi

	je short label_q2

	inc eax
	cmp eax, r9d

	jb short label_test2

	mov eax, ebp

	jmp short label_res3

label_res2:
	bsf rcx, rbx
	shl eax, 4h
	add eax, ecx
	add eax, r8d
label_res3:
	movaps xmm8, [rsp + 10h]
	movaps xmm7, [rsp + 20h]
	movaps xmm6, [rsp + 30h]
	mov rdi, [rsp + 60h]
	mov rsi, [rsp + 58h]
	mov rbx, [rsp + 50h]
	movaps xmm9, [rsp]
	add rsp,40h
	pop rbp
	ret 

label_q2:
	mov ebp,eax

label_q1:
	mov eax,ebp

	jmp short label_res3
	__CKF4FIXES_search_64bit ENDP

end