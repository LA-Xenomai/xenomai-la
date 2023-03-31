/*
 * Copyright (C) 2008 Gilles Chanteperdrix <gilles.chanteperdrix@xenomai.org>.
 *
 * LoongArch Port
 * Copyright (C) 2022 Hao Miao <haomiao19@mails.ucas.ac.cn>.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.

 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA.
 */
#ifndef _COBALT_LOONGARCH_ASM_UAPI_ARITH_H
#define _COBALT_LOONGARCH_ASM_UAPI_ARITH_H

#include <asm/xenomai/uapi/features.h>

// static inline __attribute__((__const__)) long long
// mach_loongarch_llimd (long long op, unsigned m, unsigned d)
// {
// 	long long res;

// 	__asm__ __volatile__ (
// 		"mul.d		$t0, %[op], %[mul]	\t\n"
// 		"mulh.d		$t1, %[op], %[mul]	\t\n"
// 		"ori		$t2, $zero, 1		\t\n"
// 		"slli.d		$t2, $t2, 63		\t\n"
// 		"div.du		$t2, $t2, %[div]	\t\n"
// 		"slli.d		$t1, $t1, 1		\t\n"
// 		"mul.d		$t1, $t1, $t2		\t\n"
// 		"div.du 	$t0, $t0, %[div]	\t\n"
// 		"add.d		%[res], $t0, $t1	\t\n"
// 		: [res] "=r" (res)
// 		: [op] "r" (op), [mul] "r" (m), [div] "r" (d)
// 		: "$t0", "$t1", "$t2", "memory");

// 	return res;
// }
// #define xnarch_llimd(ll,m,d) mach_loongarch_llimd((ll),(m),(d))

static inline __attribute__((__const__)) long long
mach_loongarch_llmulshft(long long op, unsigned m, unsigned s)
{
	unsigned long long res, mul = (unsigned long long)m;

	__asm__ __volatile__ (
		"mul.d	$t0, %[op], %[mul]	\t\n"
		"mulh.d	$t1, %[op], %[mul]	\t\n"
		"srl.d	$t0, $t0, %[shft]	\t\n"
		"li.d	$t2, 64			\t\n"
		"sub.w	%[shft], $t2, %[shft]	\t\n"
		"sll.d	$t1, $t1, %[shft]	\t\n"
		"or	%[res], $t0, $t1	\t\n"
		: [res] "=r" (res)
		: [op] "r" (op), [mul] "r" (mul), [shft] "r" (s)
		: "$t0", "$t1", "$t2", "memory");

	return res;
}
#define xnarch_llmulshft(op, m, s) mach_loongarch_llmulshft((op), (m), (s))

#include <cobalt/uapi/asm-generic/arith.h>

#endif /* _COBALT_LOONGARCH_ASM_UAPI_ARITH_H */
