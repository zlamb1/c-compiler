.globl main
main:
    mov $1, %eax
    cmp $0, %eax
    je _L0
    mov $1, %eax
    jmp _L1
_L0:
    mov $0, %eax
    cmp $0, %eax
    jne _L2
    jmp _L3
_L2:
    mov $2, %eax
    cmp $0, %eax
    mov $0, %eax
    setne %al
_L3:
    cmp $0, %eax
    mov $0, %eax
    setne %al
_L1:
    ret
