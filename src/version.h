#define VERSION_MAJOR 2
#define VERSION_MINOR 1
#define VERSION_PATCH 0

/*****************************************************************************/
/* Version 2.1.x */

/*
 *  Rogue Clone IV 2.1.0, February 22, 2004
 *  new Windows console application port
 *  DOS port now compiles with Open Watcom and DJGPP (abandoned Watcom 7.0)
 *  uses PC-Rogue high ASCII characters
 *  now supports color graphics throughout
 *  fixed bug in dragon's fire-breathing aim
 *  fixed several bugs pertaining to object naming under gcc
 */

/*****************************************************************************/
/* Version 2.0.x */

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
/* Version 1.0.x */

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

/*****************************************************************************/

#define STRINGIZE(x) #x
#define STR(x) STRINGIZE(x)

#define NAME_STRING "Rogue Clone IV"

#if VERSION_PATCH == 0
  #define PATCH_STRING ""
#else
  #define PATCH_STRING "." STR(VERSION_PATCH)
#endif

#ifdef RELEASE /* define this on the [n|w]make command line */
  #define REL_STRING ""
#else
  #define REL_STRING " (pre-release)"
#endif

#if defined(__DOS__) || defined(MSDOS)
  #if _M_IX86 >= 300 ||  defined(__i386__) || defined(__386__)
    #define PORT "32-bit DOS"
  #else
    #define PORT "16-bit DOS"
  #endif
#elif defined(_WIN32) || defined(__WINDOWS__)
  #define PORT "Windows"
#else
  #define PORT "???"
#endif

#define VERSION_STRING NAME_STRING " version " STR(VERSION_MAJOR) \
		"." STR(VERSION_MINOR) PATCH_STRING REL_STRING

#define LONG_VERSION_STRING NAME_STRING " v" STR(VERSION_MAJOR) "." \
		STR(VERSION_MINOR) PATCH_STRING " for " PORT REL_STRING \
		" built " __DATE__ " " __TIME__

#define COPYRIGHT_LINE1 \
    "Derived from Timothy C. Stoehr's Rogue 5.3 Clone version III"

#define COPYRIGHT_LINE2 \
    "Copyright (c) 1988, 1993 The Regents of the University of California"
