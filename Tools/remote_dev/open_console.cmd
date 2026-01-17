#!/usr/bin/env -S 2>/dev/null=2>NUL sh
@goto batch 2>NUL;rm -f NUL

source $(dirname -- "$0")/open_console.sh
exit

:batch
@echo off
"%ProgramFiles%\Git\bin\bash.exe" "%~dp0\open_console.sh" %*
exit /B