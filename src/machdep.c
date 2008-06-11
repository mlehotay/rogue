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
 * @(#)machdep.c 8.1 (Berkeley) 5/31/93
 */

/*
 * machdep.c
 *
 * This source herein may be modified and/or distributed by anybody who
 * so desires, with the following restrictions:
 *    1.)  No portion of this notice shall be removed.
 *    2.)  Credit shall not be taken for the creation of this source.
 *    3.)  This code is not to be traded, sold, or used for personal
 *         gain or profit.
 *
 */

/* Included in this file are all system dependent routines.  Extensive use
 * of #ifdef's will be used to compile the appropriate code on each system:
 *
 *    __WATCOMC__  Open Watcom
 *    _MSC_VER     Microsoft C
 *    __DJGPP__    djgpp version 2 and above
 *
 * To change a routine to include a new system, simply #ifdef the existing
 * routine, as in the following example:
 *
 *   To make a routine compatible with Turbo C, change the first function
 *  to the second:
 *
 *      md_function()
 *      {
 *         code;
 *      }
 *
 *      md_function()
 *      {
 *      #ifdef __BORLANDC__
 *         borlandcode;
 *      #else
 *         code;
 *      #endif
 *      }
 *
 * Appropriate variations of this are of course acceptible.
 * The use of "#elseif" is discouraged because of non-portability.
 * If the correct #define doesn't exist, "__BORLANDC__" in this case, make it
 * up and insert it in the list at the top of the file.  Alter the CFLAGS
 * in your Makefile appropriately.
 *
 */

#include <stdio.h>
#include <conio.h>
#include <time.h>
#include <io.h>
#include <fcntl.h> 
#include <signal.h>
#include <process.h>
#include "rogue.h"
#include "paths.h"
#include "version.h"

#ifdef _MSC_VER
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif


/* md_slurp:
 *
 * This routine throws away all keyboard input that has not
 * yet been read.  It is used to get rid of input that the user may have
 * typed-ahead.
 *
 * This function is not necessary, so it may be stubbed.  The might cause
 * message-line output to flash by because the game has continued to read
 * input without waiting for the user to read the message.  Not such a
 * big deal.
 */

void md_slurp(void)
{
#ifdef _MSC_VER
    FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));
#else
    while(kbhit())
        rgetchar();
#endif
}


/* md_control_keyboard():
 *
 * This routine is much like md_cbreak_no_echo_nonl() below.  It sets up
 * the keyboard for appropriate input.  Specifically, it prevents the tty
 * driver from stealing characters.  For example, ^Y is needed as a command
 * character, but the tty driver intercepts it for another purpose.  Any
 * such behavior should be stopped.  This routine could be avoided if
 * we used RAW mode instead of CBREAK.  But RAW mode does not allow the
 * generation of keyboard signals, which the program uses.
 *
 * The parameter 'mode' when true, indicates that the keyboard should
 * be set up to play rogue.  When false, it should be restored if
 * necessary.
 *
 * This routine is not strictly necessary and may be stubbed.  This may
 * cause certain command characters to be unavailable.
 */

void md_control_keybord(boolean mode)
{
    mode = mode; /* make compiler happy */
}


/* md_heed_signals():
 *
 * This routine tells the program to call particular routines when
 * certain interrupts/events occur:
 *
 *      SIGINT: call onintr() to interrupt fight with monster or long rest.
 *      SIGQUIT: call byebye() to check for game termination.
 *      SIGHUP: call error_save() to save game when terminal hangs up.
 *
 *		On VMS, SIGINT and SIGQUIT correspond to ^C and ^Y.
 *
 * This routine is not strictly necessary and can be stubbed.  This will
 * mean that the game cannot be interrupted properly with keyboard
 * input, this is not usually critical.
 */

void md_heed_signals(void)
{
	signal(SIGINT, onintr);
	signal(SIGTERM, byebye);
}


/* md_ignore_signals():
 *
 * This routine tells the program to completely ignore the events mentioned
 * in md_heed_signals() above.  The event handlers will later be turned on
 * by a future call to md_heed_signals(), so md_heed_signals() and
 * md_ignore_signals() need to work together.
 *
 * This function should be implemented or the user risks interrupting
 * critical sections of code, which could cause score file, or saved-game
 * file, corruption.
 */

void md_ignore_signals(void)
{
	signal(SIGINT, SIG_IGN);
	signal(SIGTERM, SIG_IGN);
}


/* md_get_file_id():
 *
 * This function returns an integer that uniquely identifies the specified
 * file.  It need not check for the file's existence.  In UNIX, the inode
 * number is used.
 *
 * This function is used to identify saved-game files.
 */

int md_get_file_id(char *fname)
{
    fname = fname; /* make compiler happy */
    return 0;
}


/* md_link_count():
 *
 * This routine returns the number of hard links to the specified file.
 *
 * This function is not strictly necessary.  On systems without hard links
 * this routine can be stubbed by just returning 1.
 */

int md_link_count(char *fname)
{
    fname = fname; /* make compiler happy */
    return 1;
}


/* md_gct(): (Get Current Time)
 *
 * This function returns the current year, month(1-12), day(1-31), hour(0-23),
 * minute(0-59), and second(0-59).  This is used for identifying the time
 * at which a game is saved.
 *
 * This function is not strictly necessary.  It can be stubbed by returning
 * zeros instead of the correct year, month, etc.  If your operating
 * system doesn't provide all of the time units requested here, then you
 * can provide only those that it does, and return zeros for the others.
 * If you cannot provide good time values, then users may be able to copy
 * saved-game files and play them.  
 */

void md_gct(rogue_time *rt_buf)
{
	rt_buf->year = 0;
	rt_buf->month = 0;
	rt_buf->day = 0;
	rt_buf->hour = 0;
	rt_buf->minute = 0;
	rt_buf->second = 0;
}


/* md_gfmt: (Get File Modification Time)
 *
 * This routine returns a file's date of last modification in the same format
 * as md_gct() above.
 *
 * This function is not strictly necessary.  It is used to see if saved-game
 * files have been modified since they were saved.  If you have stubbed the
 * routine md_gct() above by returning constant values, then you may do
 * exactly the same here.
 * Or if md_gct() is implemented correctly, but your system does not provide
 * file modification dates, you may return some date far in the past so
 * that the program will never know that a saved-game file being modified.  
 * You may also do this if you wish to be able to restore games from
 * saved-games that have been modified.
 */

void md_gfmt(char *fname, rogue_time *rt_buf)
{
    fname = fname; /* make compiler happy */
    
	rt_buf->year = 0;
	rt_buf->month = 0;
	rt_buf->day = 0;
	rt_buf->hour = 0;
	rt_buf->minute = 0;
	rt_buf->second = 0;
}

/* md_df: (Delete File)
 *
 * This function deletes the specified file, and returns true (1) if the
 * operation was successful.  This is used to delete saved-game files
 * after restoring games from them.
 *
 * Again, this function is not strictly necessary, and can be stubbed
 * by simply returning 1.  In this case, saved-game files will not be
 * deleted and can be replayed.
 */

boolean md_df(const char *fname)
{
#ifndef _MSC_VER
	if (unlink(fname)) 
#else
	if (_unlink(fname))		/* Microsoft C api equivalent*/
#endif

	{
		return(0);
	}
	return(1);
}

/* md_gln: (Get login name)
 *
 * This routine returns the login name of the user.  This string is
 * used mainly for identifying users in score files.
 *
 * A dummy string may be returned if you are unable to implement this
 * function, but then the score file would only have one name in it.
 */

char * md_gln(void)
{
    char *name;

    printf(VERSION_STRING "\n");
    printf(COPYRIGHT_LINE1 "\n");
    printf(COPYRIGHT_LINE2 "\n");

    printf("\n\nWho are you? ");
    fflush(stdout);

    name = md_malloc(MAX_OPT_LEN + 2);
	fgets(name, MAX_OPT_LEN, stdin);	    
    md_slurp(); /* in case user typed very long name */
    strip(name, 0);
    if(!name[0])
        strcpy(name, "Rodney");

    return name;
}

/* md_sleep:
 *
 * This routine causes the game to pause for the specified number of
 * seconds.
 *
 * This routine is not particularly necessary at all.  It is used for
 * delaying execution, which is useful to this program at some times.
 */

void md_sleep(int nsecs)
{
    nsecs = nsecs; /* make compiler happy */    
#if 0
    clock_t alarm = clock() + nsecs*CLK_TCK;

    while(clock()<alarm)
       ;
#endif
}

/* md_getenv()
 *
 * This routine gets certain values from the user's environment.  These
 * values are strings, and each string is identified by a name.  The names
 * of the values needed, and their use, is as follows:
 *
 *   ROGUEOPTS
 *     A string containing the various game options.  This need not be
 *     defined.
 *   HOME
 *     The user's home directory.  This is only used when the user specifies
 *     '~' as the first character of a saved-game file.  This string need
 *     not be defined.
 *   SHELL
 *     The user's favorite shell.  If not found, the value in the COMSPEC
 *     environment variable is used.
 *
 * If your system does not provide a means of searching for these values,
 * you will have to do it yourself.  None of the values above really need
 * to be defined and you can get by with simply always returning zero.
 * Returning zero indicates that their is no defined value for the given
 * string.
 */


char * md_getenv(const char *name)
{
	char *value;
	/*char *getenv();	 	already defined in stdlib.h */

	value = getenv(name);

	return(value);
}


/* md_malloc()
 *
 * This routine allocates, and returns a pointer to, the specified number
 * of bytes.  This routines absolutely MUST be implemented for your
 * particular system or the program will not run at all.  Return zero
 * when no more memory can be allocated.
 */

char * md_malloc(const int n)
{
	/* char *malloc();	already defined in included standard header file */
	char *t;

	t = malloc(n);
	return(t);
}


/* md_gseed() (Get Seed)
 *
 * This function returns a seed for the random number generator (RNG).  This
 * seed causes the RNG to begin generating numbers at some point in it's
 * sequence.  Without a random seed, the RNG will generate the same set
 * of numbers, and every game will start out exactly the same way.  A good
 * number to use is the process id, given by getpid() on most UNIX systems.
 *
 * You need to find some single random integer, such as:
 *   process id.
 *   current time (minutes + seconds) returned from md_gct(), if implemented.
 *   
 * It will not help to return "get_rand()" or "rand()" or the return value of
 * any pseudo-RNG.  If you don't have a random number, you can just return 1,
 * but this means your games will ALWAYS start the same way, and will play
 * exactly the same way given the same input.
 */

int md_gseed(void)
{
#ifndef _MSC_VER
	return time(NULL);
#else
	{
	time_t *tmp_null_ptr;
	tmp_null_ptr = NULL;
	return (int) time(tmp_null_ptr);
	}
#endif
}


/* md_exit():
 *
 * This function causes the program to discontinue execution and exit.
 * This function must be implemented or the program will continue to
 * hang when it should quit.
 */

void md_exit(int status)
{
	exit(status);
}


/* md_lock():
 *
 * This function is intended to give the user exclusive access to the score
 * file.  It does so by flock'ing the score file.  The full path name of the
 * score file should be defined for any particular site in paths.h.  The
 * constants _PATH_SCOREFILE defines this file name.
 *
 * When the parameter 'l' is non-zero (true), a lock is requested.  Otherwise
 * the lock is released.
 */

void md_lock(boolean l)
{
    l = l; /* make compiler happy */

#if 0
	static int fd;
	short tries;

	if (l) {
		if ((fd = open(_PATH_SCOREFILE, O_RDONLY, 0)) < 1) {
			message("cannot lock score file", 0);
			return;
		}
		for (tries = 0; tries < 5; tries++)
			if (!lock(fd, 0, filelength(fd)))
				return;
	} else {
		(void)unlock(fd, 0, filelength(fd));
		(void)close(fd);
	}
#endif
}


/* md_shell():
 *
 * This function spawns a shell for the user to use.  When this shell is
 * terminated, the game continues.  Since this program may often be run
 * setuid to gain access to privileged files, care is taken that the shell
 * is run with the user's REAL user id, and not the effective user id.
 * The effective user id is restored after the shell completes.
 */

void md_shell(const char *shell)
{
#ifndef _MSC_VER
	spawnl(P_WAIT, shell, shell, NULL);
#else
	(void) _spawnl(_P_WAIT, shell, shell, NULL);
#endif
}
