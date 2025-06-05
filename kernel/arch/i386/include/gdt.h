 #ifndef GDT_H
 #define GDT_H 1

 #include <stdint.h>
 
struct GDT {
    uint16_t limitLow;
    uint16_t baseLow;
    uint8_t baseMid;
    uint8_t access;
    unsigned int limitHigh : 4;
    unsigned int flags : 4;
    uint8_t baseHigh;
}  __attribute__((packed));

struct GDTR {
    uint16_t size;
    uint32_t offset;
} __attribute__((packed));

int encodeGdtEntry(struct GDT* gdt, int base, int limit, uint8_t access, uint8_t flags);
void gdtInit(struct GDT* gdt, int n);

#endif