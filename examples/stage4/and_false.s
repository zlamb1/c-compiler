.globl main
main:
    mov $1, %eax
    cmp $0, %eax
    jne _L0
    mov $0, %eax
    jmp _L1
_L0:
    mov $0, %eax
    cmp $0, %eax
    mov $0, %eax
    setne %al
_L1:
    ret
