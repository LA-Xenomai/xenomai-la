/*
 * SPDX-License-Identifier: GPL-2.0
 *
 * Copyright (C) 2006 Gilles Chanteperdrix <gilles.chanteperdrix@xenomai.org>
 */

#ifndef _COBALT_LOONGARCH_FPTEST_H
#define _COBALT_LOONGARCH_FPTEST_H

#include <linux/errno.h>
#include <asm/xenomai/uapi/fptest.h>
#include <asm/hwcap.h>

#define have_fp 0x1

static inline int fp_kernel_supported(void)
{
	return 1;
}

static inline int fp_linux_begin(void)
{
	return -ENOSYS;
}

static inline void fp_linux_end(void)
{
}

static inline int fp_detect(void)
{
	return have_fp ? __COBALT_HAVE_FPU : 0;
}

#endif /* !_COBALT_LOONGARCH_FPTEST_H */
