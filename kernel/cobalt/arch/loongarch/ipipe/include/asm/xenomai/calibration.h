/*
 * SPDX-License-Identifier: GPL-2.0
 *
 * Copyright (C) 2015 Philippe Gerum  <rpm@xenomai.org>
 */

#ifndef _COBALT_LOONGARCH_CALIBRATION_H
#define _COBALT_LOONGARCH_CALIBRATION_H

static inline void xnarch_get_latencies(struct xnclock_gravity *p)
{
	unsigned int ulat;
#if CONFIG_XENO_OPT_TIMING_SCHEDLAT != 0
	ulat = CONFIG_XENO_OPT_TIMING_SCHEDLAT;
#elif defined(CONFIG_ARCH_HISI)
	ulat = 4000;
#else
	ulat = 4000;
#endif
	p->user = xnclock_ns_to_ticks(&nkclock, ulat);
	p->kernel = xnclock_ns_to_ticks(&nkclock, CONFIG_XENO_OPT_TIMING_KSCHEDLAT);
	p->irq = xnclock_ns_to_ticks(&nkclock, CONFIG_XENO_OPT_TIMING_IRQLAT);
}

#endif /* !_COBALT_LOONGARCH_CALIBRATION_H */
