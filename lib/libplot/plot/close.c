#ifndef lint
static char sccsid[] = "@(#)close.c	4.1 (Berkeley) 06/27/83";
#endif

#include <stdio.h>
closevt(){
	fflush(stdout);
}
closepl(){
	fflush(stdout);
}
