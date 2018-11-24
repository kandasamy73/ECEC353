#--------- WORKER THREAD -----------
	movl	0 --> i
	jmp	.L2
.L3:
	movl	val --> eax
	subl	1, %eax           (eax--)
	movl	eax --> val
	addl	1, -4(%rbp)       (i++)
.L2:
	cmpl	$999999, i
	jle	.L3

#--------- MAIN THREAD -----------
.L5:
	movl	0 --> i
	jmp	.L6
.L7:
	movl	val --> eax
	addl	$1, %eax         (eax++)
	movl	eax --> val
	addl	$1, -24(%rbp)    (i++)
.L6:
	cmpl	$999999, i
	jle	.L7
