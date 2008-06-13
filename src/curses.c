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
 * 3. Neither the name of the University nor the names of its contributors
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

/*
 * ML, 31 Jan 2004: added support for win32 console applications
 *
 * NS, June 2003: Color Curses
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

#ifdef __WATCOMC__

#include <dos.h>
typedef union REGPACK regs_t;
#define	intr_fn(N,R) intr(N,R)

#elif _MSC_VER /* Microsoft C (tested with VC98) */

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#elif __DJGPP__

#include <dpmi.h>
typedef __dpmi_regs regs_t;
#define	intr_fn(N,R) __dpmi_int(N,R)

#else
#error curses.c only compiles with Open Watcom, Microsoft C, or DJGPP
#endif

/* ============== CONSTANTS AND TYPEDEFS =============== */

struct _win_st {
	short _cury, _curx;
	short _maxy, _maxx;
};
typedef struct _win_st WINDOW;


typedef union _colorChar    COLORBUF;
#define BLANKCH		    MAKE_COLOR_CHAR(WHITE,BLACK,' ')



/* ================== GLOBAL VARIABLES ================== */

int LINES=DROWS, COLS=DCOLS;

/* static char terminal[DROWS][DCOLS]; */
/* static char buffer[DROWS][DCOLS]; */
static COLORBUF terminal[DROWS][DCOLS];
static COLORBUF buffer[DROWS][DCOLS];

static boolean screen_dirty;
static boolean lines_dirty[DROWS];
static boolean buf_stand_out = 0;

static WINDOW scr_buf;
static WINDOW *curscr = &scr_buf;
static short cur_row, cur_col;

#ifdef _MSC_VER
static HANDLE hStdIn, hStdOut;
#else
static unsigned char videomode, videopage;
#endif

/* ================= PRIVATE PROTOTYPES ================= */

static void addstr_in_color(char *str, const byte color);	/* only used within this file */
static void addcstr(color_char *cstr);		/* only used within this file */
static char translate_keypad(int scancode);	/* only used within this file */
/* static void put_char_at(int row, int col, char ch)	only used within this file*/
static void put_color_char_at(int row, int col, COLORBUF cb);	/* only used within this file */
static void put_cursor(int row, int col);	/* only used within this file */
static void clear_buffers(void);			/* only used within this file */
static void cls(void);						/* only uwed within this file */
static void wrefresh(WINDOW *scr);			/* only used within this file */


/* ================== PUBLIC FUNCTIONS ================== */

/*  Returns a keystroke, translated into a Rogue command.
 */

char rgetchar(void)
{
    char ch;
#ifdef _MSC_VER
    INPUT_RECORD ir;
    DWORD n;
    
    put_cursor(curscr->_cury, curscr->_curx);
#else
    int scancode;
    regs_t regs;
#endif

    for(;;) {
#ifdef _MSC_VER
	if(!ReadConsoleInput(hStdIn, &ir, 1, &n) ||
		ir.EventType != KEY_EVENT || !ir.Event.KeyEvent.bKeyDown)
	    continue;

	/* convert keypad input to rogue commands */	
	switch(ir.Event.KeyEvent.wVirtualKeyCode) {
	case VK_NUMPAD9: case VK_PRIOR:
	    ir.Event.KeyEvent.uChar.AsciiChar = 'u'; break;
	case VK_NUMPAD3: case VK_NEXT:
	    ir.Event.KeyEvent.uChar.AsciiChar = 'n'; break;
	case VK_NUMPAD1: case VK_END:
	    ir.Event.KeyEvent.uChar.AsciiChar = 'b'; break;
	case VK_NUMPAD7: case VK_HOME:
	    ir.Event.KeyEvent.uChar.AsciiChar = 'y'; break;
	case VK_NUMPAD4: case VK_LEFT:
	    ir.Event.KeyEvent.uChar.AsciiChar = 'h'; break;
	case VK_NUMPAD8: case VK_UP:
	    ir.Event.KeyEvent.uChar.AsciiChar = 'k'; break;
	case VK_NUMPAD6: case VK_RIGHT:
	    ir.Event.KeyEvent.uChar.AsciiChar = 'l'; break;
	case VK_NUMPAD2: case VK_DOWN:
	    ir.Event.KeyEvent.uChar.AsciiChar = 'j'; break;
	case VK_DELETE:
	    ir.Event.KeyEvent.uChar.AsciiChar = '.'; break;
	}

	if(!ir.Event.KeyEvent.uChar.AsciiChar)
	    continue;

	/* apply shift or control to keypad movement keys */	
	if((ir.Event.KeyEvent.wVirtualKeyCode>=VK_PRIOR &&
		ir.Event.KeyEvent.wVirtualKeyCode<=VK_DOWN) || 
		(ir.Event.KeyEvent.wVirtualKeyCode>=VK_NUMPAD1 &&
		ir.Event.KeyEvent.wVirtualKeyCode<=VK_NUMPAD9)) {	    
	    if(ir.Event.KeyEvent.dwControlKeyState & SHIFT_PRESSED)
		ir.Event.KeyEvent.uChar.AsciiChar -= ('a' - 'A');
	    else if(ir.Event.KeyEvent.dwControlKeyState & (LEFT_CTRL_PRESSED |
		    RIGHT_CTRL_PRESSED))
		ir.Event.KeyEvent.uChar.AsciiChar -= ('a' - 1);
	
	    /* Check for fast play mode, but only for the keypad. The current
	     * method of converting y to ^y can mess up answers to y/n questions. */
	    if(ir.Event.KeyEvent.dwControlKeyState & SCROLLLOCK_ON) {
		if(strchr("hjklyubn", ir.Event.KeyEvent.uChar.AsciiChar))
		    ir.Event.KeyEvent.uChar.AsciiChar -= ('a' - 1);
		else if(strchr("HJKLYUBN", ir.Event.KeyEvent.uChar.AsciiChar))
		    ir.Event.KeyEvent.uChar.AsciiChar += ('a' - 'A');
		else if(strchr("\010\012\013\014\031\025\002\016",
		    ir.Event.KeyEvent.uChar.AsciiChar))
		    ir.Event.KeyEvent.uChar.AsciiChar += ('a' - 1);
	    }
	}

	ch = ir.Event.KeyEvent.uChar.AsciiChar;
#else
	regs.h.ah = 0x00; /* get keystroke */
	intr_fn(0x16, &regs);
	scancode = regs.h.ah; /* BIOS scancode */
	
	if(scancode>0x46 && scancode<0x54)
	    ch = translate_keypad(scancode);
	else
	    ch = regs.h.al; /* ASCII code */
#endif
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

void initscr(void)
{
#ifdef _MSC_VER
    CONSOLE_SCREEN_BUFFER_INFO csbi;

    hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    hStdIn = GetStdHandle(STD_INPUT_HANDLE);

    GetConsoleScreenBufferInfo(hStdOut, &csbi);
    COLS = csbi.dwSize.X;
    LINES = csbi.dwSize.Y;

    SetConsoleTitle("Rogue Clone IV");
    SetConsoleMode(hStdIn, 0);
#else
    regs_t regs;

    regs.h.ah = 0x0F;
    intr_fn(0x10, &regs);
    videomode = regs.h.al;
    videopage = regs.h.bh;
    COLS = regs.h.ah;
#endif
}


void move(const short row, const short col)
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

void addstr(char *str)
{
	while (*str) {
		addch((int) *str++);
	}
}


void mvaddstr(const short row, const short col, char *str)
{
	move(row, col);
	addstr(str);
}


static void addstr_in_color(char *str, const byte color)	/* only used within this file */
{
	color_char cc;
	while (*str) {
		cc.b8.color = color;
		cc.b8.ch = *str;
		addcch(cc);
		str++;
	}
}


void mvaddstr_in_color(const short row, const short col, char *str, const byte color)
{
	move(row, col);
	addstr_in_color(str, color);
}


static void addcstr(color_char *cstr)	/* only used within this file */
{
	while ((*cstr).b16 != 0) {
		addcch(*cstr++);
	}
}


void mvaddcstr(const short row, const short col, color_char *cstr)
{
	move(row, col);
	addcstr(cstr);
}



/* Adds a character to the screen at the current cursor position
 * (addch, addcch) or at the specified position (mvaddch, mvaddcch), in
 * B&W (addch, mvaddch) or color (addcch, mvaddcch).
 */

void addch(const int ch)
{
	color_char cc;

	cc.b8.ch = (char) ch;
	if (buf_stand_out) {
		cc.b8.color = MAKE_COLOR(BRIGHT_WHITE, BLACK);
	} else {
		cc.b8.color = MAKE_COLOR(WHITE, BLACK);
	}

	addcch(cc);
}


void mvaddch(const short row, const short col, const int ch)
{
	move(row, col);
	addch(ch);
}


void addcch(const color_char cc)
{
	short row, col;

	row = curscr->_cury;
	col = curscr->_curx++;

	buffer[row][col].b16 = cc.b16;
	lines_dirty[row] = 1;
	screen_dirty = 1;
}


void mvaddcch(const short row, const short col, const color_char cc)
{
	move(row, col);
	addcch(cc);
}



/* Turns a dull B&W string into a vibrant color one. :-)
 */

void colorize(char *str, const byte color, color_char *cstr)
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

void refresh(void)
{
	int i, j, line;
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


void redraw(void) 
{
	wrefresh(curscr);
}



/* Returns the character currently at the specified current position
 */
int mvinch(const short row, const short col)
{
	move(row, col);
	return((int) (buffer[row][col].b8.ch));
}


color_char mvincch(const short row, const short col)
{
	move(row, col);
	return (buffer[row][col]);
}



/* Clears the whole screan (clear) or to the current EOL (clrtoeol)
 */

void clear(void)
{
	cls();
	fflush(stdout);
	cur_row = cur_col = 0;
	move(0, 0);
	clear_buffers();
}


void clrtoeol(void)
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

void standout(void)
{
	buf_stand_out = 1;
}

void standend(void)
{
	buf_stand_out = 0;
}



/*  NS 14 June 2003:  Draws a box using the given character set.
 *	Specify size as the upper-left corner, the height, and the width.
 *  Specify cset in order: horiz, vert, and 4 corners: ul, ur, ll, lr
 */

void draw_box(const color_char cset[6], const short ulrow, const short ulcol, const short height, const short width)
{
	short i;
	const short toprow = ulrow;
	const short bottomrow = ulrow + height - 1;
	const short leftcol = ulcol;
	const short rightcol = ulcol + width - 1;

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

void endwin(void)
{
}


void crmode(void)
{
}


void noecho(void)
{
}


void nonl(void)
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
#if 0
void overlay(color_char cca[][], int rows, int cols, int ulrow, int ulcol)
{
	int r, c, ar, ar0, ac;
	int lrrow, lrcol;
	color_char cc;

	/* compute window dimensions and override any insanity */
	if (rows < 1  ||  cols < 1)
		return 0;
	ulrow = MAX(MIN_ROW, ulrow);
	ulcol = MAX(1, ulcol);
	lrrow = MIN(ulrow + rows, DROWS);
	lrcol = MIN(ulcol + cols, DCOLS);

	/* We will loop until all rows in cca are displayed */
	ar = 0;
	do while (ar < rows) {

		/* write the next screenful of data.  Reserve the bottom
		   row for "Press Space" messages. */		
		ar0 = ar;		/* save where our screenful begins */
		for (r=ulrow; r<=lrrow-1 && ; r++) {
			for (c=ulcol, ac=0; c<=lrcol; c++, ac++) {
				cc = mvincch(r, c);
				addcch(cca[ar][ac]);
				cca[ar][ac] = cc;
			}
			ar++;
		}
}
#endif



/* ================== PRIVATE FUNCTIONS ================== */

/* Translates a numeric keypad press into the equivalent Rogue command.
 */
#ifndef _MSC_VER
static char translate_keypad(int scancode)	/* only used within this file */
{
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
#endif


/*   Writes a character to the string in B&W (put_char_at) or in color
 *   (put_color_char_at)
 */

#if 0
static void put_char_at(int row, int col, char ch)	/* only used within this file*/
{
	put_color_char_at(row, col, (COLORBUF) MAKE_COLOR_CHAR(WHITE, BLACK, ch));
}
#endif


static void put_color_char_at(int row, int col, COLORBUF cb)	/* only used within this file */
{
#ifdef _MSC_VER
    COORD coord;
    WORD attrib;
    DWORD n;
#else
    regs_t regs;
    
    put_cursor(row, col);
#endif
    
#ifdef _MSC_VER
    coord.X = (short) col;
    coord.Y = (short) row;
    attrib = cb.b8.color;

    WriteConsoleOutputCharacter(hStdOut, &cb.b8.ch, 1, coord, &n);
    WriteConsoleOutputAttribute(hStdOut, &attrib, 1, coord, &n);
#else
    regs.h.ah = 0x09;
    /* regs.h.al = (ch & ~ST_MASK); */
    regs.h.al = cb.b8.ch;
    regs.h.bh = videopage;
    /* regs.h.bl = (ch & ST_MASK) ? 0x0f : 0x07; */
    regs.h.bl = cb.b8.color;
    regs.x.cx = 1;
    intr_fn(0x10, &regs);
#endif
    
    terminal[row][col].b16 = cb.b16;
}


/*   Moves the cursor to a coordinate on the screen.
 */

static void put_cursor(int row, int col)	/* only used within this file */
{
#ifdef _MSC_VER
    COORD coord;

    coord.X = (short) col;
    coord.Y = (short) row;

    SetConsoleCursorPosition(hStdOut, coord);
#else
    regs_t regs;

    regs.h.ah = 0x02;
    regs.h.dh = row;
    regs.h.dl = col;
    regs.h.bh = videopage;
    intr_fn(0x10, &regs);
#endif
}



/*  Clears the internal screen buffers
 */

static void clear_buffers(void)		/* only used within this file */
{
	int i, j;

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

static void cls(void)		/* only uwed within this file */
{
#ifdef _MSC_VER
    COORD coord;
    DWORD n;

    coord.X = coord.Y = 0;
    FillConsoleOutputCharacter(hStdOut, ' ', LINES*COLS, coord, &n);
    FillConsoleOutputAttribute(hStdOut, WHITE, LINES*COLS, coord, &n);
#else
    regs_t regs;

    regs.h.ah = 0x00;
    regs.h.al = videomode;
    intr_fn(0x10, &regs);

    /* turn off the blinkin' blinking */
    regs.x.ax = 0x1003;
    regs.x.bx = 0;
    intr_fn(0x10, &regs);
#endif
}



/*  Redraws the complete game screen.
 */

static void wrefresh(WINDOW *scr)	/* only used within this file */
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
#ifndef _MSC_VER
			if (col < DCOLS) {
				put_cursor(i, col);
			}
#endif
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
