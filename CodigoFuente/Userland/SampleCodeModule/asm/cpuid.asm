section .text

global getCpuId1
global getCpuId2
global getCpuId3

getCpuId1:
	push rbp
	mov rbp, rsp
	push rbx
	push rcx
	push rdx
	
	mov rax, 0
	cpuid ;deja un string de 12 bytes en rbx, rdx y rcx, en ese orden
	mov [string], rbx
	mov [string + 4], rdx
	mov [string + 8], rcx
 
    mov rax, string

	pop rdx
	pop rcx
	pop rbx
	mov rsp, rbp
	pop rbp
	ret


getCpuId2:
    push rbp
    mov rbp, rsp
    push rbx
    push rcx
    push rdx

    mov eax, 80000002h
    cpuid
    mov [string], eax
    mov [string+4], ebx
    mov [string+8], ecx
    mov [string+12], edx
    
    mov eax, 80000003h
    cpuid
    mov [string+16], eax
    mov [string+20], ebx
    mov [string+24], ecx
    mov [string+28], edx
    
    mov eax, 80000004h
    cpuid
    mov [string+32], eax
    mov [string+36], ebx
    mov [string+40], ecx
    mov [string+44], edx
    
    mov rax, string

    pop rdx
    pop rcx
    pop rbx
    mov rsp, rbp
    pop rbp
    ret

getCpuId3:
    push rbp
	mov rbp, rsp
	
	mov eax, 1
	cpuid

	mov rsp, rbp
	pop rbp
	ret
    

section .bss
string resb 128

