/*
	#define VERSION_MAJOR 2
	#define VERSION_MINOR 1
	#define VERSION_PATCH 0
	#define EXPERIMENTAL  0

	#define STRINGIZE(x) #x

	#if VERSION_PATCH == 0
	  #define PATCH_STRING
	#else
	  #define PATCH_STRING "." STRINGIZE(VERSION_PATCH)
	#endif

	#if EXPERIMENTAL == 0
	  #define DEV_STRING
	#else
	  #define DEV_STRING " (in development)"
	#endif


	#define VERSION_STRING "DOS Rogue Clone version " STRINGIZE(VERSION_MAJOR) \
						   "." STRINGIZE(VERSION_MINOR) PATCH_STRING DEV_STRING
	#define LONG_VERSION_STRING VERSION_STRING " built " __DATE__ " " __TIME__
*/

/* This eliminates all preprocessor headaches, especially as gcc seems
 * not to expand macro paramters before applying the stringizing (#)
 * operator.
 */

#define VERSION "2.1.0a"

#define VERSION_STRING 			"DOS Rogue Clone version " VERSION
#define LONG_VERSION_STRING 	VERSION_STRING " built " __DATE__ " " __TIME__



#define COPYRIGHT_LINE1 \
"Based on Timothy C. Stoehr's Rogue 5.3 Clone Version III"

#define COPYRIGHT_LINE2 \
"Copyright (c) 1988, 1993 The Regents of the University of California"


/*****************************************************************************/
/* Version 2.1.0  NS */

/*
 *  DOS Rogue Clone 2.1.0, June 14 2003
 *  now supports color graphics throughout.
 *  uses PC-Rogue DOS characters and tombstone.
 *  adapted to compile under gcc (and hopefully still under Watcom).
 *  more sophisticated command line/environment options
 *  fixed bug in dragon's fire-breathing aim.
 *	fixed several bugs pertaining to object naming under gcc.
 */

/*
 *  DOS Rogue Clone 2.1.0a, June 16 2003
 *	less garish floor color when using traditional ASCII characters
 *  added option for hypothermia (my son hates it :-))
 *  Now supports -? and -?? switches
 *	Save Screen always saves in ASCII characters.
 *  Objects appear on gray background in tunnels.
 */


/*****************************************************************************/
/* Version 2.0.1 */

/*
 *  DOS Rogue Clone 2.0.1, December 20, 2002
 *  fixed crashing after dragonfire kills a monster (NetBSD PR bin/5806)
 *  fixed broken wand of cold/fire standout effect
 */

/*
 *  DOS Rogue Clone 2.0a, December 2, 2002
 *  edited readme file to make less of a big deal about the copyright license
 *  removed extra stuff from license.txt
 *  no changes made to the program, just the documentation
 */

/*
 *  DOS Rogue Clone 2.0, December 1, 2002
 *  A new DOS port based on rogue clone version III
 *  /, ?, o, and ! commands now implemented
 *  passgo option now implemented
 *  ^p goes back 5 messages instead of just 1
 *  screenshot command changed from 'X' to '&'
 *  wands of confuse monster and sleep removed from game
 *  wands of fire, cold, and drain life added
 *  scroll of confuse monster added
 */

/*****************************************************************************/
/* Version 1.0.1 */

/*
 *  DOS Rogue Clone 1.0.1, November 23, 2002
 *  title screen and version command now give proper credit to Tim Stoehr
 *  changed the default player name to Rodney
 *  changed the wizard's password back to bathtub
 */

/*
 *  DOS Rogue Clone 1.0, December 19, 2001
 *  Initial release, based on rogue clone version II
 */
