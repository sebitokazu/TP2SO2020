GLOBAL getHours
GLOBAL getMinutes
GLOBAL getSeconds

getHours:
	push rbp
	mov rbp,rsp
	
	mov rax,0
	mov al, 4
	out 70h, al
	in al, 71h

	mov rsp, rbp
	pop rbp
	ret

getMinutes:
	push rbp
	mov rbp,rsp

	mov rax,0
	mov al, 2
	out 70h, al
	in al, 71h

	mov rsp, rbp
	pop rbp
	ret

getSeconds:
	push rbp
	mov rbp,rsp

	mov rax,0
	mov al, 0
	out 70h, al
	in al, 71h

	mov rsp, rbp
	pop rbp
	ret