/*-
 * Copyright (c) 1980, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * %sccs.include.proprietary.c%
 */

#ifndef lint
static char sccsid[] = "@(#)cont.c	8.1 (Berkeley) 06/04/93";
#endif /* not lint */

cont_(x,y)
int *x, *y;
{
	cont(*x, *y);
}
