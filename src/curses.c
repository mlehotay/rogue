/*
 * Copyright (c) 1988, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Timothy C. Stoehr.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	  This product includes software developed by the University of
 *	  California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * @(#)curses.c 8.1 (Berkeley) 5/31/93
 */

/*
 * curses.c
 *
 * This source herein may be modified and/or distributed by anybody who
 * so desires, with the following restrictions:
 *    1.)  No portion of this notice shall be removed.
 *    2.)  Credit shall not be taken for the creation of this source.
 *    3.)  This code is not to be traded, sold, or used for personal
 *         gain or profit.
 *
 */

/* NS, June 2003: Color Curses
 *
 * This is a modified version of the Rogue curses.c library that supports
 * basic IBM color graphics.  Everything here ought to work on almost all
 * modern Windows machines, but there will always be exceptions.  I don't
 * pretend to understand all of the DPMI and EGA/VGA issues; I merely lifted
 * code (with attributions) from a variety of C libraries posted on the Web,
 * and it all works on my various home machines (built 1996-2000).  A good
 * DOS games developer is invited to generalize this library further.
 *
 * Among the significant changes:
 *   o Modified curses to compile under djgpp as well as Watcom.  Borrowed
 *     the DPMI macros and typedefs from public domain code offered by
 *     Chris Geise (http://my.execpc.com/~geezer/osd/boot/size.c).
 *   o Changed the two internal buffers to be 16-bit rather than 8-bit,
 *     allowing color information to be stored with the character.
 *   o Created color_char versions of the basic curses functions.
 *	 o Created an "overlay" function for pop-up messages and other
 *     useful windowing effects.
 */

#include <stdio.h>
#include "rogue.h"


/* ============== COMPILER-SPECIFIC CONSTANTS AND TYPEDEFS =============== */

/* --- For compiling under Open Watcom --- */
#ifdef __WATCOMC__

#include <dos.h>

typedef union REGPACK  regs_t;
#define	R_AX		w.ax
#define	R_BX		w.bx
#define	R_CX		w.cx
#define	R_DX		w.dx
#define	R_FLAGS		w.flags

/* WARNING: for 32-bit code, unused fields of regs_t
must be zeroed before using this macro */
#define	intr_fn(N,R)	intr(N,R)


/* --- For compiling under djgpp --- */
#elif __DJGPP__

#include <dpmi.h>

typedef __dpmi_regs regs_t;
#define	R_AX		x.ax
#define	R_BX		x.bx
#define	R_CX		x.cx
#define	R_DX		x.dx
#define	R_FLAGS		x.flags

#define	intr_fn(N,R)	__dpmi_int(N,R)


/* --- Else you're on your own --- */
#else
#error Sorry, curses.c compiles under Open Watcom or djgpp
#endif



/* ============== CONSTANTS AND TYPEDEFS =============== */

struct _win_st {
	short _cury, _curx;
	short _maxy, _maxx;
};
typedef struct _win_st WINDOW;


typedef union _colorChar	COLORBUF;
#define BLANKCH				MAKE_COLOR_CHAR(WHITE,BLACK,' ')



/* ================== GLOBAL VARIABLES ================== */

int LINES=DROWS, COLS=DCOLS;


// static char terminal[DROWS][DCOLS];
// static char buffer[DROWS][DCOLS];
static COLORBUF terminal[DROWS][DCOLS];
static COLORBUF buffer[DROWS][DCOLS];

static boolean screen_dirty;
static boolean lines_dirty[DROWS];
static boolean buf_stand_out = 0;

static WINDOW scr_buf;
static WINDOW *curscr = &scr_buf;
static short cur_row, cur_col;
static unsigned char videomode, videopage;



/* ================= PRIVATE PROTOTYPES ================= */

static clear_buffers();
static cls();
/* static put_char_at(register int row, register int col, char ch); */
static put_color_char_at(register int row, register int col, COLORBUF cb);
static put_cursor(register int row, register int col);
static char translate_keypad(int scancode);
static wrefresh(WINDOW *scr);



/* ================== PUBLIC FUNCTIONS ================== */

/*  Returns a keystroke, translated into a Rogue command.
 */
rgetchar()
{
	register ch;
    register scancode;
	regs_t regs;

	for(;;) {
        regs.h.ah = 0x00; /* get keystroke */
        intr_fn(0x16, &regs);
        scancode = regs.h.ah; /* BIOS scancode */

        if(scancode>0x46 && scancode<0x54)
            ch = translate_keypad(scancode);
        else
            ch = regs.h.al; /* ASCII code */

        switch(ch) {
		case '\022': /* ^R */
			wrefresh(curscr);
			break;
        case '&':
            save_screen();
			break;
		default:
			return(ch);
		}
	}
}


/*  Initializes the screen and pulls in the DPMI mode and page in use
 */
initscr()
{
	regs_t regs;

    regs.h.ah = 0x0F;
    intr_fn(0x10, &regs);
    videomode = regs.h.al;
    videopage = regs.h.bh;
    COLS = regs.h.ah;
}


move(row, col)
short row, col;
{
	curscr->_cury = row;
	curscr->_curx = col;
	screen_dirty = 1;
}



/* Adds a string to the screen at the current cursor position (addstr,
 * addstr_in_color, addcstr) or at the specified position (mvaddstr,
 * mvaddstr_in_color, mvaddcstr) in B&W (*addstr), B&W converted to
 * color (*addstr_in_color) or as a fully colorized string (*addcstr).
 */

addstr(char *str)
{
	while (*str) {
		addch((int) *str++);
	}
}


mvaddstr(row, col, str)
short row, col;
char *str;
{
	move(row, col);
	addstr(str);
}


addstr_in_color(char *str, byte color)
{
	color_char cc;
	while (*str) {
		cc.b8.color = color;
		cc.b8.ch = *str;
		addcch(cc);
		str++;
	}
}


mvaddstr_in_color(short row, short col, char *str, byte color)
{
	move(row, col);
	addstr_in_color(str, color);
}


addcstr(color_char *cstr)
{
	while ((*cstr).b16 != 0) {
		addcch(*cstr++);
	}
}


mvaddcstr(short row, short col, color_char *cstr)
{
	move(row, col);
	addcstr(cstr);
}



/* Adds a character to the screen at the current cursor position
 * (addch, addcch) or at the specified position (mvaddch, mvaddcch), in
 * B&W (addch, mvaddch) or color (addcch, mvaddcch).
 */

addch(register int ch)
{
	color_char cc;

	cc.b8.ch = ch;
	if (buf_stand_out) {
		cc.b8.color = MAKE_COLOR(BRIGHT_WHITE, BLACK);
	} else {
		cc.b8.color = MAKE_COLOR(WHITE, BLACK);
	}

	addcch(cc);
}

mvaddch(short row, short col, int ch)
{
	move(row, col);
	addch(ch);
}

addcch(color_char cc)
{
	short row, col;

	row = curscr->_cury;
	col = curscr->_curx++;

	buffer[row][col].b16 = cc.b16;
	lines_dirty[row] = 1;
	screen_dirty = 1;
}

mvaddcch(short row, short col, color_char cc)
{
	move(row, col);
	addcch(cc);
}



/* Turns a dull B&W string into a vibrant color one. :-)
 */

colorize(char *str, byte color, color_char *cstr)
{
	while (*str != '\0') {
		(*cstr).b8.color = color;
		(*cstr).b8.ch = *str;
		str++;
		cstr++;
	}
	(*cstr).b16 = 0;
}



/* These functions redraw the whole screen (redraw) or just the dirty lines
 * of the current screen (refresh)
 */

refresh()
{
	register i, j, line;
	short old_row, old_col, first_row;

	if (screen_dirty) {

		old_row = curscr->_cury;
		old_col = curscr->_curx;
		first_row = cur_row;

		for (i = 0; i < DROWS; i++) {
			line = (first_row + i) % DROWS;
			if (lines_dirty[line]) {
				for (j = 0; j < DCOLS; j++) {
					if (buffer[line][j].b16 != terminal[line][j].b16) {
						put_color_char_at(line, j, buffer[line][j]);
					}
				}
				lines_dirty[line] = 0;
			}
		}
		put_cursor(old_row, old_col);
		screen_dirty = 0;
		fflush(stdout);
	}
}

redraw() {
	wrefresh(curscr);
}



/* Returns the character currently at the specified current position
 */
mvinch(row, col)
short row, col;
{
	move(row, col);
	return((int) (buffer[row][col].b8.ch));
}

color_char mvincch(short row, short col)
{
	move(row, col);
	return (buffer[row][col]);
}



/* Clears the whole screan (clear) or to the current EOL (clrtoeol)
 */

clear()
{
    cls();
	fflush(stdout);
	cur_row = cur_col = 0;
	move(0, 0);
	clear_buffers();
}

clrtoeol()
{
	short row, col;

	row = curscr->_cury;

	for (col = curscr->_curx; col < DCOLS; col++) {
		buffer[row][col].b16 = BLANKCH;
	}
	lines_dirty[row] = 1;
}



/*  Turns bold characters on and off.  Legacy functions.
 */

standout()
{
	buf_stand_out = 1;
}

standend()
{
	buf_stand_out = 0;
}



/*  NS 14 June 2003:  Draws a box using the given character set.
 *	Specify size as the upper-left corner, the height, and the width.
 *  Specify cset in order: horiz, vert, and 4 corners: ul, ur, ll, lr
 */

void
draw_box(color_char cset[6], int ulrow, int ulcol, int height, int width)
{
	int i;
	const int toprow = ulrow;
	const int bottomrow = ulrow + height - 1;
	const int leftcol = ulcol;
	const int rightcol = ulcol + width - 1;

	/* check for nonsense */
	if (height <= 1 || width <= 1
			|| toprow < 0 || toprow >= DROWS
			|| leftcol < 0 || leftcol >= DCOLS
			|| bottomrow < 0 || bottomrow >= DROWS
			|| rightcol < 0 || rightcol >= DCOLS)
		return;

	/* draw the walls */
	for (i = leftcol + 1; i < rightcol; i++) {
		mvaddcch(toprow, i, cset[0]);
		mvaddcch(bottomrow, i, cset[0]);
	}
	for (i = toprow + 1; i < bottomrow; i++) {
		mvaddcch(i, leftcol, cset[1]);
		mvaddcch(i, rightcol, cset[1]);
	}

	/* add the corners */
	mvaddcch(toprow, leftcol, cset[2]);
	mvaddcch(toprow, rightcol, cset[3]);
	mvaddcch(bottomrow, leftcol, cset[4]);
	mvaddcch(bottomrow, rightcol, cset[5]);
}


/*   Stubs called during game initialization and exit.
 */

endwin()
{
}

crmode()
{
}

noecho()
{
}

nonl()
{
}


/* NS: This is experimental, but if it works as it should then we can
 *	create arbitrary 'popup' windows which will make messaging and
 *  inventories a lot easier.  Note that cca is destructively
 *  modified during the function call (because we use it as a backing
 *  store buffer).  Arguments:
 *		    cca: holds the message you want to display
 *	   	   rows: lines in the message
 *	   	   cols: columns in the message
 *		  ulrow: upper-left row of the subwindow
 *		  ulcol: upper-left column of the subwindow
 */
/*
overlay(color_char cca[][], int rows, int cols, int ulrow, int ulcol)
{
	int r, c, ar, ar0, ac;
	int lrrow, lrcol;
	color_char cc;

	// compute window dimensions and override any insanity
	if (rows < 1  ||  cols < 1)
		return 0;
	ulrow = MAX(MIN_ROW, ulrow);
	ulcol = MAX(1, ulcol);
	lrrow = MIN(ulrow + rows, DROWS);
	lrcol = MIN(ulcol + cols, DCOLS);

	// We will loop until all rows in cca are displayed
	ar = 0;
	do while (ar < rows) {

		// write the next screenful of data.  Reserve the bottom
		// row for "Press Space" messages.
		//
		ar0 = ar;		// save where our screenful begins
		for (r=ulrow; r<=lrrow-1 && ; r++) {
			for (c=ulcol, ac=0; c<=lrcol; c++, ac++) {
				cc = mvincch(r, c);
				addcch(cca[ar][ac]);
				cca[ar][ac] = cc;
			}
			ar++;
		}
}
*/



/* ================== PRIVATE FUNCTIONS ================== */

/* Translates a numeric keypad press into the equivalent Rogue command.
 */
static char translate_keypad(int scancode) {
    const char keys[] = "yku-h5l+bjn0.";
    const char control[] = "\031\013\025-\0105\014+\002\012\0160.";
	regs_t regs;

    regs.h.ah = 0x02; /* get shift flags */
    intr_fn(0x16, &regs);

    /* return direction or ctrl-direction */
    if(((regs.h.al & 0x03) && !(regs.h.al & 0x10)) ||
            (!(regs.h.al & 0x03) && (regs.h.al & 0x10)))
        return control[scancode-0x47];  /* shift XOR scroll lock */
    else
        return keys[scancode-0x47];
}



/*   Writes a character to the string in B&W (put_char_at) or in color
 *   (put_color_char_at)
 */

#if 0
static put_char_at(register int row, register int col, char ch)
{
	put_color_char_at(row, col, (COLORBUF) MAKE_COLOR_CHAR(WHITE, BLACK, ch));
}
#endif

static put_color_char_at(register int row, register int col, COLORBUF cb)
{
	regs_t regs;

	put_cursor(row, col);

    regs.h.ah = 0x09;
//    regs.h.al = (ch & ~ST_MASK);
	regs.h.al = cb.b8.ch;
    regs.h.bh = videopage;
//    regs.h.bl = (ch & ST_MASK) ? 0x0f : 0x07;
    regs.h.bl = cb.b8.color;
    regs.x.cx = 1;
    intr_fn(0x10, &regs);

	terminal[row][col].b16 = cb.b16;
}


/*   Moves the cursor to a coordinate on the screen.
 */
static put_cursor(register int row, register int col)
{
	regs_t regs;

    regs.h.ah = 0x02;
    regs.h.dh = row;
    regs.h.dl = col;
    regs.h.bh = videopage;
    intr_fn(0x10, &regs);
}



/*  Clears the internal screen buffers
 */
static clear_buffers()
{
	register i, j;

	screen_dirty = 0;

	for (i = 0; i < DROWS; i++) {
		lines_dirty[i] = 0;
		for (j = 0; j < DCOLS; j++) {
			terminal[i][j].b16 = BLANKCH;
			buffer[i][j].b16 = BLANKCH;
		}
	}
}



/* Clears the physical screen
 */

static cls()
{
	regs_t regs;

    regs.h.ah = 0x00;
    regs.h.al = videomode;
    intr_fn(0x10, &regs);
}



/*  Redraws the complete game screen.
 */

static wrefresh(WINDOW *scr)
{
	short i, col;

    cls();
	cur_row = cur_col = 0;

	for (i = 0; i < DROWS; i++) {
		col = 0;
		while (col < DCOLS) {
			while ((col < DCOLS) && (buffer[i][col].b16 == BLANKCH)) {
				col++;
			}
			if (col < DCOLS) {
				put_cursor(i, col);
			}
			while ((col < DCOLS) && (buffer[i][col].b16 != BLANKCH)) {
				put_color_char_at(i, col, buffer[i][col]);
				col++;
			}
		}
	}
	put_cursor(curscr->_cury, curscr->_curx);
	fflush(stdout);
	scr = scr;		/* make lint happy */
}
