section .text

global getRegisters

getRegisters:
	;push rbp
	;mov rbp, rsp


    mov [vec], rax
    mov [vec+8], rcx
    mov [vec+16], rdx
    mov [vec+24], rbx
    mov [vec+32], rsp
    mov [vec+40], rbp
    mov [vec+48], rsi
    mov [vec+56], rdi
    mov [vec+64], r8
    mov [vec+72], r9
    mov [vec+80], r10
    mov [vec+88], r11
    mov [vec+96], r12
    mov [vec+104], r13
    mov [vec+112], r14
    mov [vec+120], r15
	

	mov rax, vec
	;mov rsp, rbp
	;pop rbp
	ret

section .bss
vec resb 8*18