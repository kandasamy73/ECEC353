
====== WORKER THREAD ============
.L3:
	movl	val(%rip), %eax    ; val -> eax
	subl	$1, %eax           ; eax--
	movl	%eax, val(%rip)    ; eax -> val
	addl	$1, -4(%rbp)       ; i++
	cmpl	$9999, -4(%rbp)    ; if (i < 10000)
	jle	.L3                    ;    goto .L3
=================================


======= MAIN  THREAD ============
.L6:
	movl	val(%rip), %eax   ; val -> eax
	addl	$1, %eax          ; eax++
	movl	%eax, val(%rip)   ; eax -> val
	addl	$1, -12(%rbp)     ; i++
	cmpl	$9999, -12(%rbp)  ; if (i < 10000)
	jle	.L6                   ;    goto .L6
=================================


+-----------------+
|  function parm  |
+-----------------+
|  function parm  |
+-----------------+
|  return addr    |
+-----------------+
|  frame pointer  |  <- ebp
+-----------------+
|  local vars     |  <- -4(%ebp)
+-----------------+
