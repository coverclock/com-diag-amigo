/**
 * @file
 * Copyright 2012 Digital Aggregates Corporation, Colorado, USA\n
 * Licensed under the terms in README.h\n
 * Chip Overclock mailto:coverclock@diag.com\n
 * http://www.diag.com/navigation/downloads/Amigo.html\n
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include "com/diag/amigo/fatal.h"

// Originally I had just the one default interrupt service routine. But for
// the 328p port it became useful to know through which vector I was getting an
// unexpected interrupt. So I came up with the second mechanism that tells
// me that. The down side is it generates a function for every vector that
// isn't otherwise used by one of the device drivers.

#if 0

// Placing this in the low text section seems to be occasionally necessary.
// Once in a while I'll get a link error about the linker not being able
// to generate a jump to the default __bad_interrupt function because it is
// too far away.
ISR(_VECTOR(default), __attribute__ ((section(".lowtext")))) {
	// I'd really like to print the vector we came through to get here, but I
	// don't see a way to do that. The PC on the stack is that of the code
	// that got the spurious interrupt. So unwinding the stack, while entirely
	// feasible, isn't really useful.
	com::diag::amigo::fatal(PSTR(__FILE__), __LINE__);
}

#else

// GCC calls the first vector after the reset vector at 0x0000 "vector 1".
// But the Atmel data sheet calls the reset vector "1", and the vector after
// that "2". So we generate code using the GCC convention, but print a message
// using the Atmel convention because someone debugging an unexpected interrupt
// will probably be looking at the data sheet to identify it. Since the _N_
// argument is a constant, the compiler will optimize the (_N_ + 1) to a
// constant, so this convenience generates no additional code.

#define COM_DIAG_AMIGO_UNEXPECTED(_N_) \
	ISR(_VECTOR(_N_), __attribute__ ((section(".lowtext"), weak))) {  \
		com::diag::amigo::unexpected(_N_ + 1); \
	}

// There should be some way to package this into a header file that can be
// included repeatedly prefaced with the vector number defined as a preprocessor
// symbol using the technique sometimes called "magic macro" or "super macro".
// But I wasn't able to get it to work, even using all the usual double nested
// function macro tricks. So I wrote a little script to generate this code.
// Note that this only generates the ISRs that are really needed on the target,
// and since these are weak external references, they only get included by the
// linker if the application doesn't otherwise specify them. It's pretty much
// optimal, at least for C/C++.

#if (1 < (_VECTORS_SIZE / 4))
COM_DIAG_AMIGO_UNEXPECTED(1)
#endif

#if (2 < (_VECTORS_SIZE / 4))
COM_DIAG_AMIGO_UNEXPECTED(2)
#endif

#if (3 < (_VECTORS_SIZE / 4))
COM_DIAG_AMIGO_UNEXPECTED(3)
#endif

#if (4 < (_VECTORS_SIZE / 4))
COM_DIAG_AMIGO_UNEXPECTED(4)
#endif

#if (5 < (_VECTORS_SIZE / 4))
COM_DIAG_AMIGO_UNEXPECTED(5)
#endif

#if (6 < (_VECTORS_SIZE / 4))
COM_DIAG_AMIGO_UNEXPECTED(6)
#endif

#if (7 < (_VECTORS_SIZE / 4))
COM_DIAG_AMIGO_UNEXPECTED(7)
#endif

#if (8 < (_VECTORS_SIZE / 4))
COM_DIAG_AMIGO_UNEXPECTED(8)
#endif

#if (9 < (_VECTORS_SIZE / 4))
COM_DIAG_AMIGO_UNEXPECTED(9)
#endif

#if (10 < (_VECTORS_SIZE / 4))
COM_DIAG_AMIGO_UNEXPECTED(10)
#endif

#if (11 < (_VECTORS_SIZE / 4))
COM_DIAG_AMIGO_UNEXPECTED(11)
#endif

#if (12 < (_VECTORS_SIZE / 4))
COM_DIAG_AMIGO_UNEXPECTED(12)
#endif

#if (13 < (_VECTORS_SIZE / 4))
COM_DIAG_AMIGO_UNEXPECTED(13)
#endif

#if (14 < (_VECTORS_SIZE / 4))
COM_DIAG_AMIGO_UNEXPECTED(14)
#endif

#if (15 < (_VECTORS_SIZE / 4))
COM_DIAG_AMIGO_UNEXPECTED(15)
#endif

#if (16 < (_VECTORS_SIZE / 4))
COM_DIAG_AMIGO_UNEXPECTED(16)
#endif

#if (17 < (_VECTORS_SIZE / 4))
COM_DIAG_AMIGO_UNEXPECTED(17)
#endif

#if (18 < (_VECTORS_SIZE / 4))
COM_DIAG_AMIGO_UNEXPECTED(18)
#endif

#if (19 < (_VECTORS_SIZE / 4))
COM_DIAG_AMIGO_UNEXPECTED(19)
#endif

#if (20 < (_VECTORS_SIZE / 4))
COM_DIAG_AMIGO_UNEXPECTED(20)
#endif

#if (21 < (_VECTORS_SIZE / 4))
COM_DIAG_AMIGO_UNEXPECTED(21)
#endif

#if (22 < (_VECTORS_SIZE / 4))
COM_DIAG_AMIGO_UNEXPECTED(22)
#endif

#if (23 < (_VECTORS_SIZE / 4))
COM_DIAG_AMIGO_UNEXPECTED(23)
#endif

#if (24 < (_VECTORS_SIZE / 4))
COM_DIAG_AMIGO_UNEXPECTED(24)
#endif

#if (25 < (_VECTORS_SIZE / 4))
COM_DIAG_AMIGO_UNEXPECTED(25)
#endif

#if (26 < (_VECTORS_SIZE / 4))
COM_DIAG_AMIGO_UNEXPECTED(26)
#endif

#if (27 < (_VECTORS_SIZE / 4))
COM_DIAG_AMIGO_UNEXPECTED(27)
#endif

#if (28 < (_VECTORS_SIZE / 4))
COM_DIAG_AMIGO_UNEXPECTED(28)
#endif

#if (29 < (_VECTORS_SIZE / 4))
COM_DIAG_AMIGO_UNEXPECTED(29)
#endif

#if (30 < (_VECTORS_SIZE / 4))
COM_DIAG_AMIGO_UNEXPECTED(30)
#endif

#if (31 < (_VECTORS_SIZE / 4))
COM_DIAG_AMIGO_UNEXPECTED(31)
#endif

#if (32 < (_VECTORS_SIZE / 4))
COM_DIAG_AMIGO_UNEXPECTED(32)
#endif

#if (33 < (_VECTORS_SIZE / 4))
COM_DIAG_AMIGO_UNEXPECTED(33)
#endif

#if (34 < (_VECTORS_SIZE / 4))
COM_DIAG_AMIGO_UNEXPECTED(34)
#endif

#if (35 < (_VECTORS_SIZE / 4))
COM_DIAG_AMIGO_UNEXPECTED(35)
#endif

#if (36 < (_VECTORS_SIZE / 4))
COM_DIAG_AMIGO_UNEXPECTED(36)
#endif

#if (37 < (_VECTORS_SIZE / 4))
COM_DIAG_AMIGO_UNEXPECTED(37)
#endif

#if (38 < (_VECTORS_SIZE / 4))
COM_DIAG_AMIGO_UNEXPECTED(38)
#endif

#if (39 < (_VECTORS_SIZE / 4))
COM_DIAG_AMIGO_UNEXPECTED(39)
#endif

#if (40 < (_VECTORS_SIZE / 4))
COM_DIAG_AMIGO_UNEXPECTED(40)
#endif

#if (41 < (_VECTORS_SIZE / 4))
COM_DIAG_AMIGO_UNEXPECTED(41)
#endif

#if (42 < (_VECTORS_SIZE / 4))
COM_DIAG_AMIGO_UNEXPECTED(42)
#endif

#if (43 < (_VECTORS_SIZE / 4))
COM_DIAG_AMIGO_UNEXPECTED(43)
#endif

#if (44 < (_VECTORS_SIZE / 4))
COM_DIAG_AMIGO_UNEXPECTED(44)
#endif

#if (45 < (_VECTORS_SIZE / 4))
COM_DIAG_AMIGO_UNEXPECTED(45)
#endif

#if (46 < (_VECTORS_SIZE / 4))
COM_DIAG_AMIGO_UNEXPECTED(46)
#endif

#if (47 < (_VECTORS_SIZE / 4))
COM_DIAG_AMIGO_UNEXPECTED(47)
#endif

#if (48 < (_VECTORS_SIZE / 4))
COM_DIAG_AMIGO_UNEXPECTED(48)
#endif

#if (49 < (_VECTORS_SIZE / 4))
COM_DIAG_AMIGO_UNEXPECTED(49)
#endif

#if (50 < (_VECTORS_SIZE / 4))
COM_DIAG_AMIGO_UNEXPECTED(50)
#endif

#if (51 < (_VECTORS_SIZE / 4))
COM_DIAG_AMIGO_UNEXPECTED(51)
#endif

#if (52 < (_VECTORS_SIZE / 4))
COM_DIAG_AMIGO_UNEXPECTED(52)
#endif

#if (53 < (_VECTORS_SIZE / 4))
COM_DIAG_AMIGO_UNEXPECTED(53)
#endif

#if (54 < (_VECTORS_SIZE / 4))
COM_DIAG_AMIGO_UNEXPECTED(54)
#endif

#if (55 < (_VECTORS_SIZE / 4))
COM_DIAG_AMIGO_UNEXPECTED(55)
#endif

#if (56 < (_VECTORS_SIZE / 4))
COM_DIAG_AMIGO_UNEXPECTED(56)
#endif

#if (57 < (_VECTORS_SIZE / 4))
COM_DIAG_AMIGO_UNEXPECTED(57)
#endif

#if (58 < (_VECTORS_SIZE / 4))
COM_DIAG_AMIGO_UNEXPECTED(58)
#endif

#if (59 < (_VECTORS_SIZE / 4))
COM_DIAG_AMIGO_UNEXPECTED(59)
#endif

#if (60 < (_VECTORS_SIZE / 4))
COM_DIAG_AMIGO_UNEXPECTED(60)
#endif

#if (61 < (_VECTORS_SIZE / 4))
COM_DIAG_AMIGO_UNEXPECTED(61)
#endif

#if (62 < (_VECTORS_SIZE / 4))
COM_DIAG_AMIGO_UNEXPECTED(62)
#endif

#if (63 < (_VECTORS_SIZE / 4))
COM_DIAG_AMIGO_UNEXPECTED(63)
#endif

#if (64 < (_VECTORS_SIZE / 4))
COM_DIAG_AMIGO_UNEXPECTED(64)
#endif

#if (65 < (_VECTORS_SIZE / 4))
COM_DIAG_AMIGO_UNEXPECTED(65)
#endif

#if (66 < (_VECTORS_SIZE / 4))
COM_DIAG_AMIGO_UNEXPECTED(66)
#endif

#if (67 < (_VECTORS_SIZE / 4))
COM_DIAG_AMIGO_UNEXPECTED(67)
#endif

#if (68 < (_VECTORS_SIZE / 4))
COM_DIAG_AMIGO_UNEXPECTED(68)
#endif

#if (69 < (_VECTORS_SIZE / 4))
COM_DIAG_AMIGO_UNEXPECTED(69)
#endif

#if (70 < (_VECTORS_SIZE / 4))
COM_DIAG_AMIGO_UNEXPECTED(70)
#endif

#if (71 < (_VECTORS_SIZE / 4))
COM_DIAG_AMIGO_UNEXPECTED(71)
#endif

#if (72 < (_VECTORS_SIZE / 4))
COM_DIAG_AMIGO_UNEXPECTED(72)
#endif

#if (73 < (_VECTORS_SIZE / 4))
COM_DIAG_AMIGO_UNEXPECTED(73)
#endif

#if (74 < (_VECTORS_SIZE / 4))
COM_DIAG_AMIGO_UNEXPECTED(74)
#endif

#if (75 < (_VECTORS_SIZE / 4))
COM_DIAG_AMIGO_UNEXPECTED(75)
#endif

#if (76 < (_VECTORS_SIZE / 4))
COM_DIAG_AMIGO_UNEXPECTED(76)
#endif

#if (77 < (_VECTORS_SIZE / 4))
COM_DIAG_AMIGO_UNEXPECTED(77)
#endif

#if (78 < (_VECTORS_SIZE / 4))
COM_DIAG_AMIGO_UNEXPECTED(78)
#endif

#if (79 < (_VECTORS_SIZE / 4))
COM_DIAG_AMIGO_UNEXPECTED(79)
#endif

#if (80 < (_VECTORS_SIZE / 4))
COM_DIAG_AMIGO_UNEXPECTED(80)
#endif

#if (81 < (_VECTORS_SIZE / 4))
COM_DIAG_AMIGO_UNEXPECTED(81)
#endif

#if (82 < (_VECTORS_SIZE / 4))
COM_DIAG_AMIGO_UNEXPECTED(82)
#endif

#if (83 < (_VECTORS_SIZE / 4))
COM_DIAG_AMIGO_UNEXPECTED(83)
#endif

#if (84 < (_VECTORS_SIZE / 4))
COM_DIAG_AMIGO_UNEXPECTED(84)
#endif

#if (85 < (_VECTORS_SIZE / 4))
COM_DIAG_AMIGO_UNEXPECTED(85)
#endif

#if (86 < (_VECTORS_SIZE / 4))
COM_DIAG_AMIGO_UNEXPECTED(86)
#endif

#if (87 < (_VECTORS_SIZE / 4))
COM_DIAG_AMIGO_UNEXPECTED(87)
#endif

#if (88 < (_VECTORS_SIZE / 4))
COM_DIAG_AMIGO_UNEXPECTED(88)
#endif

#if (89 < (_VECTORS_SIZE / 4))
COM_DIAG_AMIGO_UNEXPECTED(89)
#endif

#if (90 < (_VECTORS_SIZE / 4))
COM_DIAG_AMIGO_UNEXPECTED(90)
#endif

#if (91 < (_VECTORS_SIZE / 4))
COM_DIAG_AMIGO_UNEXPECTED(91)
#endif

#if (92 < (_VECTORS_SIZE / 4))
COM_DIAG_AMIGO_UNEXPECTED(92)
#endif

#if (93 < (_VECTORS_SIZE / 4))
COM_DIAG_AMIGO_UNEXPECTED(93)
#endif

#if (94 < (_VECTORS_SIZE / 4))
COM_DIAG_AMIGO_UNEXPECTED(94)
#endif

#if (95 < (_VECTORS_SIZE / 4))
COM_DIAG_AMIGO_UNEXPECTED(95)
#endif

#if (96 < (_VECTORS_SIZE / 4))
COM_DIAG_AMIGO_UNEXPECTED(96)
#endif

#if (97 < (_VECTORS_SIZE / 4))
COM_DIAG_AMIGO_UNEXPECTED(97)
#endif

#if (98 < (_VECTORS_SIZE / 4))
COM_DIAG_AMIGO_UNEXPECTED(98)
#endif

#if (99 < (_VECTORS_SIZE / 4))
COM_DIAG_AMIGO_UNEXPECTED(99)
#endif

#if (100 < (_VECTORS_SIZE / 4))
COM_DIAG_AMIGO_UNEXPECTED(100)
#endif

#if (101 < (_VECTORS_SIZE / 4))
COM_DIAG_AMIGO_UNEXPECTED(101)
#endif

#if (102 < (_VECTORS_SIZE / 4))
COM_DIAG_AMIGO_UNEXPECTED(102)
#endif

#if (103 < (_VECTORS_SIZE / 4))
COM_DIAG_AMIGO_UNEXPECTED(103)
#endif

#if (104 < (_VECTORS_SIZE / 4))
COM_DIAG_AMIGO_UNEXPECTED(104)
#endif

#if (105 < (_VECTORS_SIZE / 4))
COM_DIAG_AMIGO_UNEXPECTED(105)
#endif

#if (106 < (_VECTORS_SIZE / 4))
COM_DIAG_AMIGO_UNEXPECTED(106)
#endif

#if (107 < (_VECTORS_SIZE / 4))
COM_DIAG_AMIGO_UNEXPECTED(107)
#endif

#if (108 < (_VECTORS_SIZE / 4))
COM_DIAG_AMIGO_UNEXPECTED(108)
#endif

#if (109 < (_VECTORS_SIZE / 4))
COM_DIAG_AMIGO_UNEXPECTED(109)
#endif

#if (110 < (_VECTORS_SIZE / 4))
COM_DIAG_AMIGO_UNEXPECTED(110)
#endif

#if (111 < (_VECTORS_SIZE / 4))
COM_DIAG_AMIGO_UNEXPECTED(111)
#endif

#if (112 < (_VECTORS_SIZE / 4))
COM_DIAG_AMIGO_UNEXPECTED(112)
#endif

#if (113 < (_VECTORS_SIZE / 4))
COM_DIAG_AMIGO_UNEXPECTED(113)
#endif

#if (114 < (_VECTORS_SIZE / 4))
COM_DIAG_AMIGO_UNEXPECTED(114)
#endif

#if (115 < (_VECTORS_SIZE / 4))
COM_DIAG_AMIGO_UNEXPECTED(115)
#endif

#if (116 < (_VECTORS_SIZE / 4))
COM_DIAG_AMIGO_UNEXPECTED(116)
#endif

#if (117 < (_VECTORS_SIZE / 4))
COM_DIAG_AMIGO_UNEXPECTED(117)
#endif

#if (118 < (_VECTORS_SIZE / 4))
COM_DIAG_AMIGO_UNEXPECTED(118)
#endif

#if (119 < (_VECTORS_SIZE / 4))
COM_DIAG_AMIGO_UNEXPECTED(119)
#endif

#if (120 < (_VECTORS_SIZE / 4))
COM_DIAG_AMIGO_UNEXPECTED(120)
#endif

#if (121 < (_VECTORS_SIZE / 4))
COM_DIAG_AMIGO_UNEXPECTED(121)
#endif

#if (122 < (_VECTORS_SIZE / 4))
COM_DIAG_AMIGO_UNEXPECTED(122)
#endif

#if (123 < (_VECTORS_SIZE / 4))
COM_DIAG_AMIGO_UNEXPECTED(123)
#endif

#if (124 < (_VECTORS_SIZE / 4))
COM_DIAG_AMIGO_UNEXPECTED(124)
#endif

#if (125 < (_VECTORS_SIZE / 4))
COM_DIAG_AMIGO_UNEXPECTED(125)
#endif

#if (126 < (_VECTORS_SIZE / 4))
COM_DIAG_AMIGO_UNEXPECTED(126)
#endif

#if (127 < (_VECTORS_SIZE / 4))
COM_DIAG_AMIGO_UNEXPECTED(127)
#endif

#endif
