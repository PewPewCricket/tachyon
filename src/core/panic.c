_Noreturn void panic() {
	asm volatile ("1: cli; hlt; jmp 1b");
	while (1);
}