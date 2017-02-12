	.file	"ejercicio2.c"
	.section	.rodata
	.align 8
.LC0:
	.string	"Hola soy hang %d trace %d cont %d\n"
.LC1:
	.string	"error en el primer fork"
.LC2:
	.string	"error en el segundo fork"
	.align 8
.LC3:
	.string	"Soy el proceso que no espera  a nadie,\nHijo de %d\n"
	.align 8
.LC4:
	.string	"El proceso %d ha waiteado a su hijo %d, pido q espere a f2 %d\n"
	.align 8
.LC5:
	.string	"Voy a pedir q %d espere a %d, que deberoa ser el de 100\n"
.LC6:
	.string	"%s\n"
	.align 8
.LC7:
	.string	"Yo no tengo hijos, soy el hijo de  %d\n pero quiero esperar al de 100, %d (%d)\n"
	.align 8
.LC8:
	.string	"El proceso %d ha waiteado a su hijo %d\n"
.LC9:
	.string	"%d proceso %d padre %d\n"
	.text
	.globl	main
	.type	main, @function
main:
.LFB2:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$48, %rsp
	movq	%fs:40, %rax
	movq	%rax, -8(%rbp)
	xorl	%eax, %eax
	movl	$8, %ecx
	movl	$2, %edx
	movl	$1, %esi
	movl	$.LC0, %edi
	movl	$0, %eax
	call	printf
	call	fork
	movl	%eax, -28(%rbp)
	cmpl	$-1, -28(%rbp)
	jne	.L2
	movl	$.LC1, %edi
	call	puts
	movl	$1, %edi
	call	exit
.L2:
	call	fork
	movl	%eax, -24(%rbp)
	cmpl	$-1, -24(%rbp)
	jne	.L3
	movl	$.LC2, %edi
	call	puts
	movl	$1, %edi
	call	exit
.L3:
	cmpl	$0, -28(%rbp)
	jne	.L4
	cmpl	$0, -24(%rbp)
	jne	.L5
	call	getppid
	movl	%eax, %esi
	movl	$.LC3, %edi
	movl	$0, %eax
	call	printf
	movl	$0, -32(%rbp)
	jmp	.L8
.L5:
	leaq	-40(%rbp), %rcx
	movl	-24(%rbp), %eax
	movl	$0, %edx
	movq	%rcx, %rsi
	movl	%eax, %edi
	call	waitpid
	movl	%eax, -20(%rbp)
	cmpl	$-1, -20(%rbp)
	jne	.L7
	movl	$1, %edi
	call	exit
.L7:
	call	getpid
	movl	%eax, %esi
	movl	-24(%rbp), %edx
	movl	-20(%rbp), %eax
	movl	%edx, %ecx
	movl	%eax, %edx
	movl	$.LC4, %edi
	movl	$0, %eax
	call	printf
	movl	$100, -32(%rbp)
	jmp	.L8
.L4:
	cmpl	$0, -24(%rbp)
	jne	.L9
	call	getpid
	movl	%eax, %ecx
	movl	-28(%rbp), %eax
	movl	%eax, %edx
	movl	%ecx, %esi
	movl	$.LC5, %edi
	movl	$0, %eax
	call	printf
	leaq	-40(%rbp), %rcx
	movl	-28(%rbp), %eax
	movl	$0, %edx
	movq	%rcx, %rsi
	movl	%eax, %edi
	call	waitpid
	testl	%eax, %eax
	jns	.L10
	leaq	-40(%rbp), %rcx
	movl	-28(%rbp), %eax
	movl	$0, %edx
	movq	%rcx, %rsi
	movl	%eax, %edi
	call	explain_waitpid
	movq	%rax, %rdx
	movq	stderr(%rip), %rax
	movl	$.LC6, %esi
	movq	%rax, %rdi
	movl	$0, %eax
	call	fprintf
	movl	$1, %edi
	call	exit
.L10:
	call	getpid
	movl	%eax, %esi
	movl	-20(%rbp), %edx
	movl	-28(%rbp), %eax
	movl	%edx, %ecx
	movl	%eax, %edx
	movl	$.LC7, %edi
	movl	$0, %eax
	call	printf
	movl	$1000, -32(%rbp)
	jmp	.L8
.L9:
	leaq	-40(%rbp), %rcx
	movl	-24(%rbp), %eax
	movl	$0, %edx
	movq	%rcx, %rsi
	movl	%eax, %edi
	call	waitpid
	movl	%eax, -20(%rbp)
	cmpl	$-1, -20(%rbp)
	jne	.L11
	movl	$1, %edi
	call	exit
.L11:
	call	getpid
	movl	%eax, %ecx
	movl	-20(%rbp), %eax
	movl	%eax, %edx
	movl	%ecx, %esi
	movl	$.LC8, %edi
	movl	$0, %eax
	call	printf
	movl	$10000, -32(%rbp)
.L8:
	call	getpid
	movl	%eax, -16(%rbp)
	call	getppid
	movl	%eax, -12(%rbp)
	movl	-32(%rbp), %eax
	movl	%eax, -36(%rbp)
	jmp	.L12
.L13:
	movl	-12(%rbp), %ecx
	movl	-16(%rbp), %edx
	movl	-36(%rbp), %eax
	movl	%eax, %esi
	movl	$.LC9, %edi
	movl	$0, %eax
	call	printf
	addl	$1, -36(%rbp)
.L12:
	movl	-36(%rbp), %eax
	subl	-32(%rbp), %eax
	cmpl	$9, %eax
	jle	.L13
	movl	$0, %edi
	call	exit
	.cfi_endproc
.LFE2:
	.size	main, .-main
	.ident	"GCC: (Ubuntu 5.4.0-6ubuntu1~16.04.4) 5.4.0 20160609"
	.section	.note.GNU-stack,"",@progbits
