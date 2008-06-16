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
 * @(#)object.c 8.1 (Berkeley) 5/31/93
 */

/*
 * object.c
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
#include "paths.h"

#define MIN(x,y)	(((x) < (y)) ? (x) : (y))
#define MAX(x,y)	(((x) > (y)) ? (x) : (y))

object level_objects;
unsigned short dungeon[DROWS][DCOLS];
short foods = 0;
object *free_list = (object *) 0;
char *fruit = (char *) 0;

fighter rogue = {
	INIT_AW,	/* armor, weapon */
	INIT_RINGS,	/* rings */
	INIT_HPCUR,		/* current Hp */
	INIT_HPMAX,		/* max Hp */
	INIT_STRCUR,	/* current Str */
	INIT_STRMAX,	/* max Str */
	INIT_PACK,	/* pack */
	INIT_GOLD,	/* gold */
	INIT_EXP,	/* exp level,points */
	0, 0,		/* row, col */
	INIT_CHAR_COLOR,  /* for color mode play */
	INIT_CHAR,	/* char */
	INIT_DOSCHAR, /* DOS character */
	INIT_MOVES	/* moves */
};

struct id id_potions[POTIONS] = {
{100, "blue \0                           ", "of increase strength ", 0},
{250, "red \0                            ", "of restore strength ", 0},
{100, "green \0                          ", "of healing ", 0},
{200, "grey \0                           ", "of extra healing ", 0},
 {10, "brown \0                          ", "of poison ", 0},
{300, "clear \0                          ", "of raise level ", 0},
 {10, "pink \0                           ", "of blindness ", 0},
 {25, "white \0                          ", "of hallucination ", 0},
{100, "purple \0                         ", "of detect monster ", 0},
{100, "black \0                          ", "of detect things ", 0},
 {10, "yellow \0                         ", "of confusion ", 0},
 {80, "plaid \0                          ", "of levitation ", 0},
{150, "burgundy \0                       ", "of haste self ", 0},
{145, "beige \0                          ", "of see invisible ", 0}
};


/*  NS: Each object now has a unique starting name, so that an ANSI
 *		compiler (such as gcc) will create separate entries in the
 *		string table.  Otherwise, when we initialize them with
 *		random names, they'll all wind up with the same name (because
 *		they point to the same shared string buffer).
 */

struct id id_scrolls[SCROLS] = {
{505, "string title #1                    ", "of protect armor ", 0},
{200, "string title #2                    ", "of hold monster ", 0},
{235, "string title #3                    ", "of enchant weapon ", 0},
{235, "string title #4                    ", "of enchant armor ", 0},
{175, "string title #5                    ", "of identify ", 0},
{190, "string title #6                    ", "of teleportation ", 0},
 {25, "string title #7                    ", "of sleep ", 0},
{610, "string title #8                    ", "of scare monster ", 0},
{210, "string title #9                    ", "of remove curse ", 0},
 {80, "string title #10                   ", "of create monster ",0},
 {25, "string title #11                   ", "of aggravate monster ",0},
{180, "string title #12                   ", "of magic mapping ", 0},
 {90, "string title #13                   ", "of confuse monster ", 0}
};

struct id id_weapons[WEAPONS] = {
	{150, "short bow ", "", 0},
	  {8, "darts ", "", 0},
	 {15, "arrows ", "", 0},
	 {27, "daggers ", "", 0},
	 {35, "shurikens ", "", 0},
	{360, "mace ", "", 0},
	{470, "long sword ", "", 0},
	{580, "two-handed sword ", "", 0}
};

struct id id_armors[ARMORS] = {
	{300, "leather armor ", "", (UNIDENTIFIED)},
	{300, "ring mail ", "", (UNIDENTIFIED)},
	{400, "scale mail ", "", (UNIDENTIFIED)},
	{500, "chain mail ", "", (UNIDENTIFIED)},
	{600, "banded mail ", "", (UNIDENTIFIED)},
	{600, "splint mail ", "", (UNIDENTIFIED)},
	{700, "plate mail ", "", (UNIDENTIFIED)}
};

struct id id_wands[WANDS] = {
	 {25, "wand title #1                    ", "of teleport away ",0},
	 {50, "wand title #2                    ", "of slow monster ", 0},
	  {8, "wand title #3                    ", "of invisibility ",0},
	 {55, "wand title #4                    ", "of polymorph ",0},
	  {2, "wand title #5                    ", "of haste monster ",0},
	 {20, "wand title #6                    ", "of magic missile ",0},
	 {20, "wand title #7                    ", "of cancellation ",0},
	  {0, "wand title #8                    ", "of do nothing ",0},
	 {35, "wand title #9                    ", "of drain life ",0},
	 {20, "wand title #10                   ", "of cold ",0},
	 {20, "wand title #11                   ", "of fire ",0}
};

struct id id_rings[RINGS] = {
	 {250, "ring title #1                    ", "of stealth ",0},
	 {100, "ring title #2                    ", "of teleportation ", 0},
	 {255, "ring title #3                    ", "of regeneration ",0},
	 {295, "ring title #4                    ", "of slow digestion ",0},
	 {200, "ring title #5                    ", "of add strength ",0},
	 {250, "ring title #6                    ", "of sustain strength ",0},
	 {250, "ring title #7                    ", "of dexterity ",0},
	  {25, "ring title #8                    ", "of adornment ",0},
	 {300, "ring title #9                    ", "of see invisible ",0},
	 {290, "ring title #10                   ", "of maintain armor ",0},
	 {270, "ring title #11                   ", "of searching ",0},
};

extern short cur_level, max_level;
extern short party_room;
extern boolean is_wood[];
extern boolean use_color;


static void put_gold(void);			/* only used within this file */
static void plant_gold(const short row, const short col, const boolean is_maze);			/* only used within this file */
static void gr_scroll(object *obj);			/* only used within this file */
static void gr_potion(object *obj);			/* only used within this file */
static void gr_weapon(object *obj, const int assign_wk);/* only used within this file */	
static void gr_armor(object *obj);			/* only used within this file */
static void gr_wand(object *obj);			/* only used within this file */
static void make_party(void);				/* only used within this file */
static void rand_place(object *obj);		/* only used within this file */


void put_objects(void)
{
	short i, n;
	object *obj;

	if (cur_level < max_level) {
		return;
	}
	n = (short) (coin_toss() ? get_rand(2, 4) : get_rand(3, 5));
	while (rand_percent(33)) {
		n++;
	}
	if (party_room != NO_ROOM) {
		make_party();
	}
	for (i = 0; i < n; i++) {
		obj = gr_object();
		rand_place(obj);
	}
	put_gold();
}


static void put_gold(void)			/* only used within this file */
{
	short i, j;
	short row,col;
	boolean is_maze, is_room;

	for (i = 0; i < MAXROOMS; i++) {
		is_maze = (rooms[i].is_room & R_MAZE) ? 1 : 0;
		is_room = (rooms[i].is_room & R_ROOM) ? 1 : 0;

		if (!(is_room || is_maze)) {
			continue;
		}
		if (is_maze || rand_percent(GOLD_PERCENT)) {
			for (j = 0; j < 50; j++) {
				row = (short) get_rand(rooms[i].top_row+1,
				rooms[i].bottom_row-1);
				col = (short) get_rand(rooms[i].left_col+1,
				rooms[i].right_col-1);
				if ((dungeon[row][col] == FLOOR) ||
					(dungeon[row][col] == TUNNEL)) {
					plant_gold(row, col, is_maze);
					break;
				}
			}
		}
	}
}


static void plant_gold(const short row, const short col, const boolean is_maze)	/* only used within this file */
{
	object *obj;

	obj = alloc_object();
	obj->row = row; obj->col = col;
	obj->what_is = GOLD;
	obj->quantity = (short) get_rand((2 * cur_level), (16 * cur_level));
	if (is_maze) {
		obj->quantity += obj->quantity / 2;
	}
	dungeon[row][col] |= OBJECT;
	(void) add_to_pack(obj, &level_objects, 0);
}


void place_at(object *obj, const short row, const short col)
{
	obj->row = row;
	obj->col = col;
	dungeon[row][col] |= OBJECT;
	(void) add_to_pack(obj, &level_objects, 0);
}


object * object_at(object *pack, const short row, const short col)
{
	object *obj = (object *) 0;

	if (dungeon[row][col] & (MONSTER | OBJECT)) {
		obj = pack->next_object;

		while (obj && ((obj->row != row) || (obj->col != col))) {
			obj = obj->next_object;
		}
		if (!obj) {
			message("object_at(): inconsistent", 1);
		}
	}
	return(obj);
}


object * get_letter_object(const short ch)
{
	object *obj;

	obj = rogue.pack.next_object;

	while (obj && (obj->ichar != ch)) {
		obj = obj->next_object;
	}
	return(obj);
}


void free_stuff(object *objlist)
{
	object *obj;

	while (objlist->next_object) {
		obj = objlist->next_object;
		objlist->next_object =
			objlist->next_object->next_object;
		free_object(obj);
	}
}


char * name_of(const object *obj)
{
	char *retstring;

	switch(obj->what_is) {
	case SCROL:
		retstring = obj->quantity > 1 ? "scrolls " : "scroll ";
		break;
	case POTION:
		retstring = obj->quantity > 1 ? "potions " : "potion ";
		break;
	case FOOD:
		if (obj->which_kind == RATION) {
			retstring = "food ";
		} else {
			retstring = fruit;
		}
		break;
	case WAND:
		retstring = is_wood[obj->which_kind] ? "staff " : "wand ";
		break;
	case WEAPON:
		switch(obj->which_kind) {
		case DART:
			retstring=obj->quantity > 1 ? "darts " : "dart ";
			break;
		case ARROW:
			retstring=obj->quantity > 1 ? "arrows " : "arrow ";
			break;
		case DAGGER:
			retstring=obj->quantity > 1 ? "daggers " : "dagger ";
			break;
		case SHURIKEN:
			retstring=obj->quantity > 1?"shurikens ":"shuriken ";
			break;
		default:
			retstring = id_weapons[obj->which_kind].title;
		}
		break;
	case ARMOR:
		retstring = "armor ";
		break;
	case RING:
			retstring = "ring ";
		break;
	case AMULET:
		retstring = "amulet ";
		break;
	default:
		retstring = "unknown ";
		break;
	}
	return(retstring);
}


object * gr_object(void)
{
	object *obj;

	obj = alloc_object();

	if (foods < (cur_level / 3)) {
		obj->what_is = FOOD;
		foods++;
	} else {
		obj->what_is = gr_what_is();
	}
	switch(obj->what_is) {
	case SCROL:
		gr_scroll(obj);
		break;
	case POTION:
		gr_potion(obj);
		break;
	case WEAPON:
		gr_weapon(obj, 1);
		break;
	case ARMOR:
		gr_armor(obj);
		break;
	case WAND:
		gr_wand(obj);
		break;
	case FOOD:
		get_food(obj, 0);
		break;
	case RING:
		gr_ring(obj, 1);
		break;
	}
	return(obj);
}


unsigned short gr_what_is(void)
{
	short percent;
	unsigned short what_is;

	percent = (short) get_rand(1, 91);

	if (percent <= 30) {
		what_is = SCROL;
	} else if (percent <= 60) {
		what_is = POTION;
	} else if (percent <= 64) {
		what_is = WAND;
	} else if (percent <= 74) {
		what_is = WEAPON;
	} else if (percent <= 83) {
		what_is = ARMOR;
	} else if (percent <= 88) {
		what_is = FOOD;
	} else {
		what_is = RING;
	}
	return(what_is);
}


static void gr_scroll(object *obj)			/* only used within this file */
{
	short percent;

	percent = (short) get_rand(0, 91);

	obj->what_is = SCROL;

	if (percent <= 5) {
		obj->which_kind = PROTECT_ARMOR;
	} else if (percent <= 10) {
		obj->which_kind = HOLD_MONSTER;
	} else if (percent <= 20) {
		obj->which_kind = CREATE_MONSTER;
	} else if (percent <= 35) {
		obj->which_kind = IDENTIFY;
	} else if (percent <= 43) {
		obj->which_kind = TELEPORT;
	} else if (percent <= 50) {
		obj->which_kind = SLEEP;
	} else if (percent <= 55) {
		obj->which_kind = SCARE_MONSTER;
	} else if (percent <= 64) {
		obj->which_kind = REMOVE_CURSE;
	} else if (percent <= 69) {
		obj->which_kind = ENCH_ARMOR;
	} else if (percent <= 74) {
		obj->which_kind = ENCH_WEAPON;
	} else if (percent <= 80) {
		obj->which_kind = AGGRAVATE_MONSTER;
	} else if (percent <= 86) {
		obj->which_kind = CON_MON;
	} else {
		obj->which_kind = MAGIC_MAPPING;
	}
}


static void gr_potion(object *obj)	/* only used within this file */
{
	short percent;

	percent = (short) get_rand(1, 118);

	obj->what_is = POTION;

	if (percent <= 5) {
		obj->which_kind = RAISE_LEVEL;
	} else if (percent <= 15) {
		obj->which_kind = DETECT_OBJECTS;
	} else if (percent <= 25) {
		obj->which_kind = DETECT_MONSTER;
	} else if (percent <= 35) {
		obj->which_kind = INCREASE_STRENGTH;
	} else if (percent <= 45) {
		obj->which_kind = RESTORE_STRENGTH;
	} else if (percent <= 55) {
		obj->which_kind = HEALING;
	} else if (percent <= 65) {
		obj->which_kind = EXTRA_HEALING;
	} else if (percent <= 75) {
		obj->which_kind = BLINDNESS;
	} else if (percent <= 85) {
		obj->which_kind = HALLUCINATION;
	} else if (percent <= 95) {
		obj->which_kind = CONFUSION;
	} else if (percent <= 105) {
		obj->which_kind = POISON;
	} else if (percent <= 110) {
		obj->which_kind = LEVITATION;
	} else if (percent <= 114) {
		obj->which_kind = HASTE_SELF;
	} else {
		obj->which_kind = SEE_INVISIBLE;
	}
}


static void gr_weapon(object *obj, const int assign_wk)	/* only used within this file */	
{
	short percent;
	short i;
	short blessing, increment;

	obj->what_is = WEAPON;
	if (assign_wk) {
		obj->which_kind = (unsigned short) get_rand(0, (WEAPONS - 1));
	}
	if ((obj->which_kind == ARROW) || (obj->which_kind == DAGGER) ||
		(obj->which_kind == SHURIKEN) || (obj->which_kind == DART)) {
		obj->quantity = (short) get_rand(3, 15);
		obj->quiver = (short) get_rand(0, 126);
	} else {
		obj->quantity = 1;
	}
	obj->hit_enchant = obj->d_enchant = 0;

	percent = (short) get_rand(1, 96);
	blessing = (short) get_rand(1, 3);

	if (percent <= 16) {
		increment = 1;
	} else if (percent <= 32) {
		increment = -1;
		obj->is_cursed = 1;
	}
	if (percent <= 32) {
		for (i = 0; i < blessing; i++) {
			if (coin_toss()) {
				obj->hit_enchant += increment;
			} else {
				obj->d_enchant += increment;
			}
		}
	}
	switch(obj->which_kind) {
	case BOW:
	case DART:
		obj->damage = "1d1";
		break;
	case ARROW:
		obj->damage = "1d2";
		break;
	case DAGGER:
		obj->damage = "1d3";
		break;
	case SHURIKEN:
		obj->damage = "1d4";
		break;
	case MACE:
		obj->damage = "2d3";
		break;
	case LONG_SWORD:
		obj->damage = "3d4";
		break;
	case TWO_HANDED_SWORD:
		obj->damage = "4d5";
		break;
	}
}


static void gr_armor(object *obj)		/* only used within this file */
{
	short percent;
	short blessing;

	obj->what_is = ARMOR;
	obj->which_kind = (unsigned short) get_rand(0, (ARMORS - 1));
	obj->class = obj->which_kind + 2;
	if ((obj->which_kind == PLATE) || (obj->which_kind == SPLINT)) {
		obj->class--;
	}
	obj->is_protected = 0;
	obj->d_enchant = 0;

	percent = (short) get_rand(1, 100);
	blessing = (short) get_rand(1, 3);

	if (percent <= 16) {
		obj->is_cursed = 1;
		obj->d_enchant -= blessing;
	} else if (percent <= 33) {
		obj->d_enchant += blessing;
	}
}


static void gr_wand(object *obj)			/* only used within this file */
{
	obj->what_is = WAND;
	obj->which_kind = (unsigned short) get_rand(0, (WANDS - 1));
	obj->class = (short) get_rand(3, 7);
}


void get_food(object *obj, const boolean force_ration)
{
	obj->what_is = FOOD;

	if (force_ration || rand_percent(80)) {
		obj->which_kind = RATION;
	} else {
		obj->which_kind = FRUIT;
	}
}


void put_stairs(void)
{
	short row, col;

	gr_row_col(&row, &col, (FLOOR | TUNNEL));
	dungeon[row][col] |= STAIRS;
}


int get_armor_class(const object *obj)
{
	if (obj) {
		return(obj->class + obj->d_enchant);
	}
	return(0);
}


object * alloc_object(void)
{
	object *obj;

	if (free_list) {
		obj = free_list;
		free_list = free_list->next_object;
	} else if (!(obj = (object *) md_malloc(sizeof(object)))) {
			message("cannot allocate object, saving game", 0);
			save_into_file(_PATH_ERRORFILE);
	}
	obj->quantity = 1;
	obj->ichar = 'L';
	obj->picked_up = obj->is_cursed = 0;
	obj->in_use_flags = NOT_USED;
	obj->identified = UNIDENTIFIED;
	obj->damage = "1d1";
	return(obj);
}


void free_object(object *obj)
{
	obj->next_object = free_list;
	free_list = obj;
}


static void make_party(void)			/* only used within this file */
{
	short n;

	party_room = gr_room();

	n = rand_percent(99) ? party_objects(party_room) : 11;
	if (rand_percent(99)) {
		party_monsters(party_room, n);
	}
}


void show_objects(void)
{
	object *obj;
	color_char rc;
	short mc, row, col;
	object *monster;

	obj = level_objects.next_object;

	while (obj) {
		row = obj->row;
		col = obj->col;

		rc = get_mask_char(obj->what_is);

		if (dungeon[row][col] & MONSTER) {
			if (monster = object_at(&level_monsters, row, col)) {
				monster->trail_char.b16 = rc.b16;
			}
		}
		mc = (short) mvinch(row, col);
		if (((mc < 'A') || (mc > 'Z')) &&
			((row != rogue.row) || (col != rogue.col))) {
			mvaddcch(row, col, rc);
		}
		obj = obj->next_object;
	}

	monster = level_monsters.next_object;

	while (monster) {
		if (monster->m_flags & IMITATES) {
			rc.b16 = gr_obj_char(monster->disguise).b16;
			mvaddcch(monster->row, monster->col, rc);
		}
		monster = monster->next_monster;
	}
}


void put_amulet(void)
{
	object *obj;

	obj = alloc_object();
	obj->what_is = AMULET;
	rand_place(obj);
}


static void rand_place(object *obj)		/* only used within this file */
{
	short row, col;

	gr_row_col(&row, &col, (FLOOR | TUNNEL));
	place_at(obj, row, col);
}


void c_object_for_wizard(void)
{
	short ch, max, wk;
	object *obj;
	char buf[80];

	if (pack_count((object *) 0) >= MAX_PACK_COUNT) {
		message("pack full", 0);
		return;
	}
	message("type of object?", 0);

	while (r_index("!?:)]=/,\033", (ch = rgetchar()), 0) == -1) {
		sound_bell();
	}
	check_message();

	if (ch == '\033') {
		return;
	}
	obj = alloc_object();

	switch(ch) {
	case '!':
		obj->what_is = POTION;
		max = POTIONS - 1;
		break;
	case '?':
		obj->what_is = SCROL;
		max = SCROLS - 1;
		break;
	case ',':
		obj->what_is = AMULET;
		break;
	case ':':
		get_food(obj, 0);
		break;
	case ')':
		gr_weapon(obj, 0);
		max = WEAPONS - 1;
		break;
	case ']':
		gr_armor(obj);
		max = ARMORS - 1;
		break;
	case '/':
		gr_wand(obj);
		max = WANDS - 1;
		break;
	case '=':
		max = RINGS - 1;
		obj->what_is = RING;
		break;
	}
	if ((ch != ',') && (ch != ':')) {
GIL:
		if (get_input_line("which kind?", "", buf, "", 0, 1)) {
			wk = (short) get_number(buf);
			if ((wk >= 0) && (wk <= max)) {
				obj->which_kind = (unsigned short) wk;
				if (obj->what_is == RING) {
					gr_ring(obj, 0);
				}
			} else {
				sound_bell();
				goto GIL;
			}
		} else {
			free_object(obj);
			return;
		}
	}
	get_desc(obj, buf);
	message(buf, 0);
	(void) add_to_pack(obj, &rogue.pack, 1);
}



#ifdef DISCOVERY
/* NS: Shows a list of what's been discovered for each of the basic
 *	   object types.  Under development.
 */

void discovery(void)
{
	object *obj;
	short i = 0, j, maxlen = 0, n;
	char bwdesc[DCOLS];
	color_char descs[MAX_PACK_COUNT+1][DCOLS];
	short row, col;
	byte letter_color, protect_color, item_color;

	obj = pack->next_object;

	if (!obj) {
		message("your pack is empty", 0);
		return;
	}

	if (use_color) {
		letter_color = MAKE_COLOR(INV_LETTER_COLOR, BLACK);
		protect_color = MAKE_COLOR(INV_PROTECT_COLOR, BLACK);
		item_color = MAKE_COLOR(INV_ITEM_COLOR, BLACK);
	} else {
		letter_color = MAKE_COLOR(WHITE,BLACK);
		protect_color = MAKE_COLOR(WHITE,BLACK);
		item_color = MAKE_COLOR(WHITE,BLACK);
	}

	while (obj) {
		if (obj->what_is & mask) {
			descs[i][0].b8.color = MAKE_COLOR(WHITE, BLACK);
			descs[i][0].b8.ch = ' ';
			descs[i][1].b8.color = letter_color;
			descs[i][1].b8.ch = obj->ichar;
			if ((obj->what_is & ARMOR) && obj->is_protected) {
				descs[i][2].b8.color = protect_color;
				descs[i][2].b8.ch = '}';
			} else {
				descs[i][2].b8.color = letter_color;
				descs[i][2].b8.ch = ')';
			}
			descs[i][3].b8.color = MAKE_COLOR(WHITE, BLACK);
			descs[i][3].b8.ch = ' ';

			get_desc(obj, bwdesc);
			(void) colorize(bwdesc, item_color, descs[i]+4);
			if ((n = (short) strlen(bwdesc)+4) > maxlen) {
				maxlen = n;
			}
		i++;
		}
		obj = obj->next_object;
	}
	(void) colorize(press_space, MAKE_COLOR(WHITE, BLACK), descs[i++]);
	/* (void) strcpy(descs[i++], press_space); */
	if (maxlen < 27) maxlen = 27;
	col = DCOLS - (maxlen + 2);

	for (row = 0; ((row < i) && (row < DROWS)); row++) {
		if (row > 0) {
			for (j = col; j < DCOLS; j++) {
				descs[row-1][j-col].b16 = mvincch(row, j).b16;
			}
			descs[row-1][j-col].b16 = 0;
		}
		mvaddcstr(row, col, descs[row]);
		clrtoeol();
	}
	refresh();
	wait_for_ack();

	move(0, 0);
	clrtoeol();

	for (j = 1; ((j < i) && (j < DROWS)); j++) {
		mvaddcstr(j, col, descs[j-1]);
	}
}
#endif

#ifdef KNOWN_ITEMS
/*
 * Patch [ 1079744 ] Known Items
 * Submitted By: steve_ued
 */

struct known_items_struct_details
	{
	unsigned short 	is_known;		/* 0 if not known, if >= 1 then this is the
									   number of times this item is known (e.g., so you
									   could use this as the number of times this item
									   was used */

	unsigned short	known_item_what_is;		/* what type of object (wand, scroll, potion, etc) */
	unsigned short 	known_item_type;		/* subtype type of the item */
	char 			printable_string[MAX_KNOWN_ITEM_STRING_LENGTH + 1];  /* user friendly printable string */
	};

typedef struct known_items_struct_details known_items_struct;

#define MAX_KNOWN_ITEMS 250

static known_items_struct 	known_items_list[MAX_KNOWN_ITEMS];
static unsigned int 		number_of_known_items = 0;

/*-------------------------------------------------------------------------------------------*/
static void known_items_initialize_single_item(
					const unsigned short tmp_known_item_what_is, 
					const unsigned short tmp_known_item_type,
					const char *tmp_item_printable_string)
{
	known_items_struct *tmp;

	/* make sure that there is room to add the item to the list */
	if (number_of_known_items == MAX_KNOWN_ITEMS)
		{
		return;	/*do nothing - this should eventually cause some error message */
		}

	/* make sure that printable string length is not too long */


	tmp = &known_items_list[number_of_known_items];

	++number_of_known_items;

	tmp->is_known = 0;		
	tmp->known_item_what_is = tmp_known_item_what_is;
	tmp->known_item_type = tmp_known_item_type;
	
	strncpy(tmp->printable_string, tmp_item_printable_string, MAX_KNOWN_ITEM_STRING_LENGTH);

	tmp->printable_string[ MAX_KNOWN_ITEM_STRING_LENGTH ] = 0;	/* force the null termination of string*/
}



/*-------------------------------------------------------------------------------------------*/
void known_items_initialize(void)
{
	known_items_initialize_single_item(SCROL, PROTECT_ARMOR, "Scroll of Protect Armor");
	known_items_initialize_single_item(SCROL, HOLD_MONSTER, "Scroll of Hold Monster");
	known_items_initialize_single_item(SCROL, ENCH_WEAPON, "Scroll of Enchant Weapon");
	known_items_initialize_single_item(SCROL, ENCH_ARMOR, "Scroll of Enchant Armor");
	known_items_initialize_single_item(SCROL, IDENTIFY, "Scroll of Identify");
	known_items_initialize_single_item(SCROL, TELEPORT, "Scroll of Teleport");
	known_items_initialize_single_item(SCROL, SLEEP, "Scroll of Sleep");
	known_items_initialize_single_item(SCROL, SCARE_MONSTER, "Scroll of Scare Monster");
	known_items_initialize_single_item(SCROL, REMOVE_CURSE, "Scroll of Remove Curse");
	known_items_initialize_single_item(SCROL, CREATE_MONSTER, "Scroll of Create Monster");
	known_items_initialize_single_item(SCROL, AGGRAVATE_MONSTER, "Scroll of Aggravate Monster");
	known_items_initialize_single_item(SCROL, MAGIC_MAPPING, "Scroll of Magic Mapping");
	known_items_initialize_single_item(SCROL, CON_MON, "Scroll of Confuse Monster");

	known_items_initialize_single_item(POTION, INCREASE_STRENGTH, "Potion of Increase Strength");
	known_items_initialize_single_item(POTION, RESTORE_STRENGTH, "Potion of Restore Strength");
	known_items_initialize_single_item(POTION, HEALING, "Potion of Healing");
	known_items_initialize_single_item(POTION, EXTRA_HEALING, "Potion of Extra Healing");
	known_items_initialize_single_item(POTION, POISON, "Potion of Poison");
	known_items_initialize_single_item(POTION, RAISE_LEVEL, "Potion of Raise Level");
	known_items_initialize_single_item(POTION, BLINDNESS, "Potion of Blindness");
	known_items_initialize_single_item(POTION, HALLUCINATION, "Potion of Hallucination");
	known_items_initialize_single_item(POTION, DETECT_MONSTER, "Potion of Detect Monsters");
	known_items_initialize_single_item(POTION, DETECT_OBJECTS, "Potion of Detect Objects");
	known_items_initialize_single_item(POTION, CONFUSION, "Potion of Confusion");
	known_items_initialize_single_item(POTION, LEVITATION, "Potion of Levitation");
	known_items_initialize_single_item(POTION, HASTE_SELF, "Potion of Haste Self");
	known_items_initialize_single_item(POTION, SEE_INVISIBLE, "Potion of See Invisible");

	known_items_initialize_single_item(WAND, TELE_AWAY, "Wand of Teleport Away");
	known_items_initialize_single_item(WAND, SLOW_MONSTER, "Wand of Slow Monster");
	known_items_initialize_single_item(WAND, INVISIBILITY, "Wand of Invisibility");
	known_items_initialize_single_item(WAND, POLYMORPH, "Wand of Polymorph");
	known_items_initialize_single_item(WAND, HASTE_MONSTER, "Wand of Haste Monster");
	known_items_initialize_single_item(WAND, MAGIC_MISSILE, "Wand of Magic Missile");
	known_items_initialize_single_item(WAND, CANCELLATION, "Wand of Cancellation");
	known_items_initialize_single_item(WAND, DO_NOTHING, "Wand of Do Nothing");
	known_items_initialize_single_item(WAND, DRAIN_LIFE, "Wand of Drain Life");
	known_items_initialize_single_item(WAND, COLD, "Wand of Cold");
	known_items_initialize_single_item(WAND, FIRE, "Wand of Fire");

	known_items_initialize_single_item(RING, STEALTH, "Ring of Stealth");
	known_items_initialize_single_item(RING, R_TELEPORT, "Ring of Teleport");
	known_items_initialize_single_item(RING, REGENERATION, "Ring of Regeneration");
	known_items_initialize_single_item(RING, SLOW_DIGEST, "Ring of Slow Digestion");
	known_items_initialize_single_item(RING, ADD_STRENGTH, "Ring of Add Strength");
	known_items_initialize_single_item(RING, SUSTAIN_STRENGTH, "Ring of Sustain Strength");
	known_items_initialize_single_item(RING, DEXTERITY, "Ring of Dexterity");
	known_items_initialize_single_item(RING, ADORNMENT, "Ring of Adornment");
	known_items_initialize_single_item(RING, R_SEE_INVISIBLE, "Ring of See Invisible");
	known_items_initialize_single_item(RING, MAINTAIN_ARMOR, "Ring of Maintain Armor");
	known_items_initialize_single_item(RING, SEARCHING, "Ring of Searching");
}


/*----------------------------------------------------------------*/
void known_items_add_new_known_item(const unsigned short what_is, 
									const unsigned short item_type)
{
	unsigned int ctr;
	unsigned short tmp_what_is;

	/*we only want the item type what is field for usable items*/
	
	tmp_what_is = what_is & (SCROL | POTION | WEAPON | ARMOR | WAND | RING);

	for (ctr = 0; ctr < number_of_known_items; ctr++)
	{
		if (known_items_list[ctr].known_item_what_is == tmp_what_is)
		if (known_items_list[ctr].known_item_type == item_type)
		{
			known_items_list[ctr].is_known++;
			return;
		}
	}
	/*-------------------------------------------------*/
	/*should never get here*/
}


void do_show_items_known()
{
	short current_row_number, row_ctr, col_ctr, something_is_known;
	unsigned int ctr;
	color_char save[DROWS][DCOLS];

	something_is_known = 0;
	for (ctr = 0; ctr < number_of_known_items; ctr++)
		{
		if (known_items_list[ctr].is_known)
			{
			something_is_known = 1;
			break;
			}
		}


	if (something_is_known == 0)
		{
		check_message();
		message("You have not found anything yet...", 0);
		refresh();
		return;
		}

	/*save current screen*/
	for (row_ctr = 0; row_ctr < DROWS; row_ctr++) 
			{
			for (col_ctr = 0; col_ctr < DCOLS; col_ctr++) 
				{
				save[row_ctr][col_ctr] = mvincch(row_ctr, col_ctr);
				}
		}


	clear();	/*clear entire screen*/

	check_message();
	message("List of things known...", 0);

	current_row_number = 0;

	for (ctr = 0; ctr < number_of_known_items; ctr++)
		{
		if (known_items_list[ctr].is_known)
			{
			char *p = known_items_list[ctr].printable_string;

			++current_row_number;
			mvaddstr(current_row_number, 5, p);

			if ((current_row_number == DROWS) || (ctr + 1 == number_of_known_items))
				{		/*give a more prompt*/
				char *m = more;
				if (ctr + 1 == number_of_known_items)
					m = press_space;

				mvaddstr(current_row_number, 0, m);
				current_row_number = 0;

				refresh();
				wait_for_ack();

				if (rgetchar() == CANCEL)
					break;

				clear();	/*clear entire screen*/
				}
			}
		}

	/*-------------------------------------------------*/
	if (current_row_number > 0)
		{				/*new items known on screen - need to give 'press space to contiue' prompt*/
		++current_row_number;
		mvaddstr(current_row_number, 0, press_space);

		refresh();
		wait_for_ack();
		}

	clear();
	for (row_ctr = 0; row_ctr < DROWS; row_ctr++) 
		{
		move(row_ctr, 0);

		for (col_ctr = 0; col_ctr < DCOLS; col_ctr++) 
			{
			addcch(save[row_ctr][col_ctr]);
			}
		}

	redraw();	/*redraw screen*/
}




/*----------------------------------------------------------------*/
void known_items_print_known_items(void)
{
	unsigned int ctr;

	for (ctr = 0; ctr < number_of_known_items; ctr++)
	{
		if (known_items_list[ctr].is_known)
		{
			printf(known_items_list[ctr].printable_string);
			printf("\n");
		}
		}
}
#endif /* KNOWN_ITEMS */
