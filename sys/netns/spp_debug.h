/*
 * Copyright (c) 1984, 1985, 1986, 1987, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * %sccs.include.redist.c%
 *
 *	@(#)spp_debug.h	8.1 (Berkeley) 06/10/93
 */

struct	spp_debug {
	u_long	sd_time;
	short	sd_act;
	short	sd_ostate;
	caddr_t	sd_cb;
	short	sd_req;
	struct	spidp sd_si;
	struct	sppcb sd_sp;
};

#define	SA_INPUT 	0
#define	SA_OUTPUT	1
#define	SA_USER		2
#define	SA_RESPOND	3
#define	SA_DROP		4

#ifdef SANAMES
char	*sanames[] =
    { "input", "output", "user", "respond", "drop" };
#endif

#define	SPP_NDEBUG 100
struct	spp_debug spp_debug[SPP_NDEBUG];
int	spp_debx;
