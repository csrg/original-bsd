/*-
 * Copyright (c) 1990 The Regents of the University of California.
 * All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Cimarron D. Taylor of the University of California, Berkeley.
 *
 * %sccs.include.redist.c%
 *
 *	@(#)find.h	5.7 (Berkeley) 05/23/91
 */

/* node type */
enum ntype {
	N_AND = 1, 				/* must start > 0 */
	N_ATIME, N_CLOSEPAREN, N_CTIME, N_DEPTH, N_EXEC, N_EXPR, N_FOLLOW,
	N_FSTYPE, N_GROUP, N_INUM, N_LINKS, N_LS, N_MTIME, N_NAME, N_NEWER,
	N_NOGROUP, N_NOT, N_NOUSER, N_OK, N_OPENPAREN, N_OR, N_PERM, N_PRINT,
	N_PRUNE, N_SIZE, N_TYPE, N_USER, N_XDEV,
};

/* node definition */
typedef struct _plandata {
	struct _plandata *next;			/* next node */
	int (*eval)();				/* node evaluation function */
	int flags;				/* private flags */
	enum ntype type;			/* plan node type */
	union {
		gid_t _g_data;			/* gid */
		ino_t _i_data;			/* inode */
		mode_t _m_data;			/* mode mask */
		nlink_t _l_data;		/* link count */
		off_t _o_data;			/* file size */
		time_t _t_data;			/* time value */
		uid_t _u_data;			/* uid */
		struct _plandata *_p_data[2];	/* PLAN trees */
		struct _ex {
			char **_e_argv;		/* argv array */
			char **_e_orig;		/* original strings */
			int *_e_len;		/* allocated length */
		} ex;
		char *_a_data[2];		/* array of char pointers */
		char *_c_data;			/* char pointer */
	} p_un;
#define	a_data	p_un._a_data
#define	c_data	p_un._c_data
#define	i_data	p_un._i_data
#define	g_data	p_un._g_data
#define	l_data	p_un._l_data
#define	m_data	p_un._m_data
#define	o_data	p_un._o_data
#define	p_data	p_un._p_data
#define	t_data	p_un._t_data
#define	u_data	p_un._u_data
#define	e_argv	p_un.ex._e_argv
#define	e_orig	p_un.ex._e_orig
#define	e_len	p_un.ex._e_len
} PLAN;

#define	error(name, number) \
	(void)fprintf(stderr, "find: %s: %s\n", name, strerror(number));

extern int ftsoptions;
extern int isdeprecated, isdepth, isoutput, isrelative, isxargs;
void *emalloc();
