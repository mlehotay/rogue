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
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
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

#include <dos.h>
#include <stdio.h>
#include "rogue.h"

#define ST_MASK 0x80

struct _win_st {
	short _cury, _curx;
	short _maxy, _maxx;
};

typedef struct _win_st WINDOW;

char terminal[DROWS][DCOLS];
char buffer[DROWS][DCOLS];

boolean screen_dirty;
boolean lines_dirty[DROWS];
boolean buf_stand_out = 0;

int LINES=DROWS, COLS=DCOLS;
WINDOW scr_buf;
WINDOW *curscr = &scr_buf;

short cur_row, cur_col;
unsigned char videomode, videopage;

char translate_keypad(int scancode) {
    const char keys[] = "yku-h5l+bjn0.";
    const char control[] = "\031\013\025-\0105\014+\002\012\0160.";
    union REGPACK regs;    

    regs.h.ah = 0x02; /* get shift flags */
    intr(0x16, &regs);

    /* return direction or ctrl-direction */
    if(((regs.h.al & 0x03) && !(regs.h.al & 0x10)) ||
            (!(regs.h.al & 0x03) && (regs.h.al & 0x10)))
        return control[scancode-0x47];  /* shift XOR scroll lock */
    else
        return keys[scancode-0x47];
}

rgetchar()
{
	register ch;
    register scancode;
    union REGPACK regs;

	for(;;) {
        regs.h.ah = 0x00; /* get keystroke */
        intr(0x16, &regs);
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

initscr()
{
    union REGPACK regs;

    regs.h.ah = 0x0F;
    intr(0x10, &regs);
    videomode = regs.h.al;
    videopage = regs.h.bh;
    COLS = regs.h.ah;
}

endwin()
{
}

move(row, col)
short row, col;
{
	curscr->_cury = row;
	curscr->_curx = col;
	screen_dirty = 1;
}

mvaddstr(row, col, str)
short row, col;
char *str;
{
	move(row, col);
	addstr(str);
}

addstr(str)
char *str;
{
	while (*str) {
		addch((int) *str++);
	}
}

addch(ch)
register int ch;
{
	short row, col;

	row = curscr->_cury;
	col = curscr->_curx++;

	if (buf_stand_out) {
		ch |= ST_MASK;
	} else {
		ch &= ~ST_MASK;
	}

	buffer[row][col] = (char) ch;
	lines_dirty[row] = 1;
	screen_dirty = 1;
}

mvaddch(row, col, ch)
short row, col;
int ch;
{
	move(row, col);
	addch(ch);
}

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
					if (buffer[line][j] != terminal[line][j]) {
						put_char_at(line, j, buffer[line][j]);
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

wrefresh(scr)
WINDOW *scr;
{
	short i, col;

    cls();
	cur_row = cur_col = 0;

	for (i = 0; i < DROWS; i++) {
		col = 0;
		while (col < DCOLS) {
			while ((col < DCOLS) && (buffer[i][col] == ' ')) {
				col++;
			}
			if (col < DCOLS) {
				put_cursor(i, col);
			}
			while ((col < DCOLS) && (buffer[i][col] != ' ')) {
				put_char_at(i, col, buffer[i][col]);
				col++;
			}
		}
	}
	put_cursor(curscr->_cury, curscr->_curx);
	fflush(stdout);
	scr = scr;		/* make lint happy */
}

redraw() {
	wrefresh(curscr);
}

mvinch(row, col)
short row, col;
{
	move(row, col);
	return((int) (buffer[row][col] & ~ST_MASK));
}

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
		buffer[row][col] = ' ';
	}
	lines_dirty[row] = 1;
}

standout()
{
	buf_stand_out = 1;
}

standend()
{
	buf_stand_out = 0;
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

clear_buffers()
{
	register i, j;

	screen_dirty = 0;

	for (i = 0; i < DROWS; i++) {
		lines_dirty[i] = 0;
		for (j = 0; j < DCOLS; j++) {
			terminal[i][j] = ' ';
			buffer[i][j] = ' ';
		}
	}
}

cls()
{
    union REGPACK regs;

    regs.h.ah = 0x00;
    regs.h.al = videomode;
    intr(0x10, &regs);
}

put_char_at(row, col, ch)
register row, col, ch;
{
    union REGPACK regs;
    
	put_cursor(row, col);

    regs.h.ah = 0x09;
    regs.h.al = (ch & ~ST_MASK);
    regs.h.bh = videopage;
    regs.h.bl = (ch & ST_MASK) ? 0x0f : 0x07;
    regs.x.cx = 1;
    intr(0x10, &regs);

	terminal[row][col] = (char) ch;
}

put_cursor(row, col)
register row, col;
{
    union REGPACK regs;

    regs.h.ah = 0x02;
    regs.h.dh = row;
    regs.h.dl = col;
    regs.h.bh = videopage;
    intr(0x10, &regs);
}
