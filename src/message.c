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
 * @(#)message.c 8.1 (Berkeley) 5/31/93
 */

/*
 * message.c
 *
 * This source herein may be modified and/or distributed by anybody who
 * so desires, with the following restrictions:
 *    1.)  No portion of this notice shall be removed.
 *    2.)  Credit shall not be taken for the creation of this source.
 *    3.)  This code is not to be traded, sold, or used for personal
 *         gain or profit.
 *
 */

#include <stdio.h>
#include "rogue.h"
#include "paths.h"

char msgs[NMESSAGES][DCOLS] = {"", "", "", "", ""};
short msg_col = 0, imsg = -1;
boolean msg_cleared = 1, rmsg = 0;
char hunger_str[8] = "";
char *more = "-more-";

extern boolean cant_int, did_int, interrupted, save_is_interactive;
extern boolean use_doschars, use_color;
extern short add_strength;
extern short cur_level;

message(msg, intrpt)
char *msg;
boolean intrpt;
{
	cant_int = 1;

	if (!save_is_interactive) {
		return;
	}
	if (intrpt) {
		interrupted = 1;
		md_slurp();
	}

	if (!msg_cleared) {
		mvaddstr(MIN_ROW-1, msg_col, more);
		refresh();
		wait_for_ack();
		check_message();
	}
	if (!rmsg) {
		imsg = (imsg + 1) % NMESSAGES;
		(void) strcpy(msgs[imsg], msg);
	}
	mvaddstr(MIN_ROW-1, 0, msg);
	addch(' ');
	refresh();
	msg_cleared = 0;
	msg_col = strlen(msg);

	cant_int = 0;

	if (did_int) {
		did_int = 0;
		onintr();
	}
}

remessage(c)
short c;
{
	if (imsg != -1) {
		check_message();
		rmsg = 1;
		while (c > imsg) {
			c -= NMESSAGES;
		}
		message(msgs[((imsg - c) % NMESSAGES)], 0);
		rmsg = 0;
		move(rogue.row, rogue.col);
		refresh();
	}
}

check_message()
{
	if (msg_cleared) {
		return;
	}
	move(MIN_ROW-1, 0);
	clrtoeol();
	refresh();
	msg_cleared = 1;
}

get_input_line(prompt, insert, buf, if_cancelled, add_blank, do_echo)
char *prompt, *buf, *insert;
char *if_cancelled;
boolean add_blank;
boolean do_echo;
{
	short ch;
	short i = 0, n;

	message(prompt, 0);
	n = strlen(prompt);

	if (insert[0]) {
		mvaddstr(0, n + 1, insert);
		(void) strcpy(buf, insert);
		i = strlen(insert);
		move(0, (n + i + 1));
		refresh();
	}

	while (((ch = rgetchar()) != '\r') && (ch != '\n') && (ch != CANCEL)) {
		if ((ch >= ' ') && (ch <= '~') && (i < MAX_TITLE_LENGTH-2)) {
			if ((ch != ' ') || (i > 0)) {
				buf[i++] = ch;
				if (do_echo) {
					addch(ch);
				}
			}
		}
		if ((ch == '\b') && (i > 0)) {
			if (do_echo) {
				mvaddch(0, i + n, ' ');
				move(MIN_ROW-1, i+n);
			}
			i--;
		}
		refresh();
	}
	check_message();
	if (add_blank) {
		buf[i++] = ' ';
	} else {
		while ((i > 0) && (buf[i-1] == ' ')) {
			i--;
		}
	}

	buf[i] = 0;

	if ((ch == CANCEL) || (i == 0) || ((i == 1) && add_blank)) {
		if (if_cancelled) {
			message(if_cancelled, 0);
		}
		return(0);
	}
	return(i);
}

/*
WAS:
Level: 99 Gold: 999999 Hp: 999(999) Str: 99(99) Arm: 99 Exp: 21/10000000 Hungry

NOW:
Level:99  Gold:999999  Hp:999(999)  Str:99(99)  Arm:99  Exp:21/10000000 -Hungry-
0    5    1    5    2    5    3    5    4    5    5    5    6    5    7    5
*/

/* NS: Color-coded the stats so that dense players get a visual warning when
 *     in trouble.  Apologies to those who dislike it, but I got tired of
 *	   fainting in the middle of a troll-battle because I forgot to eat.
 */
print_stats(stat_mask)
register stat_mask;
{
	char buf[16];
	boolean label;
	int row = DROWS - 1;
	byte color, label_color, ok_color, warn_color, danger_color;
	int tmp;

	label = (stat_mask & STAT_LABEL) ? 1 : 0;

	if (use_color) {
		label_color = STAT_LABELCOLOR;
		ok_color = STAT_OKCOLOR;
		warn_color = STAT_WARNCOLOR;
		danger_color = STAT_DANGERCOLOR;
	} else {
		label_color = MAKE_COLOR(WHITE,BLACK);
		ok_color = MAKE_COLOR(WHITE,BLACK);
		warn_color = MAKE_COLOR(WHITE,BLACK);
		danger_color = MAKE_COLOR(BRIGHT_WHITE,BLACK);
	}

	if (stat_mask & STAT_LEVEL) {
		if (label) {
			mvaddstr_in_color(row, 0, "Level:", label_color);
		}
		/* max level taken care of in make_level() */
		sprintf(buf, "%d", cur_level);
		mvaddstr_in_color(row, 6, buf, ok_color);
		pad(buf, 2);
	}
	if (stat_mask & STAT_GOLD) {
		if (label) {
			mvaddstr_in_color(row, 10, "Gold:", label_color);
		}
		if (rogue.gold > MAX_GOLD) {
			rogue.gold = MAX_GOLD;
		}
		sprintf(buf, "%ld", rogue.gold);
		mvaddstr_in_color(row, 15, buf, ok_color);
		pad(buf, 6);
	}
	if (stat_mask & STAT_HP) {
		if (label) {
			mvaddstr_in_color(row, 23, "Hp:", label_color);
		}
		if (rogue.hp_max > MAX_HP) {
			rogue.hp_current -= (rogue.hp_max - MAX_HP);
			rogue.hp_max = MAX_HP;
		}

		/* NS:  Warn if HP < half; shout if they're < 20% */
		sprintf(buf, "%d", rogue.hp_current);
		color = (rogue.hp_current <= rogue.hp_max / 5) ? danger_color :
		        (rogue.hp_current <= rogue.hp_max / 2) ? warn_color :
		        ok_color;
		mvaddstr_in_color(row, 26, buf, color);
		tmp = strlen(buf);
		sprintf(buf, "(%d)", rogue.hp_max);
		addstr_in_color(buf, label_color);
		pad(buf, 8-tmp);
	}
	if (stat_mask & STAT_STRENGTH) {
		if (label) {
			mvaddstr_in_color(row, 36, "Str: ", label_color);
		}
		if (rogue.str_max > MAX_STRENGTH) {
			rogue.str_current -= (rogue.str_max - MAX_STRENGTH);
			rogue.str_max = MAX_STRENGTH;
		}

		/* NS:  Warn if strength <= 14; shout if <= 6 */
		tmp = rogue.str_current + add_strength;
		sprintf(buf, "%d", tmp);
		color = (tmp <= 6) ? danger_color :
		        (tmp <= 14) ? warn_color : ok_color;
		mvaddstr_in_color(row, 40, buf, color);
		tmp = strlen(buf);
		sprintf(buf, "(%d)", rogue.str_max);
		addstr_in_color(buf, label_color);
		pad(buf, 6-tmp);
	}
	if (stat_mask & STAT_ARMOR) {
		if (label) {
			mvaddstr_in_color(row, 48, "Arm: ", label_color);
		}
		if (rogue.armor && (rogue.armor->d_enchant > MAX_ARMOR)) {
			rogue.armor->d_enchant = MAX_ARMOR;
		}

		/* NS: Warn if armor = 1 or 2, shout if <= 0 (naked) */
		tmp = get_armor_class(rogue.armor);
		sprintf(buf, "%d", tmp);
		color = (tmp <= 0) ? danger_color :
		        (tmp <= 2) ? warn_color : ok_color;
		mvaddstr_in_color(row, 52, buf, color);
		pad(buf, 2);
	}
	if (stat_mask & STAT_EXP) {
		if (label) {
			mvaddstr_in_color(row, 56, "Exp: ", label_color);
		}
		if (rogue.exp_points > MAX_EXP) {
			rogue.exp_points = MAX_EXP;
		}
		if (rogue.exp > MAX_EXP_LEVEL) {
			rogue.exp = MAX_EXP_LEVEL;
		}
		sprintf(buf, "%d/%ld", rogue.exp, rogue.exp_points);
		mvaddstr_in_color(row, 60, buf, ok_color);
		pad(buf, 11);
	}
	if (stat_mask & STAT_HUNGER) {
		/* Invert if hungry, warn if weak, shout if faint.  Note that
		 * if we're playing in color we use a bright white background
		 * when hungry.
		 */
		move(row, 72);
		switch (hunger_str[0]) {
			case 'h':
				color = (use_color) ? MAKE_COLOR(BLACK,WHITE) :
						INVERT_COLOR(ok_color);
				break;
			case 'w':
				color = (use_color) ? INVERT_COLOR(warn_color) :
						MAKE_COLOR(BLACK,BRIGHT_WHITE);
				break;
			case 'f':
				color = INVERT_COLOR(danger_color);
				break;
			default:
				color = 0;
		}
		addstr_in_color(" ", color);
		addstr_in_color(hunger_str, color);
		addstr_in_color(" ", color);
		clrtoeol();
	}
	refresh();
}

pad(s, n)
char *s;
short n;
{
	short i;

	for (i = strlen(s); i < n; i++) {
		addch(' ');
	}
}


/*  NS: We convert to ASCII characters before saving, if necessary.
 */
save_screen()
{
	FILE *fp;
	short i, j;
	char buf[DCOLS+2];
	boolean found_non_blank;
	boolean tmp_doschars;

	tmp_doschars = use_doschars;
	if (tmp_doschars) {
		use_doschars = 0;
		regenerate_screen();
	}

	if ((fp = fopen(_PATH_SCREENFILE, "w")) != NULL) {
		for (i = 0; i < DROWS; i++) {
			found_non_blank = 0;
			for (j = (DCOLS - 1); j >= 0; j--) {
				buf[j] = mvinch(i, j);
				if (!found_non_blank) {
					if ((buf[j] != ' ') || (j == 0)) {
						buf[j + ((j == 0) ? 0 : 1)] = 0;
						found_non_blank = 1;
					}
				}
			}
			fputs(buf, fp);
			putc('\n', fp);
		}
		fclose(fp);
	} else {
		sound_bell();
	}

	if (tmp_doschars) {
		use_doschars = 1;
		regenerate_screen();
	}
}

sound_bell()
{
	putchar(7);
	fflush(stdout);
}

boolean
is_digit(ch)
short ch;
{
	return((ch >= '0') && (ch <= '9'));
}

r_index(str, ch, last)
char *str;
int ch;
boolean last;
{
	int i = 0;

	if (last) {
		for (i = strlen(str) - 1; i >= 0; i--) {
			if (str[i] == ch) {
				return(i);
			}
		}
	} else {
		for (i = 0; str[i]; i++) {
			if (str[i] == ch) {
				return(i);
			}
		}
	}
	return(-1);
}
