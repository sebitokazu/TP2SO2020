section .text

global getCpuThermStatus
global getCpuTempTarget

getCpuThermStatus:
	push rbp
	mov rbp, rsp
	
	;mov ecx, 0x19c
	;rdmsr

	mov eax, 0x883a0008

	mov rsp, rbp
	pop rbp
	ret

getCpuTempTarget:
	push rbp
	mov rbp, rsp
	
	;mov ecx, 0x1a2
	;rdmsr

	mov eax, 0x641400

	mov rsp, rbp
	pop rbp
	ret