.section .data
t3: .word 0
t2: .word 0
t1: .word 0
x: .word 0

.section .rodata
fmt_out: .asciz "x = %d\\n"
end_msg: .asciz "terminado\\n"

.section .text
.globl my_logic
my_logic:
    la s3, x
    la s2, t1
    la s1, t2
    la s0, t3

    li t0, 1
    sw t0, 0(s3)

l1:
    lw t0, 0(s3)
    li t1, 5
    slt t2, t0, t1
    sw t2, 0(s2)

    lw t0, 0(s2)
    beqz t0, l2

    lw t0, 0(s3)
    li t1, 3
    xor t2, t0, t1
    seqz t2, t2
    sw t2, 0(s1)

    lw t0, 0(s1)
    beqz t0, l3

    lw a1, 0(s3)
    la a0, fmt_out
    call printf

    j l4

l3:
l4:
    lw t0, 0(s3)
    li t1, 1
    add t2, t0, t1
    sw t2, 0(s0)

    lw t0, 0(s0)
    sw t0, 0(s3)

    j l1

l2:
    la a0, end_msg
    call printf

    ret
