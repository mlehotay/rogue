@echo off

rem batch file to prepare rogue clone for compilation under DOS or Windows
rem invoke from sys subdirectory, ex: setup wcc

if exist setup.bat goto compiler
echo Error: setup must be run from the sys directory
goto end

:compiler
if "%1"=="wcc" goto WATCOM
if "%1"=="djgpp" goto DJGPP
if "%1"=="msc" goto MICROSOFT
echo Usage: setup compiler
echo  supported compilers:
echo   wcc - Open Watcom
echo   djgpp - DJGPP
echo   msc - Microsoft C
goto end

:WATCOM
echo Setting up for Open Watcom...
set makefile=makefile.wcc
goto common

:DJGPP
echo Setting up for DJGPP...
set makefile=makefile.dj
goto common

:MICROSOFT
echo Setting up for Microsoft C...
set makefile=makefile.msc
goto common

:common
echo Copying Makefile...
copy %makefile% ..\Makefile
echo Creating directories...
md ..\o
md ..\exe
echo Done

:end
