/*
 * SPDX-License-Identifier: GPL-2.0
 *
 * Copyright (C) 2015 Dmitriy Cherkasov <dmitriy@mperpetuo.com>
 */

#ifndef _COBALT_LOONGARCH_MACHINE_H
#define _COBALT_LOONGARCH_MACHINE_H

#include <linux/version.h>
#include <asm/byteorder.h>
#include <cobalt/kernel/assert.h>

/* D-side always behaves as PIPT on AArch64 (see arch/loongarch/include/asm/cachetype.h) */
#define xnarch_cache_aliasing() 0

static inline __attribute_const__ unsigned long ffnz(unsigned long ul)
{
	int __r;

	/* zero input is not valid */
	XENO_WARN_ON(COBALT, ul == 0);

	__asm__ (
	         "clz.w\t%0, %1\n"
	        : "=r" (__r) : "r"(ul) : "cc");

	return __r;
}

#include <asm-generic/xenomai/machine.h>

#endif /* !_COBALT_LOONGARCH_MACHINE_H */
