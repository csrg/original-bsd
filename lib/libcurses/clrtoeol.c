# include	"curses.ext"

/*
 *	This routine clears up to the end of line
 *
 * @(#)clrtoeol.c	1.3 (Berkeley) 05/01/85
 */
wclrtoeol(win)
reg WINDOW	*win; {

	reg char	*sp, *end;
	reg int		y, x;
	reg char	*maxx;
	reg int		minx;

	y = win->_cury;
	x = win->_curx;
	end = &win->_y[y][win->_maxx];
	minx = _NOCHANGE;
	maxx = &win->_y[y][x];
	for (sp = maxx; sp < end; sp++)
		if (*sp != ' ') {
			maxx = sp;
			if (minx == _NOCHANGE)
				minx = sp - win->_y[y];
			*sp = ' ';
		}
	/*
	 * update firstch and lastch for the line
	 */
	touchline(win, y, win->_curx, win->_maxx - 1);
# ifdef DEBUG
	fprintf(outf, "CLRTOEOL: minx = %d, maxx = %d, firstch = %d, lastch = %d\n", minx, maxx - win->_y[y], win->_firstch[y], win->_lastch[y]);
# endif
}
