
GLOBAL _cli
GLOBAL _sti
GLOBAL picMasterMask
GLOBAL picSlaveMask
GLOBAL haltcpu
GLOBAL _hlt
GLOBAL getIP
GLOBAL _scheduler

GLOBAL _irq00Handler
GLOBAL _irq01Handler
GLOBAL _irq02Handler
GLOBAL _irq03Handler
GLOBAL _irq04Handler
GLOBAL _irq05Handler
GLOBAL _irq80Handler

GLOBAL _exception0Handler
GLOBAL _exception6Handler

EXTERN irqDispatcher
EXTERN exceptionDispatcher
EXTERN syscall_handler
EXTERN timer_tick
EXTERN schedule

SECTION .text

%macro pushState 0
	push rax ;rbp[14]
	push rbx ;rbp[13]
	push rcx ;rbp[12]
	push rdx ;rbp[11]
	push rbp ;rbp[10]
	push rdi ;rbp[9]
	push rsi ;rbp[8]
	push r8 ;rbp[7]
	push r9 ;rbp[6]
	push r10 ;rbp[5]
	push r11 ;rbp[4]
	push r12 ;rbp[3]
	push r13 ;rbp[2]
	push r14 ;rbp[1]
	push r15 ;rbp[0]
%endmacro

%macro popState 0
	pop r15
	pop r14
	pop r13
	pop r12
	pop r11
	pop r10
	pop r9
	pop r8
	pop rsi
	pop rdi
	pop rbp
	pop rdx
	pop rcx
	pop rbx
	pop rax
%endmacro

%macro pushStateNoRax 0
	push rbx ;rbp[13]
	push rcx ;rbp[12]
	push rdx ;rbp[11]
	push rbp ;rbp[10]
	push rdi ;rbp[9]
	push rsi ;rbp[8]
	push r8 ;rbp[7]
	push r9 ;rbp[6]
	push r10 ;rbp[5]
	push r11 ;rbp[4]
	push r12 ;rbp[3]
	push r13 ;rbp[2]
	push r14 ;rbp[1]
	push r15 ;rbp[0]
%endmacro

%macro popStateNoRax 0
	pop r15
	pop r14
	pop r13
	pop r12
	pop r11
	pop r10
	pop r9
	pop r8
	pop rsi
	pop rdi
	pop rbp
	pop rdx
	pop rcx
	pop rbx
%endmacro

%macro irqHandlerMaster 1
	pushState

	mov rdi, %1 ; pasaje de parametro
	call irqDispatcher

	; signal pic EOI (End of Interrupt)
	mov al, 20h
	out 20h, al

	popState
	iretq
%endmacro



%macro exceptionHandler 1
	pushState

	mov rdi, %1 ; pasaje de parametro
    mov rsi, rsp; pasaje de parametro stack pointer
	call exceptionDispatcher

	popState
	iretq
%endmacro

getIP:
    pop rax
    ret


_hlt:
    push rbp
    mov rbp, rsp
	sti
	hlt
    mov rsp,rbp
    pop rbp
	ret

_cli:
	cli
	ret


_sti:
	sti
	ret

picMasterMask:
	push rbp
    mov rbp, rsp
    mov ax, di
    out	21h,al
    pop rbp
    retn

picSlaveMask:
	push    rbp
    mov     rbp, rsp
    mov     ax, di  ; ax = mascara de 16 bits
    out	0A1h,al
    pop     rbp
    retn


_scheduler:
	;limpiar llamadas desde el handler master
	pop rax
	pop rax
	pop rax
	pop rax
	pop rax
	pop rax
	pop rax
	pop rax
	pop rax

	mov rdi,rsp
	call schedule
	mov rsp,rax

	; signal pic EOI (End of Interrupt)
	mov al, 20h
	out 20h, al

	popState
	iretq

;8254 Timer (Timer Tick)
_irq00Handler:
	irqHandlerMaster 0

;Keyboard
_irq01Handler:
	irqHandlerMaster 1

;Cascade pic never called
_irq02Handler:
	irqHandlerMaster 2

;Serial Port 2 and 4
_irq03Handler:
	irqHandlerMaster 3

;Serial Port 1 and 3
_irq04Handler:
	irqHandlerMaster 4

;USB
_irq05Handler:
	irqHandlerMaster 5

;Interrupt80 - Syscalls
_irq80Handler:
	pushStateNoRax
	
	call syscall_handler
	
	popStateNoRax
	iretq

;Zero Division Exception
_exception0Handler:
	exceptionHandler 0

;Invalid Opcode Exception
_exception6Handler:
	exceptionHandler 6

haltcpu:
	cli
	hlt
	ret



SECTION .bss
	aux resq 1
