#ifndef lint
static char sccsid[] = "@(#)label.c	4.1 (Berkeley) 06/27/83";
#endif

extern vti;

label(s)
char *s;
{
	int i, o;

	o = 01401;
	write(vti, &o, 2);
	for(i=0; s[i++]; )
		;
	write(vti, s, i);
}
