;Funcion para testear que funcione bien printmem

section .text

global writeMem

writeMem:
	;push rbp
	;mov rbp, rsp
	
    
	cpuid ;deja un string de 12 bytes en rbx, rdx y rcx, en ese orden
	mov byte word [1000], 'a'
	mov [1000 + 4], rdx
	mov [1000 + 8], rcx

    mov byte [2000], 10
    mov byte [2001], 20

    

	;mov rsp, rbp
	;pop rbp
	ret

section .bss
string resb 32
