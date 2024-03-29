/*
 * uMPS - A general purpose computer system simulator
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef REGDEF_H
#define REGDEF_H

/*
 * GPR (General Purpose Registers)
 */

#define v0 2
#define v1 3

#define a0 4
#define a1 5
#define a2 6
#define a3 7

#define t0 8
#define t1 9
#define t2 10
#define t3 11
#define t4 12
#define t5 13
#define t6 14
#define t7 15

#define s0 16
#define s1 17
#define s2 18
#define s3 19
#define s4 20
#define s5 21
#define s6 22
#define s7 23

#define t8 24
#define t9 25

#define k0 26  // for Kernel use
#define k1 27  // for Kernel use

#define gp 28
#define sp 29  //  Stack Pointer
#define fp 30
#define s8 30  /* s8 == fp */

#define ra 31

#endif /* !defined(REGDEF_H) */
