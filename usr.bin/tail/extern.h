/*-
 * Copyright (c) 1991 The Regents of the University of California.
 * All rights reserved.
 *
 * %sccs.include.redist.c%
 *
 *	@(#)extern.h	5.1 (Berkeley) 07/21/91
 */

#define	WR(p, size) \
	if (write(STDOUT_FILENO, p, size) != size) \
		oerr();

enum STYLE { NOTSET = 0, FBYTES, FLINES, RBYTES, RLINES, REVERSE };

void	forward __P((FILE *, enum STYLE, long, struct stat *));
void	reverse __P((FILE *, enum STYLE, long, struct stat *));

void	bytes __P((FILE *, off_t));
void	lines __P((FILE *, off_t));

void	err __P((const char *fmt, ...));
void	ierr __P((void));
void	oerr __P((void));

extern int fflag, rflag, rval;
extern char *fname;
