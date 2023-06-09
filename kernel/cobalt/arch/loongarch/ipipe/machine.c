/**
 *   Copyright (C) 2005 Stelian Pop
 *
 *   Xenomai is free software; you can redistribute it and/or
 *   modify it under the terms of the GNU General Public License as
 *   published by the Free Software Foundation, Inc., 675 Mass Ave,
 *   Cambridge MA 02139, USA; either version 2 of the License, or (at
 *   your option) any later version.
 *
 *   Xenomai is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *   General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 *   02111-1307, USA.
 */

#include "asm/ipipe_base.h"
#include <linux/mm.h>
#include <linux/ipipe_tickdev.h>
#include <cobalt/kernel/arith.h>
#include <asm/cacheflush.h>
#include <asm/xenomai/machine.h>

#define CALIBRATION_LOOPS 10

static void mach_loongarch_prefault(struct vm_area_struct *vma)
{
	unsigned long addr;
	unsigned int flags;

	if ((vma->vm_flags & VM_MAYREAD)) {
		flags = (vma->vm_flags & VM_MAYWRITE) ? FAULT_FLAG_WRITE : 0;
		for (addr = vma->vm_start;
		     addr != vma->vm_end; addr += PAGE_SIZE)
#if LINUX_VERSION_CODE < KERNEL_VERSION(4,8,0)
			handle_mm_fault(vma->vm_mm, vma, addr, flags);
#else
			handle_mm_fault(vma, addr, flags);
#endif
	}
}

static const char *const fault_labels[] = {
	[IPIPE_TRAP_MAYDAY] = "Internal recovery trap",
	[IPIPE_TRAP_ACCESS] = "Data or instruction access",
	[IPIPE_TRAP_SECTION] = "Section fault",
	[IPIPE_TRAP_DABT] = "Generic data abort",
	[IPIPE_TRAP_UNKNOWN] = "Unknown exception",
	[IPIPE_TRAP_BREAK] = "Instruction breakpoint",
	[IPIPE_TRAP_FPU_ACC] = "Floating point access",
	[IPIPE_TRAP_LSX_ACC] = "LSX access",
	[IPIPE_TRAP_LASX_ACC] = "LASX access",
	[IPIPE_TRAP_FPU_EXC] = "Floating point exception",
	[IPIPE_TRAP_RI] = "Reserved instruction",
#ifdef IPIPE_TRAP_ALIGNMENT
	[IPIPE_TRAP_ALIGNMENT] = "Unaligned access exception",
#endif /* IPIPE_TRAP_ALIGNMENT */
	[IPIPE_NR_FAULTS] = NULL
};

struct cobalt_machine cobalt_machine = {
	.name = "loongarch64",
	.init = NULL,
	.late_init = NULL,
	.cleanup = NULL,
	.prefault = mach_loongarch_prefault,
	.fault_labels = fault_labels,
};
