/*
 * Copyright (C) 2005 Stelian Pop
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
#ifndef _COBALT_LOONGARCH_ASM_THREAD_H
#define _COBALT_LOONGARCH_ASM_THREAD_H

#include <linux/ipipe.h>
#include <linux/version.h>
#include <asm-generic/xenomai/ipipe/thread.h>

struct xnarchtcb {
	struct xntcb core;
};

#define xnarch_fault_regs(d)	((d)->regs)
#define xnarch_fault_trap(d)	((d)->exception)
#define xnarch_fault_code(d)	(0)
#define xnarch_fault_pc(d)	((unsigned long)((d)->regs->csr_era))

#define xnarch_fault_pf_p(d)	((d)->exception == IPIPE_TRAP_ACCESS)
#define xnarch_fault_fpu_p(d)	((d)->exception == IPIPE_TRAP_FPU_ACC)
#define xnarch_fault_bp_p(d)	((current->ptrace & PT_PTRACED) &&	\
				 ((d)->exception == IPIPE_TRAP_BREAK ||	\
				  (d)->exception == IPIPE_TRAP_RI))

#define xnarch_fault_notify(d) (!xnarch_fault_bp_p(d))

static inline
struct task_struct *xnarch_host_task(struct xnarchtcb *tcb)
{
	return tcb->core.host_task;
}

void xnarch_switch_to(struct xnthread *out, struct xnthread *in);

static inline void xnarch_enter_root(struct xnthread *root) { }

int xnarch_escalate(void);
void xnarch_init_shadow_tcb(struct xnthread *thread);

static inline void xnarch_init_root_tcb(struct xnthread *thread) { }
static inline void xnarch_leave_root(struct xnthread *root) { }
static inline void xnarch_switch_fpu(struct xnthread *f, struct xnthread *t) { }

int xnarch_handle_fpu_fault(struct xnthread *from,
			struct xnthread *to, struct ipipe_trap_data *d);

static inline void xnarch_enable_kfpu(void) { }

static inline void xnarch_disable_kfpu(void) { }

#endif /* !_COBALT_LOONGARCH_ASM_THREAD_H */
