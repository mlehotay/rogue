@echo off

rem batch file to prepare rogue clone for compilation under DOS
rem invoke from sys subdirectory, ex: setup wcc
rem todo: add support for Borland C, Digital Mars, Pacific C, Microsoft C

if exist setup.bat goto compiler
echo Error: setup must be run from the sys directory
goto end

:compiler
if "%1"=="wcc" goto WATCOM
if "%1"=="gcc" goto DJGPP
echo Usage: setup compiler
echo  supported compilers:
echo   wcc - Open Watcom
echo   gcc - DJGPP
goto end

:WATCOM
echo Setting up for Open Watcom...
set makefile=makefile.wcc
goto common

:DJGPP
echo Setting up for DJGPP...
set makefile=makefile.gcc
goto common

:common
echo Copying Makefile...
copy %makefile% ..\Makefile
echo Creating directories...
md ..\o
md ..\exe
echo Done

:end
