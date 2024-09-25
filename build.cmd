@echo off
setlocal

if not exist build (mkdir build)
pushd build

:: General Compiler Flags
set compiler=           -nologo   &:: Supress startup banner
set compiler=%compiler% -Oi       &:: Use assembly intrinsics where possible
set compiler=%compiler% -W4       &:: Display warnings up to lvl 4
set compiler=%compiler% -WX       &:: Treat all warnings as errors

:: Ignore Warnings
set compiler=%compiler% -wd4201   &:: Nameless struct/union
set compiler=%compiler% -wd4100   &:: Unused function parameter
set compiler=%compiler% -wd4189   &:: Local variable not referenced

:: Debug Variables

set debug=        -FC                             &:: Produce the full path of the source code file
set debug=%debug% -Z7                             &:: Produce debug information
set debug=%debug% -fsanitize=address

cl %compiler% %debug% ..\src\convdemo.c user32.lib
popd

