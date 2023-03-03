/*
 * Copyright (C) 2001,2002,2003,2004 Philippe Gerum <rpm@xenomai.org>.
 *
 * ARM port
 *   Copyright (C) 2005 Stelian Pop
 * 
 * ARM64 port
 *   Copyright (C) 2015 Dmitriy Cherkasov <dmitriy@mperpetuo.com>
 *   Copyright (C) 2015 Gilles Chanteperdrix <gch@xenomai.org>
 *
 * Xenomai is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * Xenomai is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Xenomai; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 */

#include <linux/sched.h>
#include <linux/ipipe.h>
#include <linux/mm.h>
#include <linux/jump_label.h>
#include <asm/mmu_context.h>
#include <asm/fpu.h>
#include <cobalt/kernel/thread.h>
#include <asm/processor.h>
#include <asm/switch_to.h>
#include <asm/thread_info.h>


void xnarch_switch_to(struct xnthread *out, struct xnthread *in)
{
	struct xnarchtcb *out_tcb = &out->tcb, *in_tcb = &in->tcb;
	struct task_struct *prev, *next, *last;
	struct mm_struct *prev_mm, *next_mm;

	next = in_tcb->core.host_task;
	prev = out_tcb->core.host_task;
	prev_mm = out_tcb->core.active_mm;

	next_mm = in_tcb->core.mm;
	if (next_mm == NULL) {
		in_tcb->core.active_mm = prev_mm;
		enter_lazy_tlb(prev_mm, next);
	} else {
		ipipe_switch_mm_head(prev_mm, next_mm, next);
		/*
		 * We might be switching back to the root thread,
		 * which we preempted earlier, shortly after "current"
		 * dropped its mm context in the do_exit() path
		 * (next->mm == NULL). In that particular case, the
		 * kernel expects a lazy TLB state for leaving the mm.
		 */
		if (next->mm == NULL)
			enter_lazy_tlb(prev_mm, next);
	}

#if LINUX_VERSION_CODE < KERNEL_VERSION(4,9,0)
	ipipe_switch_to(prev, next);
	(void)last;
#else
	switch_to(prev, next, last);
#endif
}

int xnarch_escalate(void)
{
	if (ipipe_root_p) {
		ipipe_raise_irq(cobalt_pipeline.escalate_virq);
		return 1;
	}

	return 0;
}

static inline void thread_own_fpu(struct thread_info *ti, struct task_struct *p)
{
	enable_fpu();
	set_ti_thread_flag(ti, TIF_USEDFPU);
	KSTK_EUEN(p) |= CSR_EUEN_FPEN;
}

int xnarch_handle_fpu_fault(struct xnthread *from,
			struct xnthread *to, struct ipipe_trap_data *d)
{
	struct xnarchtcb *tcb = xnthread_archtcb(to);
	struct task_struct *p = tcb->core.host_task;
	struct thread_info *ti = tcb->core.tip;

	if (!(p->flags & PF_USED_MATH)) {
		unsigned int fcsr = p->thread.fpu.fcsr;

		thread_own_fpu(ti, p);
		_init_fpu(fcsr);
		p->flags |= PF_USED_MATH;
	} else {
		if (cpu_has_fpu && !test_ti_thread_flag(ti, TIF_USEDFPU)) {
			thread_own_fpu(ti, p);
			_restore_fp(&p->thread.fpu);
		}
	}

	xnlock_get(&nklock);
	xnthread_set_state(to, XNFPU);
	xnlock_put(&nklock);

	return 1;
}

void xnarch_switch_fpu(struct xnthread *f, struct xnthread *t)
{
	struct xnarchtcb *to_tcb = xnthread_archtcb(t);
	struct task_struct *to_p = to_tcb->core.host_task;
	struct thread_info *to_ti = to_tcb->core.tip;

	if (!(to_p->flags & PF_USED_MATH))
		return;
	
	if (cpu_has_fpu && !test_ti_thread_flag(to_ti, TIF_USEDFPU)) {
		thread_own_fpu(to_ti, to_p);
		_restore_fp(&to_p->thread.fpu);
	}
}
