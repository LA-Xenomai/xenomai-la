/*
 * Copyright (C) 2015 Philippe Gerum <rpm@xenomai.org>.
 * Copyright (C) Siemens AG, 2021
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
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA.
 */
#ifndef _LIB_COBALT_LOONGARCH_SYSCALL_H
#define _LIB_COBALT_LOONGARCH_SYSCALL_H

#include <xeno_config.h>
#include <errno.h>
#include <cobalt/uapi/syscall.h>

#define __xn_syscall_args0
#define __xn_syscall_args1 , long int __a1
#define __xn_syscall_args2 __xn_syscall_args1, long int __a2
#define __xn_syscall_args3 __xn_syscall_args2, long int __a3
#define __xn_syscall_args4 __xn_syscall_args3, long int __a4
#define __xn_syscall_args5 __xn_syscall_args4, long int __a5

#define __emit_syscall0(__args...)					\
	register long int __scno __asm__("a7") = __xn_syscode(__op); \
	register long int __res __asm__("a0");			\
	__asm__ __volatile__ (						\
		"syscall 0x0;\n\t"					\
		: "=r" (__res)						\
		: "r" (__scno), ##__args				\
		: "cc", "memory");					\
	return __res
#define __emit_syscall1(__args...)					\
	register long int __x0 __asm__("a0") = __a1;		\
	__emit_syscall0("r" (__x0),  ##__args)
#define __emit_syscall2(__args...)					\
	register long int __x1 __asm__("a1") = __a2;		\
	__emit_syscall1("r" (__x1), ##__args)
#define __emit_syscall3(__args...)					\
	register long int __x2 __asm__("a2") = __a3;		\
	__emit_syscall2("r" (__x2), ##__args)
#define __emit_syscall4(__args...)					\
	register long int __x3 __asm__("a3") = __a4;		\
	__emit_syscall3("r" (__x3), ##__args)
#define __emit_syscall5(__args...)	\
	register long int __x4 __asm__("a4") = __a5;		\
	__emit_syscall4("r" (__x4), ##__args)

#define DEFINE_XENOMAI_SYSCALL(__argnr)					\
long int		\
__xenomai_do_syscall##__argnr(long int __op				\
			      __xn_syscall_args##__argnr)		\
{									\
	__emit_syscall##__argnr();					\
}

#define DECL_XENOMAI_SYSCALL(__argnr)					\
long int		\
__xenomai_do_syscall##__argnr(long int __op				\
			      __xn_syscall_args##__argnr)		\

DECL_XENOMAI_SYSCALL(0);
DECL_XENOMAI_SYSCALL(1);
DECL_XENOMAI_SYSCALL(2);
DECL_XENOMAI_SYSCALL(3);
DECL_XENOMAI_SYSCALL(4);
DECL_XENOMAI_SYSCALL(5);

#define XENOMAI_SYSCALL0(__op)					\
	__xenomai_do_syscall0(__op)
#define XENOMAI_SYSCALL1(__op, __a1)				\
	__xenomai_do_syscall1(__op,				\
			      (long int)__a1)
#define XENOMAI_SYSCALL2(__op, __a1, __a2)			\
	__xenomai_do_syscall2(__op,				\
			      (long int)__a1,		\
			      (long int)__a2)
#define XENOMAI_SYSCALL3(__op, __a1, __a2, __a3)		\
	__xenomai_do_syscall3(__op,				\
			      (long int)__a1,		\
			      (long int)__a2,		\
			      (long int)__a3)
#define XENOMAI_SYSCALL4(__op, __a1, __a2, __a3, __a4)		\
	__xenomai_do_syscall4(__op,				\
			      (long int)__a1,		\
			      (long int)__a2,		\
			      (long int)__a3,		\
			      (long int)__a4)
#define XENOMAI_SYSCALL5(__op, __a1, __a2, __a3, __a4, __a5)	\
	__xenomai_do_syscall5(__op,				\
			      (long int)__a1,		\
			      (long int)__a2,		\
			      (long int)__a3,		\
			      (long int)__a4,		\
			      (long int)__a5)
#define XENOMAI_SYSBIND(__breq)					\
	__xenomai_do_syscall1(sc_cobalt_bind,			\
			      (long int)__breq)

#endif /* !_LIB_COBALT_LOONGARCH_SYSCALL_H */
