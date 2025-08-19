.section .text
.global _die
.type _die, @function
_die:
	hlt
	jmp _die