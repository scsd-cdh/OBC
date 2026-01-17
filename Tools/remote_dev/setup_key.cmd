#!/usr/bin/env -S 2>/dev/null=2>NUL sh
@goto batch 2>NUL;rm -f NUL

source $(dirname -- "$0")/setup_key.sh
exit

:batch
@echo off
"%ProgramFiles%\Git\bin\bash.exe" "%~dp0\setup_key.sh" %*
exit /B