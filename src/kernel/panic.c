__attribute__((noreturn))
void hcf() {
    asm("cli");
    for(;;) {
        asm("hlt");
    }
    __builtin_unreachable();
}