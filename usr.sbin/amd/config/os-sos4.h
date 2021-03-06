/*
 * Copyright (c) 1989 Jan-Simon Pendry
 * Copyright (c) 1989 Imperial College of Science, Technology & Medicine
 * Copyright (c) 1989, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Jan-Simon Pendry at Imperial College, London.
 *
 * %sccs.include.redist.c%
 *
 *	@(#)os-sos4.h	8.1 (Berkeley) 06/06/93
 *
 * $Id: os-sos4.h,v 5.2.2.1 1992/02/09 15:10:41 jsp beta $
 *
 * SunOS 4.0 definitions for Amd (automounter)
 */

/*
 * Does the compiler grok void *
 */
#define	VOIDP

/*
 * What type is free(void*) returning?
 */
#undef FREE_RETURN_TYPE
#define FREE_RETURN_TYPE	int

/*
 * Which version of the Sun RPC library we are using
 * This is the implementation release number, not
 * the protocol revision number.
 */
#define	RPC_4

/*
 * Which version of the NFS interface are we using.
 * This is the implementation release number, not
 * the protocol revision number.
 */
#define	NFS_4

/*
 * Does this OS have NDBM support?
 */
#define OS_HAS_NDBM

/*
 * Byte ordering
 */
#undef ARCH_ENDIAN
#if defined(mc68010) || defined(mc68020) || defined(sparc)
#define	ARCH_ENDIAN	"big"
#endif
#if defined(i386)
#define ARCH_ENDIAN	"little"
#endif

/*
 * Name of filesystem types
 */
#define MOUNT_TYPE_NFS	"nfs"
#define MOUNT_TYPE_UFS	"4.2"

/*
 * Type of a file handle
 */
#undef NFS_FH_TYPE
#define	NFS_FH_TYPE	caddr_t

/*
 * Type of filesystem type
 */
#undef MTYPE_TYPE
#define	MTYPE_TYPE	char *

/*
 * Add support for SunOS 4 automounter files
 */
#define	SUNOS4_COMPAT

/*
 * System Vr4 / SunOS 4.1 compatibility
 * - put dev= in the options list
 *
 * From: Brent Callaghan <brent@eng.sun.com>
 */
#define	MNTINFO_DEV	"dev"
#define MNTINFO_PREF	""
