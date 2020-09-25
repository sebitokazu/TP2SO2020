GLOBAL cpuVendor
GLOBAL getKeyboardScancode
GLOBAL checkIfFindCharacter


section .text
 
cpuVendor:
	push rbp
	mov rbp, rsp

	push rbx

	mov rax, 0
	cpuid


	mov [rdi], ebx
	mov [rdi + 4], edx
	mov [rdi + 8], ecx

	mov byte [rdi+13], 0

	mov rax, rdi

	pop rbx

	mov rsp, rbp
	pop rbp
	ret


getKeyboardScancode:
    push rbp
    mov rbp, rsp
    
    in al,0x60
       
    mov rsp, rbp
    pop rbp
    ret
    
checkIfFindCharacter:
    push rbp
    mov rbp, rsp
    
    in al,0x64
    mov cl,al
    and al,0x01
    cmp al,0
    
    mov al, 1
    mov rsp, rbp
    pop rbp
    ret
    


