/*-
 * Copyright (c) 1990 The Regents of the University of California.
 * All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * William Jolitz.
 *
 * %sccs.include.redist.c%
 */

#if defined(SYSLIBC_SCCS) && !defined(lint)
	.asciz "@(#)getppid.s	5.1 (Berkeley) 04/23/90"
#endif /* SYSLIBC_SCCS and not lint */

#include "SYS.h"

PSEUDO(getppid,getpid)
	movl	%edx,%eax
	ret			/* ppid = getppid(); */
