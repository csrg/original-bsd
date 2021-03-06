/*
 * Copyright (c) 1992, 1993, 1994
 *	The Regents of the University of California.  All rights reserved.
 *
 * This code is derived from software donated to Berkeley by
 * Jan-Simon Pendry.
 *
 * %sccs.include.redist.c%
 */

#ifndef lint
char copyright[] =
"@(#) Copyright (c) 1992, 1993, 1994\n\
	The Regents of the University of California.  All rights reserved.\n";
#endif /* not lint */

#ifndef lint
static char sccsid[] = "@(#)mount_null.c	8.6 (Berkeley) 04/26/95";
#endif /* not lint */

#include <sys/param.h>
#include <sys/mount.h>
#include <miscfs/nullfs/null.h>

#include <err.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "mntopts.h"

struct mntopt mopts[] = {
	MOPT_STDOPTS,
	{ NULL }
};

int	subdir __P((const char *, const char *));
void	usage __P((void));

int
main(argc, argv)
	int argc;
	char *argv[];
{
	struct null_args args;
	int ch, mntflags;
	char target[MAXPATHLEN];

	mntflags = 0;
	while ((ch = getopt(argc, argv, "o:")) != EOF)
		switch(ch) {
		case 'o':
			getmntopts(optarg, mopts, &mntflags, 0);
			break;
		case '?':
		default:
			usage();
		}
	argc -= optind;
	argv += optind;

	if (argc != 2)
		usage();

	if (realpath(argv[0], target) == 0)
		err(1, "%s", target);

	if (subdir(target, argv[1]) || subdir(argv[1], target))
		errx(1, "%s (%s) and %s are not distinct paths",
		    argv[0], target, argv[1]);

	args.target = target;

	if (mount("loopback", argv[1], mntflags, &args))
		err(1, NULL);
	exit(0);
}

int
subdir(p, dir)
	const char *p;
	const char *dir;
{
	int l;

	l = strlen(dir);
	if (l <= 1)
		return (1);

	if ((strncmp(p, dir, l) == 0) && (p[l] == '/' || p[l] == '\0'))
		return (1);

	return (0);
}

void
usage()
{
	(void)fprintf(stderr,
		"usage: mount_null [-o options] target_fs mount_point\n");
	exit(1);
}
