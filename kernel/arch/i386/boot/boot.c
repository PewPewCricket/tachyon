/* Tachyon Kernel
 * Copyright (C) 2025 PewPewCricket
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <gdt.h>

void boot() {
    // TODO: parse mb2 data

    // Set up GDT
    struct GDT gdts[5];
    encodeGdtEntry(&gdts[0], 0, 0, 0, 0);               // Null Descriptor
    encodeGdtEntry(&gdts[1], 0, 0xFFFFF, 0x9A, 0xC);    // Kernel Code
    encodeGdtEntry(&gdts[2], 0, 0xFFFFF, 0x92, 0xC);    // Kernel Data
    encodeGdtEntry(&gdts[2], 0, 0xFFFFF, 0xFA, 0xC);    // User Code
    encodeGdtEntry(&gdts[2], 0, 0xFFFFF, 0xF2, 0xC);    // User Data

    gdtInit(gdts, 5);
    
    while(1);
}