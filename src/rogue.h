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
 * @(#)rogue.h 8.1 (Berkeley) 5/31/93
 */

/*
 * rogue.h
 *
 * This source herein may be modified and/or distributed by anybody who
 * so desires, with the following restrictions:
 *    1.)  This notice shall not be removed.
 *    2.)  Credit shall not be taken for the creation of this source.
 *    3.)  This code is not to be traded, sold, or used for personal
 *         gain or profit.
 */

#ifdef _MSC_VER
/* supress warnings about parameters and return values,
 * at least until we write proper prototypes */

/* OLD_CODE - show all warnings
#pragma warning(disable:4033 4716 4113)	
*/


/*add in standard include files for strlen, sprintf, etc.*/
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#endif

typedef unsigned char boolean;
typedef unsigned char byte;


#define NOTHING		((unsigned short)       0)
#define OBJECT		((unsigned short)      01)
#define MONSTER		((unsigned short)      02)
#define STAIRS		((unsigned short)      04)
#define HORWALL		((unsigned short)     010)
#define VERTWALL	((unsigned short)     020)
#define DOOR		((unsigned short)     040)
#define FLOOR		((unsigned short)    0100)
#define TUNNEL		((unsigned short)    0200)
#define TRAP		((unsigned short)    0400)
#define ULCORNER    ((unsigned short)   01000)
#define URCORNER    ((unsigned short)   02000)
#define LLCORNER    ((unsigned short)   04000)
#define LRCORNER    ((unsigned short)  010000)
#define TERRAINTYPES  14  /* don't count HIDDEN */

#define HIDDEN		((unsigned short) 0100000)
#define ANYWALL     (HORWALL | VERTWALL)
#define ANYCORNER   (ULCORNER | URCORNER | LLCORNER | LRCORNER)
#define ANYROOMSIDE (ANYWALL | ANYCORNER)

#define ARMOR		((unsigned short)   01)
#define WEAPON		((unsigned short)   02)
#define SCROL		((unsigned short)   04)
#define POTION		((unsigned short)  010)
#define GOLD		((unsigned short)  020)
#define FOOD		((unsigned short)  040)
#define WAND		((unsigned short) 0100)
#define RING		((unsigned short) 0200)
#define AMULET		((unsigned short) 0400)
#define ALL_OBJECTS	((unsigned short) 0777)
#define OBJECTTYPES  9


#define LEATHER 0
#define RINGMAIL 1
#define SCALE 2
#define CHAIN 3
#define BANDED 4
#define SPLINT 5
#define PLATE 6
#define ARMORS 7

#define BOW 0
#define DART 1
#define ARROW 2
#define DAGGER 3
#define SHURIKEN 4
#define MACE 5
#define LONG_SWORD 6
#define TWO_HANDED_SWORD 7
#define WEAPONS 8

#define MAX_PACK_COUNT 24

#define PROTECT_ARMOR 0
#define HOLD_MONSTER 1
#define ENCH_WEAPON 2
#define ENCH_ARMOR 3
#define IDENTIFY 4
#define TELEPORT 5
#define SLEEP 6
#define SCARE_MONSTER 7
#define REMOVE_CURSE 8
#define CREATE_MONSTER 9
#define AGGRAVATE_MONSTER 10
#define MAGIC_MAPPING 11
#define CON_MON 12
#define SCROLS 13

#define INCREASE_STRENGTH 0
#define RESTORE_STRENGTH 1
#define HEALING 2
#define EXTRA_HEALING 3
#define POISON 4
#define RAISE_LEVEL 5
#define BLINDNESS 6
#define HALLUCINATION 7
#define DETECT_MONSTER 8
#define DETECT_OBJECTS 9
#define CONFUSION 10
#define LEVITATION 11
#define HASTE_SELF 12
#define SEE_INVISIBLE 13
#define POTIONS 14

#define TELE_AWAY 0
#define SLOW_MONSTER 1
#define INVISIBILITY 2
#define POLYMORPH 3
#define HASTE_MONSTER 4
#define MAGIC_MISSILE 5
#define CANCELLATION 6
#define DO_NOTHING 7
#define DRAIN_LIFE 8
#define COLD 9
#define FIRE 10
#define WANDS 11

#define STEALTH 0
#define R_TELEPORT 1
#define REGENERATION 2
#define SLOW_DIGEST 3
#define ADD_STRENGTH 4
#define SUSTAIN_STRENGTH 5
#define DEXTERITY 6
#define ADORNMENT 7
#define R_SEE_INVISIBLE 8
#define MAINTAIN_ARMOR 9
#define SEARCHING 10
#define RINGS 11

#define RATION 0
#define FRUIT 1

#define NOT_USED		((unsigned short)   0)
#define BEING_WIELDED	((unsigned short)  01)
#define BEING_WORN		((unsigned short)  02)
#define ON_LEFT_HAND	((unsigned short)  04)
#define ON_RIGHT_HAND	((unsigned short) 010)
#define ON_EITHER_HAND	((unsigned short) 014)
#define BEING_USED		((unsigned short) 017)

#define NO_TRAP -1
#define TRAP_DOOR 0
#define BEAR_TRAP 1
#define TELE_TRAP 2
#define DART_TRAP 3
#define SLEEPING_GAS_TRAP 4
#define RUST_TRAP 5
#define TRAPS 6

#define STEALTH_FACTOR 3
#define R_TELE_PERCENT 8

#define UNIDENTIFIED ((unsigned short) 00)	/* MUST BE ZERO! */
#define IDENTIFIED ((unsigned short) 01)
#define CALLED ((unsigned short) 02)

#define DROWS 24
#define DCOLS 80
#define NMESSAGES 5
#define MAX_TITLE_LENGTH 30
#define MAXSYLLABLES 40
#define MAX_METAL 14
#define WAND_MATERIALS 30
#define GEMS 14

#define GOLD_PERCENT 46

#define MAX_OPT_LEN 40
#define MAX_OBJ_NAME 40


struct id {
	short value;
	char *title;
	char *real;
	unsigned short id_status;
};



/*   Support for color GUI...
 */
#define BLACK			0x00
#define BLUE			0x01
#define GREEN			0x02
#define CYAN			0x03
#define RED				0x04
#define MAGENTA			0x05
#define YELLOW			0x06
#define WHITE			0x07
#define GRAY			0x08
#define BRIGHT_BLUE		0x09
#define BRIGHT_GREEN	0x0A
#define BRIGHT_CYAN		0x0B
#define BRIGHT_RED		0x0C
#define BRIGHT_MAGENTA	0x0D
#define BRIGHT_YELLOW	0x0E
#define BRIGHT_WHITE	0x0F

union _colorChar {
	struct {
		byte color;
		char ch;
	} b8;
	unsigned short b16;
};
typedef union _colorChar color_char;

#define MAKE_COLOR(fg,bg)			(byte)(((fg) & 0x0F) | (((bg) & 0x0F) << 4))
#define MAKE_COLOR_CHAR(fg,bg,ch)	(unsigned short) \
				(((unsigned short) (ch) << 8) | MAKE_COLOR((fg),(bg)))

#define INVERT_COLOR(col)			(byte)( (((col) >> 4) & 0x0F) \
										  | (((col) << 4) & 0xF0) )
#define BGCOLOR_OF(col)				(byte)(((col) >> 4) & 0x0F)
#define FGCOLOR_OF(col)				(byte)((col) & 0x0F)



/* The following #defines provide more meaningful names for some of the
 * struct object fields that are used for monsters.  This, since each monster
 * and object (scrolls, potions, etc) are represented by a struct object.
 * Ideally, this should be handled by some kind of union structure.
 */

#define m_damage damage
#define hp_to_kill quantity
#define m_char ichar
#define first_level is_protected
#define last_level is_cursed
#define m_hit_chance class
#define stationary_damage identified
#define drop_percent which_kind
#define trail_char cchar
#define slowed_toggle quiver
#define moves_confused hit_enchant
#define nap_length picked_up
#define disguise dchar_ix
#define next_monster next_object

struct obj {				/* comment is monster meaning */
	unsigned long m_flags;	/* monster flags */
	char *damage;			/* damage it does */
	short quantity;			/* hit points to kill */
	short ichar;			/* 'A' is for aquatar */
	short kill_exp;			/* exp for killing it */
	short is_protected;		/* level starts */
	short is_cursed;		/* level ends */
	short class;			/* chance of hitting you */
	short identified;		/* 'F' damage, 1,2,3... */
	unsigned short which_kind; /* item carry/drop % */
	short o_row, o_col, o;	/* o is how many times stuck at o_row, o_col */
	short row, col;			/* current row, col */
	short d_enchant;
	short quiver;			/* monster slowed toggle */
	short trow, tcol;		/* target row, col */
	short hit_enchant;		/* how many moves is confused */
	unsigned short what_is;
	short picked_up;		/* sleep from wand of sleep */
	unsigned short in_use_flags;
	color_char cchar;		/* character underneath monster */
	int dchar_ix;	   		/* imitator's character index */
	struct obj *next_object;	/* next monster */
};

typedef struct obj object;

#define INIT_AW (object*)0,(object*)0
#define INIT_RINGS (object*)0,(object*)0
#define INIT_HPCUR 12
#define INIT_HPMAX 12
#define INIT_STRCUR 16
#define INIT_STRMAX 16
#define INIT_EXP 1,0
#define INIT_PACK {0}
#define INIT_GOLD 0
#define INIT_CHAR_COLOR MAKE_COLOR(BRIGHT_YELLOW, BLACK)
#define INIT_CHAR '@'
#define INIT_DOSCHAR '\x1'
#define INIT_MOVES 1250

struct fightr {
	object *armor;
	object *weapon;
	object *left_ring, *right_ring;
	short hp_current;
	short hp_max;
	short str_current;
	short str_max;
	object pack;
	long gold;
	short exp;
	long exp_points;
	short row, col;
	byte color;
	char fchar;
	char dosfchar;
	short moves_left;
};

typedef struct fightr fighter;

struct dr {
	short oth_room;
	short oth_row,
	      oth_col;
	short door_row,
		  door_col;
};

typedef struct dr door;

struct rm {
	short bottom_row, right_col, left_col, top_row;
	door doors[4];
	unsigned short is_room;
};

typedef struct rm room;

#define MAXROOMS 9
#define BIG_ROOM 10

#define NO_ROOM -1

#define PASSAGE -3		/* cur_room value */

#define AMULET_LEVEL 26

#define R_NOTHING	((unsigned short) 01)
#define R_ROOM		((unsigned short) 02)
#define R_MAZE		((unsigned short) 04)
#define R_DEADEND	((unsigned short) 010)
#define R_CROSS		((unsigned short) 020)

#define MAX_EXP_LEVEL 21
#define MAX_EXP 10000001L
#define MAX_GOLD 999999
#define MAX_ARMOR 99
#define MAX_HP 999
#define MAX_STRENGTH 99
#define LAST_DUNGEON 99

#define STAT_LEVEL 01
#define STAT_GOLD 02
#define STAT_HP 04
#define STAT_STRENGTH 010
#define STAT_ARMOR 020
#define STAT_EXP 040
#define STAT_HUNGER 0100
#define STAT_LABEL 0200
#define STAT_ALL 0377


/* #define PARTY_TIME 10 */	/* (NS - this is unused) one party somewhere in each 10 level span */
#define PARTY_PCT	8	/* NS - % of levels having a party room */
#define BIG_PARTY_PCT	1	/* NS - % of party rooms that are supersized */

#define MAX_TRAPS 10	/* maximum traps per level */

#define HIDE_PERCENT 12

#define FRUIT_TYPE "slime-mold"
/*#define FRUIT_TYPE "mango"*/	/* NS - more appetizing than slime molds */

struct tr {
	short trap_type;
	short trap_row, trap_col;
};

typedef struct tr trap;

extern fighter rogue;
extern room rooms[];
extern trap traps[];
extern unsigned short dungeon[DROWS][DCOLS];
extern object level_objects;

extern struct id id_scrolls[];
extern struct id id_potions[];
extern struct id id_wands[];
extern struct id id_rings[];
extern struct id id_weapons[];
extern struct id id_armors[];

extern object mon_tab[];
extern object level_monsters;

#define MONSTERS 26

#define HASTED					01L
#define SLOWED					02L
#define INVISIBLE				04L
#define ASLEEP				   010L
#define WAKENS				   020L
#define WANDERS				   040L
#define FLIES				  0100L
#define FLITS				  0200L
#define CAN_FLIT			  0400L		/* can, but usually doesn't, flit */
#define CONFUSED	 		 01000L
#define RUSTS				 02000L
#define HOLDS				 04000L
#define FREEZES				010000L
#define STEALS_GOLD			020000L
#define STEALS_ITEM			040000L
#define STINGS			   0100000L
#define DRAINS_LIFE		   0200000L
#define DROPS_LEVEL		   0400000L
#define SEEKS_GOLD		  01000000L
#define FREEZING_ROGUE	  02000000L
#define RUST_VANISHED	  04000000L
#define CONFUSES		 010000000L
#define IMITATES		 020000000L
#define FLAMES			 040000000L
#define STATIONARY		0100000000L		/* damage will be 1,2,3,... */
#define NAPPING			0200000000L		/* can't wake up for a while */
#define ALREADY_MOVED	0400000000L

#define SPECIAL_HIT		(RUSTS|HOLDS|FREEZES|STEALS_GOLD|STEALS_ITEM|STINGS|DRAINS_LIFE|DROPS_LEVEL)

#define WAKE_PERCENT 45
#define FLIT_PERCENT 40
#define PARTY_WAKE_PERCENT 75

#define HYPOTHERMIA 1
#define STARVATION 2
#define POISON_DART 3
#define QUIT 4
#define WIN 5
#define KFIRE 6

#define UPWARD 0
#define UPRIGHT 1
#define RIGHT 2
#define DOWNRIGHT 3
#define DOWN 4
#define DOWNLEFT 5
#define LEFT 6
#define UPLEFT 7
#define DIRS 8

#define ROW1 7
#define ROW2 15

#define COL1 26
#define COL2 52

#define MOVED 0
#define MOVE_FAILED -1
#define STOPPED_ON_SOMETHING -2
#define CANCEL '\033'
#define LIST '*'

#define HUNGRY 300
#define WEAK 150
#define FAINT 20
#define STARVE 0

#define RIP_TOMBSTONE	1	/* 0=White Skull, 1=Color Tombstone */

#define MIN_ROW 1

#ifdef KNOWN_ITEMS
#define MAX_KNOWN_ITEM_STRING_LENGTH 	60
#endif

/* external routine declarations.
 */

/* 
char *strcpy();
char *strncpy();
char *strcat();
*/

	/* char *mon_name();	OLD_CODE - declared in the monster.c prototype section below */
	/* char *get_ench_color();	OLD_CODE - declared in and only used in use.c */
	/* char *name_of();	OLD_CODE - declared in the object.c prototype section below */
	/* char *md_gln();	OLD_CODE - declared in the machdep.c prototype section below */
	/* char *md_getenv();	OLD_CODE - declared in the machdep.c prototype section below */
	/* char *md_malloc();	OLD_CODE - declared in the machdep.c prototype section below */
	/* boolean is_direction();	  OLD_CODE - see the move.c ANSI prototype section below */
	/* boolean mon_sees();	OLD_CODE - see the monster.c ANSI prototype section below */
	/* boolean mask_pack();  OLD_CODE - declared in and only used in pack.c */
	/* boolean mask_room();  OLD_CODE - declared in and only used in level.c */
	/* boolean check_hunger();	  OLD_CODE - see the move.c ANSI prototype section below */
	/* boolean reg_move();	  OLD_CODE - see the move.c ANSI prototype section below */
	/* boolean md_df();		OLD_CODE - declared in the machdep.c prototype section below */
	/* boolean has_been_touched();	OLD_CODE - declared in and only used in save.c	*/
	/* object *add_to_pack();	OLD_CODE - declared in the pack.c prototype section below */
	/* object *alloc_object();	OLD_CODE - declared in the object.c prototype section below */
	/* object *get_letter_object();		OLD_CODE - see object.c ANSI prototype below*/
	/* object *gr_monster();	OLD_CODE - declared in the monster.c prototype section below */
	/* object *get_thrown_at_monster();	OLD_CODE - declared in and only used in throw.c */
	/* object *get_zapped_monster(); OLD_CODE - declared in and only used in zap.c */
	/* object *check_duplicate();	OLD_CODE - declared in and only used in pack.c */
	/* object *gr_object();	OLD_CODE - declared in the object.c prototype section below */
	/* object *object_at();	OLD_CODE - see object.c ANSI prototype below*/
	/* object *pick_up();	OLD_CODE - see pack.c ANSI prototype below*/
	/* struct id *get_id_table();	OLD_CODE - declared in the invent.c prototype section below */
	/* unsigned short gr_what_is();	OLD_CODE - declared in the object.c prototype section below */
	/* long lget_number();		OLD_CODE - declared in the hit.c prototype section below */
	/* long xxx();	OLD_CODE - see score.c ANSI prototype below*/
	/* void byebye(), onintr(), error_save();	OLD_CODE - see init.c ANSI prototype below*/
	/* color_char mvincch(short row, short col);	OLD_CODE - see curses.c ANSI prototype below*/
	/* color_char get_mask_char(unsigned short mask);	OLD_CODE - see room.c ANSI prototype below*/
	/* color_char get_dungeon_char();	OLD_CODE - see room.c ANSI prototype below*/
	/* color_char get_terrain_char();	OLD_CODE - see room.c ANSI prototype below*/
	/* color_char get_rogue_char();	OLD_CODE - see room.c ANSI prototype below*/
	/* color_char gmc_row_col();  OLD_CODE - see the monster.c ANSI prototype section below */
	/* color_char gr_obj_char(int ix); OLD_CODE - see the room.c ANSI prototype section below */
	/* color_char gmc();  OLD_CODE - see the monster.c ANSI prototype section below */
	/* int gr_obj_index(); OLD_CODE - see the room.c ANSI prototype section below */



struct rogue_time_struct_definition {
	short year;		/* >= 1987 */
	short month;	/* 1 - 12 */
	short day;		/* 1 - 31 */
	short hour;		/* 0 - 23 */
	short minute;	/* 0 - 59 */
	short second;	/* 0 - 59 */
};

typedef struct rogue_time_struct_definition rogue_time;	/* fixes many compiler wanings */

extern int LINES, COLS;


	/* curses.c */
char rgetchar(void);
void initscr(void);
void move(const short row, const short col);
void addstr(char *str);
void mvaddstr(const short row, const short col, char *str);
void mvaddstr_in_color(const short row, const short col, char *str, const byte color);
void mvaddcstr(const short row, const short col, color_char *cstr);
void addch(const int ch);
void mvaddch(const short row, const short col, const int ch);
void addcch(const color_char cc);
void mvaddcch(const short row, const short col, const color_char cc);
void colorize(char *str, const byte color, color_char *cstr);
void refresh(void);
void redraw(void);
int mvinch(const short row, const short col);
color_char mvincch(const short row, const short col);
void clear(void);
void clrtoeol(void);
void standout(void);
void standend(void);
void draw_box(const color_char cset[6], const short ulrow, const short ulcol, const short height, const short width);
void endwin(void);
void crmode(void);
void noecho(void);
void nonl(void);


	/* hit.c */
void mon_hit(object *monster);
void rogue_hit(object *monster, const boolean force_hit);
void rogue_damage(const short d, object *monster, const short other);
short get_damage(char *ds, const boolean r);
int get_number(const char *s);
long lget_number(const char *s);
int mon_damage(object *monster, const short damage);
void fight(const boolean to_the_death);
void get_dir_rc(const short dir, short *row, short *col, const short allow_off_screen);
short get_hit_chance(const object *weapon);
short get_weapon_damage(const object *weapon);
void s_con_mon(object *monster);


	/* init.c */
int init(const int argc, char *argv[]);
void make_filename(char **fname, const char *name);
void clean_up(const char *estr);
void start_window(void);
void stop_window(void);
void error_save(void);
void strip(char *s, const boolean add_blank);
#ifdef _MSC_VER
void onintr(int foo);
void byebye(int foo);
#else
void onintr(void);
void byebye(void);
#endif


	/* invent.c */
#ifdef KNOWN_ITEMS
extern char *press_space;
#endif
void inventory(object *pack, const unsigned short mask);
void id_com(void);
void mix_colors(void);
void make_scroll_titles(void);
void get_desc(object *obj, char *desc);
void get_wand_and_ring_materials(void);
void single_inv(const short ichar);
struct id * get_id_table(const object *obj);
void inv_armor_weapon(const boolean is_weapon);
void id_type(void);

	/* level.c */
void make_level(void);
void clear_level(void);
void put_player(const short nr);
int drop_check(void);
int check_up(void);
void add_exp(const int e, const boolean promotion);
int hp_raise(void);
void show_average_hp(void);


	/* machdep.c */
void md_slurp(void);
void md_control_keybord(boolean mode);
void md_heed_signals(void);
void md_ignore_signals(void);
int md_get_file_id(char *fname);
int md_link_count(char *fname);
void md_gct(rogue_time *rt_buf);
void md_gfmt(char *fname, rogue_time *rt_buf);
boolean md_df(const char *fname);
char * md_gln(void);
void md_sleep(const int nsecs);
char * md_getenv(const char *name);
char * md_malloc(const int n);
int md_gseed(void);
void md_exit(int status);
void md_lock(boolean l);
void md_shell(const char *shell);


	/* message.c */
#ifdef KNOWN_ITEMS
extern char *more;
#endif
void message(char *msg, const boolean intrpt);
void remessage(short c);
void check_message(void);
short get_input_line(char *prompt, char *insert, char *buf, char *if_cancelled, const boolean add_blank, const boolean do_echo);
void print_stats(const int stat_mask);
void save_screen(void);
void sound_bell(void);
boolean is_digit(const short ch);
int r_index(const char *str, const int ch, const boolean last);

	/* monster.c */
void put_mons(void);
object * gr_monster(object *monster, int mn);
void mv_mons(void);
void party_monsters(const int rn, int n);
color_char gmc_row_col(const short row, const short col);
color_char gmc(const object *monster);
void mv_1_monster(object *monster, short row, short col);
void move_mon_to(object *monster, const short row, const short col);
short mon_can_go(const object *monster, const short row, const short col);
void wake_up(object *monster);
void wake_room(const short rn, const boolean entering, const short row, const short col);
char * mon_name(const object *monster);
void wanderer(void);
void show_monsters(void);
void create_monster(void);
int rogue_can_see(const short row, const short col);
void aggravate(void);
boolean mon_sees(const object *monster, const short row, const short col);
void mv_aquatars(void);
#ifdef KNOWN_ITEMS
void known_monsters_initialize(void);
void known_monsters_add_killed_monster(const char *monster_name);
void known_monsters_print_known_monsters(void);
#endif

	/* move.c */
short one_move_rogue(short dirch, const short pickup);
void multiple_move_rogue(const short dirch);
int is_passable(const short row, const short col);
int can_move(const short row1, const short col1, const short row2, const short col2);
void move_onto(void);
boolean is_direction(const short c, short *d);
boolean check_hunger(const boolean msg_only);
boolean reg_move(void);
void rest(const int count);


	/* object.c */
void put_objects(void);
void place_at(object *obj, const short row, const short col);
object * object_at(object *pack, const short row, const short  col);
object * get_letter_object(const short ch);
void free_stuff(object *objlist);
char * name_of(const object *obj);
object * gr_object(void);
unsigned short gr_what_is(void);
void get_food(object *obj, const boolean force_ration);
void put_stairs(void);
int get_armor_class(const object *obj);
object * alloc_object(void);
void free_object(object *obj);
void show_objects(void);
void put_amulet(void);
void c_object_for_wizard(void);
#ifdef DISCOVERY
	void discovery(void);
#endif
#ifdef KNOWN_ITEMS
void known_items_initialize(void);
void known_items_add_new_known_item(const unsigned short tmp_known_item_what_type,
									const unsigned short item_type);
void do_show_items_known(void);
void known_items_print_known_items(void);
#endif

	/* pack.c */
object * add_to_pack(object *obj, object *pack, const int condense);
void take_from_pack(const object *obj, object *pack);
object * pick_up(const short row, const short col, short *status);
void drop(void);
void wait_for_ack(void);
short pack_letter(char *prompt, unsigned short mask);
void take_off(void);
void wear(void);
void unwear(object *obj);
void do_wear(object *obj);
void wield(void);
void do_wield(object *obj);
void unwield(object *obj);
void call_it(void);
short pack_count(object *new_obj);
boolean has_amulet(void);
void kick_into_pack(void);


	/* play.c */
void play_level(void);


	/* random.c */
void srrandom(const int x);
long rrandom();
int get_rand(int x, int y);
int rand_percent(const int percentage);
int coin_toss(void);


	/* ring.c */
void put_on_ring(void);
void do_put_on(object *ring, const boolean on_left);
void remove_ring(void);
void un_put_on(object *ring);
void gr_ring(object *ring, const boolean assign_wk);
void inv_rings(void);
void ring_stats(const boolean pr);


	/* room.c */
void light_up_room(const int rn);
void light_passage(const short row, const short col);
void darken_room(const short rn);
color_char get_terrain_char(const unsigned short mask);
color_char get_dungeon_char(const short row, const short col);
color_char get_mask_char(const unsigned short mask);
color_char get_rogue_char(void);
void regenerate_screen(void);
void gr_row_col(short *row, short *col, const unsigned short mask);
short gr_room(void);
short party_objects(const short rn);
short get_room_number(const short row, const short col);
int is_all_connected(void);
void draw_magic_map(void);
void dr_course(object *monster, const boolean entering, short row, short col);
void edit_opts(void);
void do_shell(void);
color_char gr_obj_char(const int ix);
int gr_obj_index(void);


	/* save.c */
void save_game(void);
void save_into_file(char *sfile);
void restore(char *fname);


	/* score.c */
void killed_by(const object *monster, const short other);
void win(void);
void quit(const boolean from_intrpt);
void put_scores(const object *monster, const short other);
int is_vowel(const short ch);
int get_value(const object *obj);
void xxxx(char *buf, const short n);
long xxx(const boolean st);


	/* spec_hit.c */
void special_hit(object *monster);
void rust(object *monster);
void cough_up(const object *monster);
int seek_gold(object *monster);
void check_gold_seeker(object *monster);
int check_imitator(object *monster);
int imitating(const short row, const short col);
void drain_life(void);
int m_confuse(object *monster);
int flame_broil(object *monster);


	/* throw.c */
void throw(void);
void rand_around(const short i, short *r, short *c);


	/* trap.c */
void trap_player(const short row, const short col);
void add_traps(void);
void id_trap(void);
void show_traps(void);
void search(const short n, const boolean is_auto);

	/* use.c */
void quaff(void);
void read_scroll(void);
void vanish(object *obj, const short rm, object *pack);
void eat(void);
void tele(void);
void hallucinate(void);
void unhallucinate(void);
void unblind(void);
void relight(void);
void take_a_nap(void);
void cnfs(void);
void unconfuse(void);


	/*zap.c*/
void zapp(void);
void wizardize(void);
void bounce(const short ball, const short dir, short row, short col, short r);


