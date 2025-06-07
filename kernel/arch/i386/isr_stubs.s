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

.section .text

.macro ISR_ERR_STUB n
isr_stub_\n:
    push \n
    call exception_handler
    iret
.endm

.rept 32
ISR_ERR_STUB \+
.endr

.global isr_stub_table
isr_stub_table:
.rept 32
.long isr_stub_\+
.endr