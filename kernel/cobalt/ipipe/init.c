/*
 * SPDX-License-Identifier: GPL-2.0
 *
 * Copyright (C) 2019 Philippe Gerum  <rpm@xenomai.org>
 */

#include <linux/init.h>
#include <linux/module.h>
#include <pipeline/machine.h>
#include <linux/ipipe_tickdev.h>
#include <cobalt/kernel/sched.h>
#include <cobalt/kernel/clock.h>

int __init pipeline_init(void)
{
	struct ipipe_sysinfo sysinfo;
	// struct ipipe_domain *ipd;
	int ret, virq;
	// int irq, ctlbits;
	// char handling, lockbit, virtuality;

	ret = ipipe_select_timers(&xnsched_realtime_cpus);
	if (ret < 0)
		return ret;

	ipipe_get_sysinfo(&sysinfo);

	cobalt_pipeline.clock_freq = sysinfo.sys_hrclock_freq;

	if (cobalt_machine.init) {
		ret = cobalt_machine.init();
		if (ret)
			return ret;
	}

	ipipe_register_head(&xnsched_primary_domain, "Xenomai");

	virq = ipipe_alloc_virq();
	if (virq == 0)
		goto fail_escalate;

	cobalt_pipeline.escalate_virq = virq;

	ipipe_request_irq(&xnsched_primary_domain,
			  cobalt_pipeline.escalate_virq,
			  (ipipe_irq_handler_t)__xnsched_run_handler,
			  NULL, NULL);

	ret = xnclock_init();
	if (ret)
		goto fail_clock;

	// ipd = ipipe_head_domain;
	// mutex_lock(&ipd->mutex);

	// for (irq = 0; irq < IPIPE_NR_IRQS; irq++) {
	// 	ctlbits = ipd->irqs[irq].control;
	// 	/*
	// 	 * There might be a hole between the last external IRQ
	// 	 * and the first virtual one; skip it.
	// 	 */
	// 	if (irq >= IPIPE_NR_XIRQS && !ipipe_virtual_irq_p(irq))
	// 		continue;

	// 	if (ctlbits & IPIPE_HANDLE_MASK)
	// 		handling = 'H';
	// 	else
	// 		handling = '.';

	// 	if (ctlbits & IPIPE_LOCK_MASK)
	// 		lockbit = 'L';
	// 	else
	// 		lockbit = '.';

	// 	if (ipipe_virtual_irq_p(irq))
	// 		virtuality = 'V';
	// 	else
	// 		virtuality = '.';

	// 	if (ctlbits & IPIPE_HANDLE_MASK)
	// 		printk(" %4u:  %c%c%c  %pf\n",
	// 			   irq, handling, lockbit, virtuality,
	// 			   ipd->irqs[irq].handler);
	// 	// else
	// 	// 	printk(" %4u:  %c%c%c\n",
	// 	// 		   irq, handling, lockbit, virtuality);
	// }

	// mutex_unlock(&ipd->mutex);

	return 0;

fail_clock:
	ipipe_free_irq(&xnsched_primary_domain,
		       cobalt_pipeline.escalate_virq);
	ipipe_free_virq(cobalt_pipeline.escalate_virq);
fail_escalate:
	ipipe_unregister_head(&xnsched_primary_domain);

	if (cobalt_machine.cleanup)
		cobalt_machine.cleanup();

	return ret;
}

int __init pipeline_late_init(void)
{
	if (cobalt_machine.late_init)
		return cobalt_machine.late_init();

	return 0;
}

__init void pipeline_cleanup(void)
{
	ipipe_unregister_head(&xnsched_primary_domain);
	ipipe_free_irq(&xnsched_primary_domain,
		       cobalt_pipeline.escalate_virq);
	ipipe_free_virq(cobalt_pipeline.escalate_virq);
	ipipe_timers_release();
	xnclock_cleanup();
}
