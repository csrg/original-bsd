/*-
 * Copyright (c) 1983, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * %sccs.include.proprietary.c%
 */

#ifndef lint
static char sccsid[] = "@(#)point.c	8.1 (Berkeley) 06/04/93";
#endif /* not lint */

#include "con.h"
point(xi,yi){
		move(xi,yi);
		label(".");
		return;
}
