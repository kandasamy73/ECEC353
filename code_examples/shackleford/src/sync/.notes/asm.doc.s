#--- THREAD DEC -------------
	movl	$0, i
	jmp	.L2
.L3:
	movl	val, %eax
	subl	$1, %eax
	movl	%eax, val(%rip)
	addl	$1, i
.L2:
	cmpl	$9999, i
	jle	.L3
#----------------------------


#--- MAIN INC ---------------
.L5:
    0 --> i
	jmp	.L6
.L7:
    val --> %eax
    %eax = %eax + 1
    %eax --> val
    i = i + 1
.L6:
	cmpl	$9999, i
	jle	.L7
#----------------------------
