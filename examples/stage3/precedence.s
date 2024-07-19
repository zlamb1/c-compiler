.globl main
main:
    mov $2, %eax
    push %rax
    mov $3, %eax
    push %rax
    mov $4, %eax
    pop %rcx
    imul %ecx, %eax
    pop %rcx
    add %ecx, %eax
    ret
