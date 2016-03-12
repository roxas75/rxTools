REM Copyright (C) 2015 The PASTA Team
REM
REM This program is free software; you can redistribute it and/or
REM modify it under the terms of the GNU General Public License
REM version 2 as published by the Free Software Foundation
REM
REM This program is distributed in the hope that it will be useful,
REM but WITHOUT ANY WARRANTY; without even the implied warranty of
REM MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
REM GNU General Public License for more details.
REM
REM You should have received a copy of the GNU General Public License
REM along with this program; if not, write to the Free Software
REM Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

@echo off

echo.
echo rxTools AIO Theme Tool v1.0.2 (rev.1)

convert | findstr /C:"Miscellaneous" 1>nul
if errorlevel 1 (
	echo ERROR! ImageMagick, is NOT installed or isn't in the PATH variable!
	echo http://www.imagemagick.org/script/binary-releases.php
	GOTO:EOF
)

if "%1"=="makebgr" goto :makebgr
if "%1"=="makebgr-all" goto :makebgr-all
if "%1"=="makepng-all" goto :makepng-all
if "%1"=="makeprev" goto :makeprev
if "%1"=="strippng-all" goto :strippng-all

echo This tool is meant for theme creators. Available commands:
echo.
echo      makebgr ^<file.png^>: converts a .png file to .bin (BGR). The output will
echo                          be in the same directory as the input file.
echo      makebgr-all: converts all .png files to .bin (BGR). If no directory is
echo                   specified, the current one will be used.
echo      makepng-all: converts all .bin (BGR) files to .png. If you are creating a
echo                   theme and you need a template, this is the first command you
echo                   should use. If no directory is specified, the current one
echo                   will be used.
echo      makeprev [gif delay]: creates a preview (animated GIFs and static PNGs) of your theme
echo                in the "Preview" folder. An Internet connection is recommended
echo                the first time to download the New Nintendo 3DS XL frame, which
echo                will be saved as "~/hero-new-3ds.png".
echo      strippng-all: removes unnecessary data from all PNG files.
echo.
echo This program is free software; you can redistribute it and/or
echo modify it under the terms of the GNU General Public License
echo version 2 as published by the Free Software Foundation
GOTO:EOF
:makebgr
if %2=="" (
	echo No input file, specified!
	GOTO:EOF
)
echo Input: "%~dpnx2"
echo Output: "%~dpn2.bin"
if exist "%~dpn2.bin" del /f "%~dpn2.bin"
if exist "%~dpn2.bin" (
	echo Failed to remove existing file "%~dpn2.bin"!
	GOTO:EOF
)
convert -rotate 90 "%~dpnx2" "%~dpn2.bin"
if exist "%~dpn2.bin" echo Conversion success.
GOTO:EOF
:makebgr-all
set cdorig=%CD%
if not "%2"=="" CD %2
set i=0
for %%f in (*.png) do call :makebgr-all_convert "%%f"
echo %i% file(s) processed.
CD "%cdorig%"
GOTO:EOF
:makebgr-all_convert
set /a i=%i%+1
convert -rotate 90 "%~nx1" bgr:"%~n1.bin"
GOTO:EOF
:makepng-all
set i=0
if not exist "TOP.bin" (
	echo Cannot find "TOP.bin"! Aborted.
	goto:EOF
)
if not exist "cfg0TOP.bin" (
	echo Cannot find "cfg0TOP.bin"! Aborted.
	goto:EOF
)
ren TOP.bin TOP.bi_
ren cfg0TOP.bin cfg0TOP.bi_
for /r %%f in (*.bin) do call :topng %%f 320
ren TOP.bi_ TOP.bin
ren cfg0TOP.bi_ cfg0TOP.bin
call :topng TOP.bin 400
call :topng cfg0TOP.bin 400
echo %i% file(s) processed.
GOTO:EOF
:topng
set /a i=%i%+1
convert -size 240x%2 -depth 8 bgr:%1 -size %2x240 -rotate 270 %~n1.png
GOTO:EOF
:makeprev
set delay=%2
if "%delay%" == "" set delay=100
if not exist "%USERPROFILE%\hero-new-3ds.png" (
	bitsadmin /transfer "3DS-Template" /download /priority high http://www.nintendo.com/images/page/3ds/what-is-3ds/hero-new-3ds.png "%USERPROFILE%\hero-new-3ds.png"
)
if not exist "%USERPROFILE%\hero-new-3ds.png" (
	echo Unable to locate "%USERPROFILE%\hero-new-3ds.png"! Continuing...
)
mkdir "Preview/Images/AIO" "Preview/Images/Menu" "Preview/Images/Advanced Options" "Preview/Images/Boot" "Preview/Images/Configuration" "Preview/Images/Decryption" "Preview/Images/Dumping" "Preview/Images/Injection" "Preview/Images/Dump Files" "Preview/Animations/AIO" "Preview/Animations/Menu" "Preview/Animations/Advanced Options" "Preview/Animations/Boot" "Preview/Animations/Configuration" "Preview/Animations/Decryption" "Preview/Animations/Dumping" "Preview/Animations/Injection" "Preview/Animations/Dump Files"
if exist TOP.png (
	for /l %%x in (0, 1, 6) do (
		call :makep1 TOP.png menu%%x.png
	)
	for /l %%x in (0, 1, 4) do (
		call :makep1 TOP.png adv%%x.png
	)
	for /l %%x in (0, 1, 5) do (
		call :makep1 TOP.png dec%%x.png
	)
	for /l %%x in (0, 1, 5) do (
		call :makep1 TOP.png fil%%x.png
	)
	for /l %%x in (0, 1, 3) do (
		call :makep1 TOP.png dmp%%x.png
	)
	for /l %%x in (0, 1, 1) do (
		call :makep1 TOP.png inj%%x.png
	)
	call :makep1 TOP.png boot.png
	call :makep1 TOP.png bootE.png
	call :makep1 TOP.png credits.png
	call :makep1 cfg0TOP.png cfg0.png
	call :makep1 cfg0TOP.png cfg1E.png
	call :makep1 cfg0TOP.png cfg1O.png
	call :makep1 TOP.png app.png
) else (
	echo "ERROR: Cannot find TOP.png."
)
convert Preview/menu0.png Preview/boot.png Preview/adv2.png +append -strip Preview/menuprev-aio.png
convert -delay %delay% -loop 0 menu0.png menu1.png menu2.png menu3.png menu4.png menu5.png menu6.png Preview/menuprev-0.gif
convert -delay %delay% -loop 0 Preview/menu0.png Preview/menu1.png Preview/menu2.png Preview/menu3.png Preview/menu4.png Preview/menu5.png Preview/menu6.png Preview/menuprev-1.gif
convert -delay %delay% -loop 0 boot.png bootE.png Preview/boot-0.gif
convert -delay %delay% -loop 0 Preview/boot.png Preview/bootE.png Preview/boot-1.gif
convert -delay %delay% -loop 0 adv0.png adv1.png adv2.png adv3.png adv4.png Preview/adv-0.gif
convert -delay %delay% -loop 0 Preview/adv0.png Preview/adv1.png Preview/adv2.png Preview/adv3.png Preview/adv4.png Preview/adv-1.gif
convert -delay %delay% -loop 0 dec0.png dec1.png dec2.png dec3.png dec4.png dec5.png Preview/dec-0.gif
convert -delay %delay% -loop 0 Preview/dec0.png Preview/dec1.png Preview/dec2.png Preview/dec3.png Preview/dec4.png Preview/dec5.png Preview/dec-1.gif
convert -delay %delay% -loop 0 dmp0.png dmp1.png dmp2.png dmp3.png Preview/dmp-0.gif
convert -delay %delay% -loop 0 Preview/dmp0.png Preview/dmp1.png Preview/dmp2.png Preview/dmp3.png Preview/dmp-1.gif
convert -delay %delay% -loop 0 fil0.png fil1.png fil2.png fil3.png fil4.png fil5.png Preview/fil-0.gif
convert -delay %delay% -loop 0 Preview/fil0.png Preview/fil1.png Preview/fil2.png Preview/fil3.png Preview/fil4.png Preview/fil5.png Preview/fil-1.gif
convert -delay %delay% -loop 0 inj0.png inj1.png Preview/inj-0.gif
convert -delay %delay% -loop 0 Preview/inj0.png Preview/inj1.png Preview/inj-1.gif
convert -delay %delay% -loop 0 cfg0.png cfg1E.png cfg1O.png Preview/cfg-0.gif
convert -delay %delay% -loop 0 Preview/cfg0.png Preview/cfg1E.png Preview/cfg1O.png Preview/cfg-1.gif
convert -delay %delay% -loop 0 menu0.png boot.png bootE.png menu1.png dec0.png dec1.png dec2.png dec3.png dec4.png dec5.png menu2.png dmp0.png dmp1.png dmp2.png dmp3.png menu3.png inj0.png inj1.png menu4.png adv0.png adv1.png adv2.png adv3.png adv4.png menu5.png app.png menu6.png credits.png Preview/menuprev-aio-0.gif
convert -delay %delay% -loop 0 Preview/menu0.png Preview/boot.png Preview/bootE.png Preview/menu1.png Preview/dec0.png Preview/dec1.png Preview/dec2.png Preview/dec3.png Preview/dec4.png Preview/dec5.png Preview/menu2.png Preview/dmp0.png Preview/dmp1.png Preview/dmp2.png Preview/fil0.png Preview/fil1.png Preview/fil2.png Preview/fil3.png Preview/fil4.png Preview/fil5.png Preview/dmp3.png Preview/menu3.png Preview/inj0.png Preview/inj1.png Preview/menu4.png Preview/adv0.png Preview/adv1.png Preview/adv2.png Preview/adv3.png Preview/adv4.png Preview/menu5.png Preview/app.png Preview/menu6.png Preview/credits.png Preview/menuprev-aio-1.gif
cd Preview
move menuprev-aio*.gif Animations/AIO
move menuprev-?.gif Animations/Menu
move boot*.gif Animations/Boot
move adv*.gif "Animations/Advanced Options"
move cfg*.gif Animations/Configuration
move dec*.gif Animations/Decryption
move dmp*.gif Animations/Dumping
move inj*.gif Animations/Injection
move fil*.gif "Animations/Dump Files"
move menuprev-aio.png Images/AIO
move menu*.png Images/Menu
move boot*.png Images/Boot
move adv*.png "Images/Advanced Options"
move cfg*.png Images/Configuration
move dec*.png Images/Decryption
move dmp*.png Images/Dumping
move inj*.png Images/Injection
move fil*.png "Images/Dump Files"
move app.png Images
move credits.png Images
cd..
GOTO:EOF
:strippng-all
set cdorig=%CD%
if not "%2"=="" CD %2
set i=0
for %%f in (*.png) do call :strippng-all_convert "%%f"
echo %i% file(s) processed.
CD "%cdorig%"
GOTO:EOF
:strippng-all_convert
set /a i=%i%+1
convert "%~nx1" -strip "%~nx1"
GOTO:EOF
:makep1
	if exist %2 (
		convert %1 -filter Lanczos -resize 264x158 Preview\temp1.png
		convert "%USERPROFILE%\hero-new-3ds.png" Preview\temp1.png -geometry +71+34 -composite Preview\temp2.png
		del Preview\temp1.png
		convert %2 -filter Lanczos -resize 213x160 Preview\temp1.png
		convert Preview\temp2.png Preview\temp1.png -geometry +96+240 -composite Preview\%2
		del Preview\temp1.png
		del Preview\temp2.png
	) else (
		echo "ERROR: Cannot find %2."
	)
GOTO:EOF
