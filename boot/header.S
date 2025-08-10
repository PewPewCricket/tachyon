.section .multiboot
.align 8
mb2h:
	.long 0xE85250D6
	.long 0
	.long mb2h_end - mb2h
	.long 0x100000000 - (0xE85250D6 + 0 + (mb2h_end - mb2h))

.align 8
mb2h_tag_fb:
	.word 5
	.word 0
	.long mb2h_tag_end - mb2h_tag_fb
	.long 0		# Framebuffer width
	.long 0		# Framebuffer height
	.long 32	# Framebuffer depth

.align 8
mb2h_tag_end:
	.word 0
	.word 0
	.long 8
mb2h_end:

.section .text
.extern boot
.global _start
.type _start, @function
_start:
	cli
	mov $stack_top, %ebp
	mov $stack_top, %esp
	pushl %ebx
	call boot
	hlt

.section .bss
.align 16
stack_bottom:
	.skip 16384			# 16KB of stack space
stack_top: