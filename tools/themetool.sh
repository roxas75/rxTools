#!/bin/sh
# Copyright (C) 2015 The PASTA Team
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# version 2 as published by the Free Software Foundation
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

echo "rxTools AIO Theme Tool v1.0.1 rev.1"
command -v convert >/dev/null 2>&1 || { echo "Please install ImageMagick in order to use this script." ; exit 1; }
if [ "$1" = "help" ]; then
	echo "This tool is meant for theme creators. Available commands:"
	echo "     makebgr <file.png>: converts a .png file to .bin (BGR)."
	echo "     makebgr-all: converts all .png files to .bin (BGR)."
	echo "     makepng-all: converts all .bin (BGR) files to .png. If you are creating a theme and you need a template, this is the first command you should use."
	echo "     makeprev [gif-delay]: creates a preview (animated GIFs and static PNGs) of your theme in the \"Preview\" folder. An Internet connection is recommended the first time to download the New Nintendo 3DS XL frame, which will be saved as \"~/hero-new-3ds.png\"."
	echo "     strippng-all: removes unnecessary data from all PNG files."
	echo ""
	echo "This program is free software; you can redistribute it and/or"
	echo "modify it under the terms of the GNU General Public License"
	echo "version 2 as published by the Free Software Foundation"
elif [ "$1" = "makebgr" ]; then
	convert -rotate 90 "$2" "bgr:${2%.*}.bin"
elif [ "$1" = "makebgr-all" ]; then
	for f in *.png
	do
		convert -rotate 90 "$f" "bgr:${f%.*}.bin"
	done
elif [ "$1" = "makepng-all" ]; then
	for f in *.bin
	do
		if [ "$f" = "TOP.bin" -o "$f" = "cfg0TOP.bin" ]; then
			continue;
		fi
		convert -size 240x320 -depth 8 "bgr:$f" -size 320x240 -rotate 270 -strip "${f%.*}.png"
	done
	convert -size 240x400 -depth 8 "bgr:TOP.bin" -size 400x240 -rotate 270 -strip "TOP.png"
	convert -size 240x400 -depth 8 "bgr:cfg0TOP.bin" -size 400x240 -rotate 270 -strip "cfg0TOP.png"
elif [ "$1" = "makeprev" ]; then
		if [ $# -eq 1 ]; then
		delay="100"
	else
		delay=$2
	fi

	if [ ! -f ~/hero-new-3ds.png ]; then
		(cd ~ ; curl -O "http://www.nintendo.com/images/page/3ds/what-is-3ds/hero-new-3ds.png")
	fi

	for w in Images Animations; do
		mkdir -p "Preview/$w/AIO" "Preview/$w/Menu" "Preview/$w/Advanced Options" "Preview/$w/Boot" "Preview/$w/Configuration" "Preview/$w/Decryption" "Preview/$w/Dumping" "Preview/$w/Injection" "Preview/$w/Dump Files"
	done

	makep1()
	{
		if [ -f $2 ]; then
			convert $1 -filter Lanczos -resize 264x158 -strip Preview/temp1.png
			convert ~/hero-new-3ds.png Preview/temp1.png -geometry +71+34 -composite -strip Preview/temp2.png
			rm Preview/temp1.png
			convert $2 -filter Lanczos -resize 213x160 -strip Preview/temp1.png
			convert Preview/temp2.png Preview/temp1.png -geometry +96+240 -composite -strip Preview/$2
			rm Preview/temp1.png Preview/temp2.png
		else
			echo "ERROR: Cannot find $2."
		fi
	}

	if [ -f TOP.png ]; then
		for i in `seq 0 6`; do
			makep1 TOP.png menu$i.png
		done

		for i in `seq 0 4`; do
			makep1 TOP.png adv$i.png
		done

		for i in `seq 0 5`; do
			makep1 TOP.png dec$i.png
		done

		for i in `seq 0 5`; do
			makep1 TOP.png fil$i.png
		done

		for i in `seq 0 3`; do
			makep1 TOP.png dmp$i.png
		done

		for i in `seq 0 1`; do
			makep1 TOP.png inj$i.png
		done

		makep1 TOP.png boot.png
		makep1 TOP.png bootE.png
		makep1 TOP.png credits.png
		makep1 cfg0TOP.png cfg0.png
		makep1 cfg0TOP.png cfg1E.png
		makep1 cfg0TOP.png cfg1O.png
		makep1 TOP.png app.png
	else
		echo "ERROR: Cannot find TOP.png."
	fi

	convert Preview/menu0.png Preview/boot.png Preview/adv2.png +append -strip Preview/menuprev-aio.png
	convert -delay $delay -loop 0 menu?.png Preview/menuprev-0.gif
	convert -delay $delay -loop 0 Preview/menu?.png Preview/menuprev-1.gif
	convert -delay $delay -loop 0 boot.png bootE.png Preview/boot-0.gif
	convert -delay $delay -loop 0 Preview/boot.png Preview/bootE.png Preview/boot-1.gif
	convert -delay $delay -loop 0 adv?.png Preview/adv-0.gif
	convert -delay $delay -loop 0 Preview/adv?.png Preview/adv-1.gif
	convert -delay $delay -loop 0 dec?.png Preview/dec-0.gif
	convert -delay $delay -loop 0 Preview/dec?.png Preview/dec-1.gif
	convert -delay $delay -loop 0 dmp?.png Preview/dmp-0.gif
	convert -delay $delay -loop 0 Preview/dmp?.png Preview/dmp-1.gif
	convert -delay $delay -loop 0 fil?.png Preview/fil-0.gif
	convert -delay $delay -loop 0 Preview/fil?.png Preview/fil-1.gif
	convert -delay $delay -loop 0 inj?.png Preview/inj-0.gif
	convert -delay $delay -loop 0 Preview/inj?.png Preview/inj-1.gif
	convert -delay $delay -loop 0 cfg0.png cfg1?.png Preview/cfg-0.gif
	convert -delay $delay -loop 0 Preview/cfg0.png Preview/cfg1?.png Preview/cfg-1.gif
	convert -delay $delay -loop 0 menu0.png boot.png bootE.png menu1.png dec?.png menu2.png dmp?.png fil?.png menu3.png inj?.png menu4.png adv?.png menu5.png app.png menu6.png credits.png Preview/menuprev-aio-0.gif
	convert -delay $delay -loop 0 Preview/menu0.png Preview/boot.png Preview/bootE.png Preview/menu1.png Preview/dec?.png Preview/menu2.png Preview/dmp?.png Preview/fil?.png Preview/menu3.png Preview/inj?.png Preview/menu4.png Preview/adv?.png Preview/menu5.png Preview/app.png Preview/menu6.png Preview/credits.png Preview/menuprev-aio-1.gif

	cd Preview

	mv menuprev-aio*.gif Animations/AIO
	mv menuprev-?.gif Animations/Menu
	mv boot*.gif Animations/Boot
	mv adv*.gif Animations/Advanced\ Options
	mv cfg*.gif Animations/Configuration
	mv dec*.gif Animations/Decryption
	mv dmp*.gif Animations/Dumping
	mv inj*.gif Animations/Injection
	mv fil*.gif Animations/Dump\ Files

	mv menuprev-aio.png Images/AIO
	mv menu*.png Images/Menu
	mv boot*.png Images/Boot
	mv adv*.png Images/Advanced\ Options
	mv cfg*.png Images/Configuration
	mv dec*.png Images/Decryption
	mv dmp*.png Images/Dumping
	mv inj*.png Images/Injection
	mv fil*.png Images/Dump\ Files
	mv app.png credits.png Images
elif [ "$1" = "strippng-all" ]; then
	for f in *.png
	do
		convert $f -strip $f
	done
else
	echo "Usage:"
	echo "sh $0 help"
	echo "sh $0 makebgr <file.png>"
	echo "sh $0 makebgr-all"
	echo "sh $0 makepng-all"
	echo "sh $0 makeprev [delay between frames]"
	echo "sh $0 strippng-all"
fi
