/*
 * Copyright (c) 1988 The Regents of the University of California.
 * All rights reserved.
 *
 * %sccs.include.redist.c%
 */

#if defined(LIBC_SCCS) && !defined(lint)
static char sccsid[] = "@(#)getpass.c	5.6 (Berkeley) 06/01/90";
#endif /* LIBC_SCCS and not lint */

#include <sys/termios.h>
#include <sys/signal.h>
#include <stdio.h>
#include <pwd.h>

char *
getpass(prompt)
	char *prompt;
{
	struct termios term;
	register int ch;
	register char *p;
	FILE *fp, *outfp;
	long omask;
	int echo;
	static char buf[_PASSWORD_LEN + 1];

	/*
	 * read and write to /dev/tty if possible; else read from
	 * stdin and write to stderr.
	 */
	if ((outfp = fp = fopen("/dev/tty", "w+")) == NULL) {
		outfp = stderr;
		fp = stdin;
	}
	/*
	 * note - blocking signals isn't necessarily the
	 * right thing, but we leave it for now.
	 */
	omask = sigblock(sigmask(SIGINT)|sigmask(SIGTSTP));
	(void)tcgetattr(fileno(fp), &term);
	if (echo = (term.c_lflag & ECHO)) {
		term.c_lflag &= ~ECHO;
		term.c_cflag |= CIGNORE;
		(void)tcsetattr(fileno(fp), TCSADFLUSH, &term);
	}
	(void)fputs(prompt, outfp);
	rewind(outfp);			/* implied flush */
	for (p = buf; (ch = getc(fp)) != EOF && ch != '\n';)
		if (p < buf + _PASSWORD_LEN)
			*p++ = ch;
	*p = '\0';
	(void)write(fileno(outfp), "\n", 1);
	if (echo) {
		term.c_lflag |= ECHO;
		term.c_cflag |= CIGNORE;
		tcsetattr(fileno(fp), TCSADFLUSH, &term);
	}
	(void)sigsetmask(omask);
	if (fp != stdin)
		(void)fclose(fp);
	return(buf);
}
