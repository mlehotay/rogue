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
 * @(#)room.c 8.1 (Berkeley) 5/31/93
 */

/*
 * room.c
 *
 * This source herein may be modified and/or distributed by anybody who
 * so desires, with the following restrictions:
 *    1.)  No portion of this notice shall be removed.
 *    2.)  Credit shall not be taken for the creation of this source.
 *    3.)  This code is not to be traded, sold, or used for personal
 *         gain or profit.
 *
 */

#include "rogue.h"

room rooms[MAXROOMS];
boolean rooms_visited[MAXROOMS];

extern short blind;
extern short halluc;
extern boolean detect_monster, jump, passgo, display_skull;
extern boolean use_doschars, use_color;
extern char *login_name, *fruit, *press_space;

/* don't modify use_colors until entire screen is redrawn, otherwise
 * some lines are displayed in color and others in black & white */
static boolean color_kludge;

#define NOPTS 7

struct option {
	char *prompt;
	boolean is_bool;
	char **strval;
	boolean *bval;
	boolean add_blank;
	boolean redraw_necc;
} options[NOPTS] = {
	{
		"Show position only at end of run (\"jump\"): ",
		1, (char **) 0, &jump, 0, 0
	},
	{
		"Follow turnings in passageways (\"passgo\"): ",
		1, (char **) 0, &passgo, 0, 0
	},
	{
		"Print skull when killed (\"skull\" or \"tomb\"): ",
		1, (char **) 0, &display_skull, 0, 0
	},
	{
		"Play in color (\"color\"): ",
		1, (char **) 0, &color_kludge, 0, 1
	},
	{
		"Use extended ASCII graphics (\"PCgraphics\"): ",
		1, (char **) 0, &use_doschars, 0, 1
	},
	{
		"Name (\"name\"): ",
		0, &login_name, (boolean *) 0, 0, 0
	},
	{
		"Fruit (\"fruit\"): ",
		0, &fruit, (boolean *) 0, 0, 0
	}
};


/*   NS, 8 June 2003: All drawing properties are in this table.  Also, now
 *   uses the PC-Rogue characters for objects and dungeon terrain.
 */
struct _screenchars {
	unsigned short mask;		/* as defined in rogue.h */
	char ch;					/* classic screen character */
	char dosch;					/* DOS screen character */
	unsigned char fgcolor;		/* foreground color */
	unsigned char bgcolor;		/* background color */
	boolean canBeMimicked;		/* Xeroc disguise or hallucination char? */
};

struct _screenchars scrObjects[OBJECTTYPES] = {
	{ ARMOR,   ']', '\x08',	BRIGHT_BLUE,   BLACK, 1 },
	{ WEAPON,  ')', '\x18', BRIGHT_BLUE,   BLACK, 1 },
	{ SCROL,   '?', '\x0D',	BRIGHT_BLUE,   BLACK, 1 },
	{ POTION,  '!', '\xAD', BRIGHT_BLUE,   BLACK, 1 },
	{ GOLD,    '*', '\x0F', BRIGHT_YELLOW, BLACK, 1 },
	{ FOOD,    ':', '\x05', RED,           BLACK, 1 },
	{ WAND,    '/', '\xE7', BRIGHT_BLUE,   BLACK, 1 },
	{ RING,    '=', '\x09', BRIGHT_BLUE,   BLACK, 1 },
	{ AMULET,  ',', '\x0C', BRIGHT_BLUE,   BLACK, 0 }
};

struct _screenchars scrTerrain[TERRAINTYPES] = {
	{ NOTHING, ' ', ' ', 	WHITE,  		BLACK,        0 },
	{ OBJECT,  '~', '~', 	WHITE,   		BLACK,        0 },	/* error */
	{ MONSTER, 'm', 'm',	WHITE,   		BLACK,        0 },	/* error */
	{ STAIRS,  '%', '\xF0', BLACK,  		BRIGHT_GREEN, 1 },
	{ HORWALL, '-', '\xCD', YELLOW, 		BLACK,        0 },
	{ VERTWALL,'|', '\xBA', YELLOW, 		BLACK,        0 },
	{ ULCORNER,'-', '\xC9', YELLOW, 		BLACK,        0 },
	{ URCORNER,'-', '\xBB', YELLOW, 		BLACK,        0 },
	{ LLCORNER,'-', '\xC8', YELLOW, 		BLACK,        0 },
	{ LRCORNER,'-', '\xBC', YELLOW, 		BLACK,        0 },
	{ DOOR,    '+', '\xCE', YELLOW,  		BLACK,        0 },
	{ FLOOR,   '.', '\xFA',	BRIGHT_GREEN,   BLACK,        0 },
	{ TUNNEL,  '#', '\xB1', WHITE,    		BLACK,        0 },
	{ TRAP,    '^', '\x04', MAGENTA, 		BLACK,        0 }
};


static struct _screenchars *disguise_chars[OBJECTTYPES + TERRAINTYPES];
static short dc_len = -1;


static void visit_rooms(const int rn);	/* only used within this file */
static int get_oth_room(const short rn, short *row, short *col);		/* only used within this file */
static void opt_show(const short i);		/* only used within this file */
static void opt_erase(const short i);		/* only used within this file */
static void opt_go(const short i);			/* only used within this file */
static void init_groc_array(void);	/* only used within this file */



/* ======================= FUNCTIONS ===================== */

void light_up_room(const int rn)
{
	short i, j;

	if (!blind) {
		for (i = rooms[rn].top_row;
			i <= rooms[rn].bottom_row; i++) {
			for (j = rooms[rn].left_col;
				j <= rooms[rn].right_col; j++) {
				if (dungeon[i][j] & MONSTER) {
					object *monster;

					if (monster = object_at(&level_monsters, i, j)) {
						dungeon[monster->row][monster->col] &= (~MONSTER);
						monster->trail_char =
							get_dungeon_char(monster->row, monster->col);
						dungeon[monster->row][monster->col] |= MONSTER;
					}
				}
				mvaddcch(i, j, get_dungeon_char(i, j));
			}
		}
		mvaddcch(rogue.row, rogue.col, get_rogue_char());

		/* NS - added this so that hallucinating characters don't see the
		 * actual room contents when they cross the threshold.
		 */
		if (halluc) {
			hallucinate();
		}
	}
}


void light_passage(const short row, const short col)
{
	short i, j, i_end, j_end;

	if (blind) {
		return;
	}
	i_end = (row < (DROWS-2)) ? 1 : 0;
	j_end = (col < (DCOLS-1)) ? 1 : 0;

	for (i = ((row > MIN_ROW) ? -1 : 0); i <= i_end; i++) {
		for (j = ((col > 0) ? -1 : 0); j <= j_end; j++) {
			if (can_move(row, col, (short) (row+i), (short) (col+j))) {
				mvaddcch((short) (row+i), (short) (col+j),
					get_dungeon_char((short) (row+i), (short) (col+j)));
			}
		}
	}
}


void darken_room(const short rn)
{
	short i, j;

	for (i = rooms[rn].top_row + 1; i < rooms[rn].bottom_row; i++) {
		for (j = rooms[rn].left_col + 1; j < rooms[rn].right_col; j++) {
			if (blind) {
				mvaddch(i, j, ' ');
			} else {
				if (!(dungeon[i][j] & (OBJECT | STAIRS)) &&
					!(detect_monster && (dungeon[i][j] & MONSTER))) {
					if (!imitating(i, j)) {
						mvaddch(i, j, ' ');
					}
					if ((dungeon[i][j] & TRAP) && (!(dungeon[i][j] & HIDDEN))) {
						mvaddcch(i, j, get_terrain_char(TRAP));
					}
				}
			}
		}
	}
}


color_char get_terrain_char(const unsigned short mask)
{
	color_char cc;
	int i;

	/* NS: this ought to be a hashtable lookup, but for now... */
	for (i=0; i<TERRAINTYPES; i++) {
		if (scrTerrain[i].mask == mask) {
			cc.b8.color = use_color ?
				MAKE_COLOR(scrTerrain[i].fgcolor, scrTerrain[i].bgcolor) :
				MAKE_COLOR(WHITE, BLACK);
			cc.b8.ch = use_doschars ? scrTerrain[i].dosch : scrTerrain[i].ch;

			return cc;
		}
	}

	/* error! */
	cc.b8.color = MAKE_COLOR(BRIGHT_WHITE, BLACK);
	cc.b8.ch = '~';
	return cc;
}



color_char get_dungeon_char(const short row, const short col)
{
	unsigned short mask = dungeon[row][col];
	color_char cc;

	/* monsters are at the top layer and overlay all else */
	if (mask & MONSTER) {
		return(gmc_row_col(row, col));
	}

	/* objects are the second layer */
	if (mask & OBJECT) {
		object *obj;

		obj = object_at(&level_objects, row, col);
		cc.b16 = get_mask_char(obj->what_is).b16;
		if ((mask & TUNNEL) && use_doschars)
		    cc.b8.color = MAKE_COLOR(FGCOLOR_OF(cc.b8.color),
					    use_color ? WHITE : GRAY);
		return cc;
	}

	/* terrain is third.  This too needs to become a hashtable. */
	if (mask & (TUNNEL | STAIRS | ANYROOMSIDE | FLOOR | DOOR)) {
		if ((mask & STAIRS) && !(mask & HIDDEN)) {
			return (get_terrain_char(STAIRS));
		}
		if ((mask & TUNNEL) && !(mask & HIDDEN)) {
			return (get_terrain_char(TUNNEL));
		}
		if (mask & HORWALL) {
			return(get_terrain_char(HORWALL));
		}
		if (mask & VERTWALL) {
			return(get_terrain_char(VERTWALL));
		}
		/* NS: Corner support added. */
		if (mask & LLCORNER) {
			return(get_terrain_char(LLCORNER));
		}
		if (mask & LRCORNER) {
			return(get_terrain_char(LRCORNER));
		}
		if (mask & ULCORNER) {
			return(get_terrain_char(ULCORNER));
		}
		if (mask & URCORNER) {
			return(get_terrain_char(URCORNER));
		}
		if (mask & FLOOR) {
			if (mask & TRAP) {
				if (!(mask & HIDDEN)) {
					return(get_terrain_char(TRAP));
				}
			}
			return(get_terrain_char(FLOOR));
		}
		if (mask & DOOR) {
			if (mask & HIDDEN) {
				if (((col > 0)
						&& (dungeon[row][col-1] & (HORWALL | ANYCORNER))) ||
					((col < (DCOLS-1))
						&& (dungeon[row][col+1] & (HORWALL | ANYCORNER)))) {
					return(get_terrain_char(HORWALL));
				} else {
					return(get_terrain_char(VERTWALL));
				}
			} else {
				return(get_terrain_char(DOOR));
			}
		}
	}

	return(get_terrain_char(NOTHING));
}



/* Returns the screen character of the specified item.
 * NS: This now returns a 16-bit color_char.
 */

color_char get_mask_char(const unsigned short mask)
{
	color_char cc;
	int i;

	/* this ought to be a hashtable lookup, but for now... */
	for (i=0; i<OBJECTTYPES; i++) {
		if (scrObjects[i].mask == mask) {
			cc.b8.color = use_color ?
				MAKE_COLOR(scrObjects[i].fgcolor, scrObjects[i].bgcolor) :
				MAKE_COLOR(WHITE, BLACK);
			cc.b8.ch = use_doschars ? scrObjects[i].dosch : scrObjects[i].ch;
			return cc;
		}
	}

	/* error! */
	cc.b8.color = MAKE_COLOR(BRIGHT_WHITE, BLACK);
	cc.b8.ch = '~';
	return cc;
}


/* Returns the rogue's character based on the current options settings.
 */

color_char get_rogue_char(void)
{
	color_char cc;

	cc.b8.color = (use_color) ? rogue.color : MAKE_COLOR(WHITE,BLACK);
	cc.b8.ch = (use_doschars) ? rogue.dosfchar : rogue.fchar;
	return cc;
}


/*  Clears and redraws the screen buffer.  Only nonblank characters are
 *	redrawn, otherwise this function would be a magic map / detect
 *  monsters / detect things combination platter.
 */

void regenerate_screen(void)
{
	short i, j;
	unsigned short mask;
	object *monster;

	for (i=0; i<DROWS; i++) {
		for (j=0; j<DCOLS; j++) {
			if (mvinch(i,j) != ' ') {
				mask = dungeon[i][j];

				/* set a monster's trail character and the Xeroc's disguise */
				if (mask & MONSTER) {
					monster = object_at(&level_monsters, i, j);
					if (monster) {
						dungeon[i][j] &= ~MONSTER;
						monster->trail_char = get_dungeon_char(i,j);
						dungeon[i][j] |= MONSTER;
					}
				}

				/* draw the character */
				addcch(get_dungeon_char(i, j));
			}
		}
	}
	mvaddcch(rogue.row, rogue.col, get_rogue_char());
	print_stats(STAT_ALL);
}


void gr_row_col(short *row, short *col, const unsigned short mask)
{
	short rn;
	short r, c;

	do {
		r = (short) get_rand(MIN_ROW, DROWS-2);
		c = (short) get_rand(0, DCOLS-1);
		rn = get_room_number(r, c);
	} while ((rn == NO_ROOM) ||
		(!(dungeon[r][c] & mask)) ||
		(dungeon[r][c] & (~mask)) ||
		(!(rooms[rn].is_room & (R_ROOM | R_MAZE))) ||
		((r == rogue.row) && (c == rogue.col)));

	*row = r;
	*col = c;
}


short gr_room(void)
{
	short i;

	do {
		i = (short) get_rand(0, MAXROOMS-1);
	} while (!(rooms[i].is_room & (R_ROOM | R_MAZE)));

	return(i);
}


short party_objects(const short rn)
{
	short i, j, nf = 0;
	object *obj;
	short n, N, row, col;
	boolean found;

	N = ((rooms[rn].bottom_row - rooms[rn].top_row) - 1) *
		((rooms[rn].right_col - rooms[rn].left_col) - 1);
	n =  (short) get_rand(5, 10);
	if (n > N) {
		n = N - 2;
	}
	for (i = 0; i < n; i++) {
		for (j = found = 0; ((!found) && (j < 250)); j++) {
			row = (short) get_rand(rooms[rn].top_row+1,
					   rooms[rn].bottom_row-1);
			col = (short) get_rand(rooms[rn].left_col+1,
					   rooms[rn].right_col-1);
			if ((dungeon[row][col] == FLOOR) || (dungeon[row][col] == TUNNEL)) {
				found = 1;
			}
		}
		if (found) {
			obj = gr_object();
			place_at(obj, row, col);
			nf++;
		}
	}
	return(nf);
}


short get_room_number(const short row, const short col)
{
	short i;

	for (i = 0; i < MAXROOMS; i++) {
		if ((row >= rooms[i].top_row) && (row <= rooms[i].bottom_row) &&
			(col >= rooms[i].left_col) && (col <= rooms[i].right_col)) {
			return(i);
		}
	}
	return(NO_ROOM);
}


int is_all_connected(void)
{
	short i, starting_room;

	for (i = 0; i < MAXROOMS; i++) {
		rooms_visited[i] = 0;
		if (rooms[i].is_room & (R_ROOM | R_MAZE)) {
			starting_room = i;
		}
	}

	visit_rooms(starting_room);

	for (i = 0; i < MAXROOMS; i++) {
		if ((rooms[i].is_room & (R_ROOM | R_MAZE)) && (!rooms_visited[i])) {
			return(0);
		}
	}
	return(1);
}


static void visit_rooms(const int rn)		/* only used within this file */
{
	short i;
	short oth_rn;

	rooms_visited[rn] = 1;

	for (i = 0; i < 4; i++) {
		oth_rn = rooms[rn].doors[i].oth_room;
		if ((oth_rn >= 0) && (!rooms_visited[oth_rn])) {
			visit_rooms(oth_rn);
		}
	}
}


/* NS: Fixed a bug here; if the rogue read a magic mapping scroll while
 *		standing on a trap, he disappears from the screen.
 */

void draw_magic_map(void)
{
	short i, j, ch;
	color_char cch, occh;
	unsigned short mask = (ANYROOMSIDE | DOOR | TUNNEL | TRAP | STAIRS |
			MONSTER);
	unsigned short s;

	for (i = 0; i < DROWS; i++) {
		for (j = 0; j < DCOLS; j++) {
			s = dungeon[i][j];
			if (s & mask) {
				ch = (short) mvinch(i,j);
				cch.b16 = mvincch(i,j).b16;
				if ((ch == ' ')
					|| ((ch >= 'A') && (ch <= 'Z'))
					|| (s & (TRAP | HIDDEN))) {
					occh.b16 = cch.b16;
					dungeon[i][j] &= (~HIDDEN);
					if (s & HORWALL) {
						cch = get_terrain_char(HORWALL);
					} else if (s & VERTWALL) {
						cch = get_terrain_char(VERTWALL);
					} else if (s & ULCORNER) {
						cch = get_terrain_char(ULCORNER);
					} else if (s & URCORNER) {
						cch = get_terrain_char(URCORNER);
					} else if (s & LLCORNER) {
						cch = get_terrain_char(LLCORNER);
					} else if (s & LRCORNER) {
						cch = get_terrain_char(LRCORNER);
					} else if (s & DOOR) {
						cch = get_terrain_char(DOOR);
					} else if (s & TRAP) {
						cch = get_terrain_char(TRAP);
					} else if (s & STAIRS) {
						cch = get_terrain_char(STAIRS);
					} else if (s & TUNNEL) {
						cch = get_terrain_char(TUNNEL);
					} else {
						continue;
					}
					if ((!(s & MONSTER)) || (ch == ' ')) {
						addcch(cch);
					}
					if (s & MONSTER) {
						object *monster;

						if (monster = object_at(&level_monsters, i, j)) {
							monster->trail_char.b16 = cch.b16;
						}
					}
				}
			}
		}
	}
	mvaddcch(rogue.row, rogue.col, get_rogue_char());
}


void dr_course(object *monster, const boolean entering, short row, short col)
{
	short i, j, k, rn;
	short r, rr;

	monster->row = row;
	monster->col = col;

	if (mon_sees(monster, rogue.row, rogue.col)) {
		monster->trow = NO_ROOM;
		return;
	}
	rn = get_room_number(row, col);

	if (entering) {		/* entering room */
		/* look for door to some other room */
		r = (short) get_rand(0, MAXROOMS-1);
		for (i = 0; i < MAXROOMS; i++) {
			rr = (r + i) % MAXROOMS;
			if ((!(rooms[rr].is_room & (R_ROOM | R_MAZE))) || (rr == rn)) {
				continue;
			}
			for (k = 0; k < 4; k++) {
				if (rooms[rr].doors[k].oth_room == rn) {
					monster->trow = rooms[rr].doors[k].oth_row;
					monster->tcol = rooms[rr].doors[k].oth_col;
					if ((monster->trow == row) &&
						(monster->tcol == col)) {
						continue;
					}
					return;
				}
			}
		}
		/* look for door to dead end */
		for (i = rooms[rn].top_row; i <= rooms[rn].bottom_row; i++) {
			for (j = rooms[rn].left_col; j <= rooms[rn].right_col; j++) {
				if ((i != monster->row) && (j != monster->col) &&
					(dungeon[i][j] & DOOR)) {
					monster->trow = i;
					monster->tcol = j;
					return;
				}
			}
		}
		/* return monster to room that he came from */
		for (i = 0; i < MAXROOMS; i++) {
			for (j = 0; j < 4; j++) {
				if (rooms[i].doors[j].oth_room == rn) {
					for (k = 0; k < 4; k++) {
						if (rooms[rn].doors[k].oth_room == i) {
							monster->trow = rooms[rn].doors[k].oth_row;
							monster->tcol = rooms[rn].doors[k].oth_col;
							return;
						}
					}
				}
			}
		}
		/* no place to send monster */
		monster->trow = NO_ROOM;
	} else {		/* exiting room */
		if (!get_oth_room(rn, &row, &col)) {
			monster->trow = NO_ROOM;
		} else {
			monster->trow = row;
			monster->tcol = col;
		}
	}
}


static int get_oth_room(const short rn, short *row, short *col)		/* only used within this file */
{
	short d = -1;

	if (*row == rooms[rn].top_row) {
		d = UPWARD/2;
	} else if (*row == rooms[rn].bottom_row) {
		d = DOWN/2;
	} else if (*col == rooms[rn].left_col) {
		d = LEFT/2;
	} else if (*col == rooms[rn].right_col) {
		d = RIGHT/2;
	}
	if ((d != -1) && (rooms[rn].doors[d].oth_room >= 0)) {
		*row = rooms[rn].doors[d].oth_row;
		*col = rooms[rn].doors[d].oth_col;
		return(1);
	}
	return(0);
}


/* NS: Now supports a redraw option when the screen color or
 *		character set changes.  At the moment, the 'regen_necc'
 *		feature in the options array only works for boolean
 *		options, but that's all we need at the moment.
 */

void edit_opts(void)
{
	color_char save[NOPTS+1][DCOLS];
	short i, j;
	short ch;
	boolean oldval[NOPTS];
	boolean done = 0, need_regen = 0;
	char buf[MAX_OPT_LEN + 2];

	color_kludge = use_color;

	for (i = 0; i < NOPTS+1; i++) {
		for (j = 0; j < DCOLS; j++) {
			save[i][j] = mvincch(i, j);
		}
		if (i < NOPTS) {
			opt_show(i);
			if (options[i].is_bool) {
				oldval[i] = *(options[i].bval);
			}
		}
	}
	opt_go(0);
	i = 0;

	while (!done) {
		refresh();
		ch = rgetchar();
CH:
		switch(ch) {
		case '\033':
			done = 1;
			break;
		case '\012':
		case '\015':
			if (i == (NOPTS - 1)) {
				mvaddstr(NOPTS, 0, press_space);
				refresh();
				wait_for_ack();
				done = 1;
			} else {
				i++;
				opt_go(i);
			}
			break;
		case '-':
			if (i > 0) {
				opt_go(--i);
			} else {
				sound_bell();
			}
			break;
		case 't':
		case 'T':
		case 'f':
		case 'F':
			if (options[i].is_bool) {
				*(options[i].bval) = (((ch == 't') || (ch == 'T')) ? 1 : 0);
				opt_show(i);
				opt_go(++i);
				break;
			}
		default:
			if (options[i].is_bool) {
				sound_bell();
				break;
			}
			j = 0;
			if ((ch == '\010') || ((ch >= ' ') && (ch <= '~'))) {
				opt_erase(i);
				do {
					if ((ch >= ' ') && (ch <= '~') && (j < MAX_OPT_LEN)) {
						buf[j++] = (char) ch;
						buf[j] = '\0';
						addch(ch);
					} else if ((ch == '\010') && (j > 0)) {
						buf[--j] = '\0';
						move(i, (short) (j + strlen(options[i].prompt)));
						addch(' ');
						move(i, (short) (j + strlen(options[i].prompt)));
					}
					refresh();
					ch = rgetchar();
				} while ((ch != '\012') && (ch != '\015') && (ch != '\033'));
				if (j != 0) {
				    strip(buf, options[i].add_blank);
					(void) strcpy(*(options[i].strval), buf);
				}
				opt_show(i);
				goto CH;
			} else {
				sound_bell();
			}
			break;
		}
	}

	for (i = 0; i < NOPTS+1; i++) {
		move(i, 0);
		for (j = 0; j < DCOLS; j++) {
			addcch(save[i][j]);
		}
		if (options[i].is_bool && options[i].redraw_necc
				&& (*(options[i].bval) != oldval[i])) {
			need_regen = 1;
		}
	}
	
	use_color = color_kludge;

	if (need_regen) {
		regenerate_screen();
	}
}


static void opt_show(const short i)
{
	char *s;
	struct option *opt = &options[i];

	opt_erase(i);

	if (opt->is_bool) {
		s = *(opt->bval) ? "True" : "False";
	} else {
		s = *(opt->strval);
	}
	addstr(s);
}


static void opt_erase(const short i)
{
	struct option *opt = &options[i];

	mvaddstr_in_color(i, 0, opt->prompt,
					  (unsigned char) (use_color ? MAKE_COLOR(CYAN, BLACK) :
					  			  MAKE_COLOR(WHITE,BLACK)));
	clrtoeol();
}


static void opt_go(const short i)	/* only used within this file */	
{
	move(i, (short) strlen(options[i].prompt));
}


void do_shell(void)
{
	char *sh;

	md_ignore_signals();
	if (!(sh = md_getenv("SHELL"))) {
		sh = md_getenv("COMSPEC");
	}
	move( (short) (LINES-1), 0);
	refresh();
	stop_window();
	printf("\n\nSpawning new shell. Type 'exit' to return to rogue...\n");
	md_shell(sh);
	start_window();
	redraw();
	md_heed_signals();
}


/* Gets a random object character for Xeroc disguises and hallucinations.
 * NS: moved this here from monster.c.  This returns a random color
 * character if the argument is <0, or the specific character index
 * if >=0.  In both cases, the current color & character set values
 * are honored.  All this work is to keep Xerocs hidden when the user
 * switches display options; there is surely an easier way to do it...
 */

color_char gr_obj_char(const int ix)
{
	struct _screenchars *sc;
	color_char cc;

	/* initialize groc array and check for failing case */
	if (dc_len < 0) {
		init_groc_array();
	}
	if (dc_len <= 0) { 		/* ugh */
		cc.b8.color = MAKE_COLOR(WHITE, BLACK);
		cc.b8.ch = '~';
		return cc;
	}

	/* get the character we are to use for the disguise */
	if ((ix < 0) || (ix >= dc_len)) {
		sc = disguise_chars[get_rand(0, dc_len-1)];
	} else {
		sc = disguise_chars[ix];
	}

	/* construct the color character */
	cc.b8.ch = (use_doschars) ? sc->dosch : sc->ch;
	cc.b8.color = (use_color) ?	MAKE_COLOR(sc->fgcolor, sc->bgcolor) :
								MAKE_COLOR(WHITE, BLACK);
	return cc;
}


/* Returns the index of a Xeroc's disguise.  This won't change for
 * the life of the monster, while the actual character might if the
 * user switches display options on us.
 */

int gr_obj_index(void)
{
	/* initialize groc array and check for failing case */
	if (dc_len < 0) {
		init_groc_array();
	}
	if (dc_len <= 0) {
		return -1;
	} else {
		return get_rand(0, dc_len-1);
	}
}



/* initializes the disguise/hallucination() array.  Called upon first use of
 * gr_obj_char() or gr_obj_index()
 */

static void init_groc_array(void)	/* only used within this file */
{
	int i;

	dc_len = 0;
	for (i=0; i<OBJECTTYPES; i++) {
		if (scrObjects[i].canBeMimicked) {
			disguise_chars[dc_len] = &scrObjects[i];
			dc_len++;
		}
	}
	for (i=0; i<TERRAINTYPES; i++) {
		if (scrTerrain[i].canBeMimicked) {
			disguise_chars[dc_len] = &scrTerrain[i];
			dc_len++;
		}
	}
}
