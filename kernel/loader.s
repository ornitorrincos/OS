global loader
extern kmain

section .text
align 4
STACKSIZE equ 0x4000


loader:
    ;setup the stack and call the kernel entry point
    ; shouldn't really use it this way, but take the loading address and move that to rsp to get a working stack
    mov rsp, stack+STACKSIZE
    mov rbp, rsp
    ; pass arguments we receiver, or maybe have to get them first fropm the stack

    call kmain
    hlt 


section .bss
align 32
stack:
    resb STACKSIZE
