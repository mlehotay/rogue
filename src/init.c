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
 * @(#)init.c 8.1 (Berkeley) 5/31/93
 */

/*
 * init.c
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
 
char *login_name = (char *) 0;
char welcome[80];
char *rest_file = 0;
boolean cant_int = 0;
boolean did_int = 0;
boolean score_only;
boolean init_curses = 0;
boolean save_is_interactive = 1;
boolean ask_quit = 1;
boolean no_skull = 0;
boolean passgo = 0;
char *byebye_string = "Okay, bye bye!";

extern char *fruit;
extern short party_room;
extern boolean jump;

init(argc, argv)
int argc;
char *argv[];
{
	int seed;

	do_args(argc, argv);
	do_opts();

	if (!score_only && !rest_file) {
        FILE *fp;
        
	    if(!login_name)
	        login_name = md_gln();

        make_filename(&rest_file, login_name);
        if((fp=fopen(rest_file, "rb"))!=NULL)
            fclose(fp);
        else {
            free(rest_file);
            rest_file = NULL;
        }
	}
	    
	initscr();
	if ((LINES < DROWS) || (COLS < DCOLS)) {
		clean_up("must be played on 24 x 80 screen");
	}
	start_window();
	init_curses = 1;

	md_heed_signals();

	if (score_only) {
		put_scores((object *) 0, 0);
	}
	seed = md_gseed();
	(void) srrandom(seed);
	
	if (rest_file) {
		restore(rest_file);
	} else {
	    mix_colors();
	    get_wand_and_ring_materials();
       	make_scroll_titles();

   	    level_objects.next_object = (object *) 0;
       	level_monsters.next_monster = (object *) 0;
       	player_init();
       	ring_stats(0);
	}

  	sprintf(welcome, "Hello %s, welcome %sto Rogue!", login_name,
   	    (rest_file ? "back " : ""));

    return (rest_file!=NULL);
}

make_filename(char **fname, char *name) {
    char *p, *punct = "_^$~!#%&-{}()@'`";

    free(*fname);
	if ((*fname = md_malloc(13))==NULL) {
		clean_up("cannot alloc() memory");
	}

    p = strncpy(*fname, name, 8);
    *(p+8) = '\0';

    while(*p) {
        if(isalnum(*p) || strchr(punct, *p))
            *p = toupper(*p);
        else 
            *p = '_';
        p++;
    }
    strcat(*fname, ".SAV");
}

player_init()
{
	object *obj;

	rogue.pack.next_object = (object *) 0;

	obj = alloc_object();
	get_food(obj, 1);
	(void) add_to_pack(obj, &rogue.pack, 1);

	obj = alloc_object();		/* initial armor */
	obj->what_is = ARMOR;
	obj->which_kind = RINGMAIL;
	obj->class = RINGMAIL+2;
	obj->is_protected = 0;
	obj->d_enchant = 1;
	(void) add_to_pack(obj, &rogue.pack, 1);
	do_wear(obj);

	obj = alloc_object();		/* initial weapons */
	obj->what_is = WEAPON;
	obj->which_kind = MACE;
	obj->damage = "2d3";
	obj->hit_enchant = obj->d_enchant = 1;
	obj->identified = 1;
	(void) add_to_pack(obj, &rogue.pack, 1);
	do_wield(obj);

	obj = alloc_object();
	obj->what_is = WEAPON;
	obj->which_kind = BOW;
	obj->damage = "1d2";
	obj->hit_enchant = 1;
	obj->d_enchant = 0;
	obj->identified = 1;
	(void) add_to_pack(obj, &rogue.pack, 1);

	obj = alloc_object();
	obj->what_is = WEAPON;
	obj->which_kind = ARROW;
	obj->quantity = get_rand(25, 35);
	obj->damage = "1d2";
	obj->hit_enchant = 0;
	obj->d_enchant = 0;
	obj->identified = 1;
	(void) add_to_pack(obj, &rogue.pack, 1);
}

clean_up(estr)
char *estr;
{
	if (save_is_interactive) {
		if (init_curses) {
			move(DROWS-1, 0);
			refresh();
			stop_window();
		}
		printf("\n%s\n", estr);
	}
	md_exit(0);
}

start_window()
{
	crmode();
	noecho();
#ifndef BAD_NONL
	nonl();
#endif
	md_control_keybord(0);
}

stop_window()
{
	endwin();
	md_control_keybord(1);
}

void
byebye()
{
	md_ignore_signals();
	if (ask_quit) {
		quit(1);
	} else {
		clean_up(byebye_string);
	}
	md_heed_signals();
}

void
onintr()
{
	md_ignore_signals();
	if (cant_int) {
		did_int = 1;
	} else {
		check_message();
		message("interrupt", 1);
	}
	md_heed_signals();
}

void
error_save()
{
	save_is_interactive = 0;
	save_into_file(_PATH_ERRORFILE);
	clean_up("");
}

do_args(argc, argv)
int argc;
char *argv[];
{
	short i, j;

	for (i = 1; i < argc; i++) {
		if (argv[i][0] == '-') {
			for (j = 1; argv[i][j]; j++) {
				switch(argv[i][j]) {
				case 's':
					score_only = 1;
					break;
				}
			}
		} else {
			rest_file = argv[i];
		}
	}
}

do_opts()
{
	char *eptr;

	if (eptr = md_getenv("ROGUEOPTS")) {
		for (;;) {
			while ((*eptr) == ' ') {
				eptr++;
			}
			if (!(*eptr)) {
				break;
			}
			if (!strncmp(eptr, "fruit:", 6)) {
				eptr += 6;
				env_get_value(&fruit, eptr, 1);
			} else if (!strncmp(eptr, "jump", 4)) {
				jump = 1;
			} else if (!strncmp(eptr, "name:", 5)) {
				eptr += 5;
				env_get_value(&login_name, eptr, 0);
			} else if (!strncmp(eptr, "noskull", 7) ||
					!strncmp(eptr,"notomb", 6)) {
				no_skull = 1;
			} else if (!strncmp(eptr, "passgo", 5)) {
				passgo = 1;
			}
			while ((*eptr) && (*eptr != ',')) {
				eptr++;
			}
			if (!(*(eptr++))) {
				break;
			}
		}
	}
	
	/* If some strings have not been set through ROGUEOPTS, assign defaults
	 * to them so that the options editor has data to work with.
	 */
	init_str(&fruit, "slime-mold ");
}

void strip(char *s, boolean add_blank) {
    char *p = s;

    while(*p && isspace(*p))
        p++;
    if(p != s) {
        char *q = s;
        
        while(*p)
            *(q++) = *(p++);
        *q = '\0';        
    }

    p = s + strlen(s) - 1;
    while(!(p<s) && isspace(*p))
        p--;

    if(add_blank)
        *(++p) = ' ';

    *(++p) = '\0';
}

env_get_value(s, e, add_blank)
char **s, *e;
boolean add_blank;
{
	short i = 0;
	char *t;

	t = e;

	while ((*e) && (*e != ',')) {
		if (*e == ':') {
			*e = ';';		/* ':' reserved for score file purposes */
		}
		e++;
		if (++i >= MAX_OPT_LEN) {
			break;
		}
	}
	*s = md_malloc(MAX_OPT_LEN + 2);
	(void) strncpy(*s, t, i);
	(*s)[i] = '\0';

	strip(*s, add_blank);
}

init_str(str, dflt)
char **str, *dflt;
{
	if (!(*str)) {
		*str = md_malloc(MAX_OPT_LEN + 2);
		(void) strcpy(*str, dflt);
	}
}
