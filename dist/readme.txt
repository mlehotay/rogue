DOS Rogue Clone version 2.0.1
=============================

This release of DOS Rogue Clone fixes the following two bugs:

 * Crashing after a monster gets killed by dragonfire (NetBSD PR bin/5806)
 * Wands of cold and wands of fire leave junk on the screen

Now that the wand effect works, you can see what is really happening when you
get five "the fire bounces" messages in a row. Those magical firebolts sure are
unpredictable!

By the way, the wizard's password is bathtub.


DOS Rogue Clone version 2.0
===========================

This version of DOS Rogue Clone is a new port of Tim Stoehr's Rogue 5.3 clone
Version III. The differences from the previous release of DOS Rogue Clone
(which was based on rogue clone II) are:

 * The ? (command help), / (whatis), o (options), and ! (spawn shell) commands
   have been added.
 * The passgo option has been added.
 * The ^P command now goes back 5 messages instead of just 1.
 * The screen capture command has been changed from X to &.
 * Wands of confuse monster and sleep have been removed from the game.
 * Wands of fire, cold, and drain life have been added.
 * Scrolls of confuse monster have been added.

Saved games from previous versions of DOS Rogue Clone will not work. You can
continue to use your old score file.

To get the latest version of DOS Rogue Clone, visit
http://post.queensu.ca/~lehotay/yendor.html on the web.

DOS Rogue Clone is free software. See the file LICENSE.TXT for copyright
information.

Email questions and bug reports to <lehotay@post.queensu.ca>.


Manifest
========

The following files are included in the DOS Rogue Clone v2.0 distribution:

File         Description
-----------  ---------------------------------------------------------------
ROGUE.EXE    DOS Rogue Clone executable program
README.TXT   The file you are currently reading
LICENSE.TXT  Copyright license
GUIDE.TXT    A Guide to the Dungeons of Doom
ROGUE.TXT    rogue man page
CLONE.TXT    rogue clone man page
CHANGES.TXT  Differences between the original rogue and UNIX rogue clone III
ROGUE.ICO    Rogue Clone icon (for Windows users)


Playing Instructions
====================

Full instructions for playing DOS Rogue Clone can be found in the files
GUIDE.TXT, ROGUE.TXT, and CLONE.TXT. Help can also be had from within rogue by
using the '?' and '/' commands.

In addition to the HJKLYUBN keys, the number keys on the numeric keypad may be
used to move around in DOS Rogue Clone. The SCROLL LOCK key will place you in
"Fast Play" mode. Experiment with different combinations of SCROLL LOCK and
SHIFT to see how this affects your movement.

When you save a game, your player name is used as the name of the save file.
Restore your game by giving the same player name when asked, "Who are you?"

If you don't like setting all your options every time you begin a new game, try
defining them in the ROGUEOPTS environment variable, like this:

   C:\ROGUE>set ROGUEOPTS=name:Blarg,fruit:grapefruit,passgo
   C:\ROGUE>rogue

This works best if you put it in a batch file.


Installation for Windows Users
==============================

Use a program such as WinZip or pkunzip to extract all the files from the zip
archive to any folder you like. "C:\Program Files\Rogue" is a good choice. You
may find it convenient to place a shortcut to Rogue.exe on your desktop or in
your Start Menu. If you don't know how to do this, you can look it up in
Windows Help by choosing Help from the Start Menu and looking for "shortcuts"
and "start menu" in the index. You should put "C:\Program Files\Rogue" (or the
folder where you put Rogue.exe) in the "Start in" or "Working" field of your
shortcut.

Start Rogue Clone by selecting your shortcut from the Start Menu, double
clicking on your shortcut on the desktop, or double clicking on Rogue.exe.

Windows 3.1 uses the phrase "program items" instead of shortcuts. If you need
help installing Rogue Clone under Windows 3.1, search for "creating program
items" in Program Manager Help.


Uninstallation for Windows Users
================================

To uninstall Rogue Clone, delete "C:\Program Files\Rogue" (or whatever folder
you created during installation) and any shortcuts you created. If you need
help with this, look up "deleting files" in Windows Help.

Windows 3.1 users should search for "deleting directories" in File Manager Help
and "deleting program items" in Program Manager Help.
