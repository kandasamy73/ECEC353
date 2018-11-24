thread:
========================================
	movl	$0, -4(%rbp)        # int i = 0
	jmp	.CHECK
.LOOP:
	movl	val(%rip), %eax     # val => eax
	subl	$1, %eax            # eax--
	movl	%eax, val(%rip)     # eax => val
	addl	$1, -4(%rbp)        # i++
.CHECK:
	cmpl	$999999, -4(%rbp)   # i < 1000000 ? .LOOP : .DONE
	jle	.LOOP
========================================

main:
========================================
	movl	$0, -16(%rbp)       # int i = 0
	jmp	.CHECK
.LOOP:
	movl	val(%rip), %eax     # val => eax
	addl	$1, %eax            # eax++
	movl	%eax, val(%rip)     # eax => val
	addl	$1, -16(%rbp)       # i++
.CHECK:
	cmpl	$999999, -16(%rbp)  # i < 1000000 ? .LOOP : .DONE
	jle	.LOOP
========================================
