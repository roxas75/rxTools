# rxTools - Roxas75 3DS Toolkit

[![Join the chat at https://gitter.im/roxas75/rxTools](https://badges.gitter.im/Join%20Chat.svg)](https://gitter.im/roxas75/rxTools?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)

rxTools is a collection of hacking tools for Nintendo 3DS/3DSXL/2DS/NEW3DS/NEW3DSXL, compatible with all the system versions from 4.1 to 9.2.

rxMode is a free custom firmware which works through rxTools, not meant to replace any existing software, but just my personal work made public and easy for the end-user.

It was not meant, at the beginning, to be open-source, so you can expect some messed up code here.

We do not support piracy in any way, all the data and information in my software have already been made public.

## Current functionality
### General
- Support for Web Browser (SPIDER) entrypoint for Old3DS (ver 4.1-9.2)
- Support for System Settings (MSET) entrypoint for Old3DS (ver 4.1-4.5, 6.0-6.4)
- Support for System Settings (MSET) downgrading for Old3DS (ver 5.0-9.2 with MSET 4.x and 7.0-9.2 with MSET 6.x)
- Support for System Settings (MSET) entrypoint for New3DS (ver 9.0-9.2 with MSET 4.x)
- Support for Ninjhax (see below)
- Hacking tools collection

### rxMode
- Support for the latest emuNAND version on Old3DS
- Support for emuNAND on New3DS up to ver 9.5
- Support for sysNAND
- Access to the eShop and to online playing (requires an updated emuNAND)
- Support for 100% of the games (newer ones will require emuNAND)
- Signatures Checks disabled, which should allow just homebrew, but the world is cruel...
- Support for installing FBI (a CIA Manager)
- Support for AGB (GBA Virtual Console)
- Support for TWL (DS/DSi cartridges and DSiWare)
- Dynamic RAM dumping (just for debug purposes)

## Nightlies
Nightly builds of rxTools are a thing, again! :D https://lavanoid.github.io/

## How to build
Requires ImageMagick >= 6.8.9 - If you are using Ubuntu 14.04 or below, do not use the version in the official repositories, as it is outdated

Requirements for Windows:
- git, clone this repository with the following command: *git clone --recursive https://github.com/roxas75/rxTools.git*
- [Python 2.7](https://www.python.org)
- [devkitPRO](http://sourceforge.net/projects/devkitpro) with devkitARM option and suboptions installed (in case make.exe fails with stack trace dump, install [GnuWin Make](http://gnuwin32.sourceforge.net) and place it in path before devkitPRO/msys/bin)
- GCC (for Windows [MinGW](http://sourceforge.net/projects/mingw) with mingw32-base will be fine)
- [ImageMagick](http://www.imagemagick.org)

Requirements for GNU/Linux:
- [git](https://apps.ubuntu.com/cat/applications/git/), clone this repository with the following command: *git  clone --recursive https://github.com/roxas75/rxTools.git*
- [devkitPRO and devkitARM](http://3dbrew.org/wiki/Setting_up_Development_Environment) (follow the tutorial for Linux)
- [ImageMagick](http://www.imagemagick.org) (on Debian/Ubuntu, [install it using apt](https://apps.ubuntu.com/cat/applications/imagemagick/))

Just run **make release** and rxTools directory should be generated in the **release** folder, along with all the other packets. Move firm directory (run tools/cdn_firm.py to download it) to release/rxTools, then copy the contents of the release/rxTools folder to the root of your SD card.  

For New3DS users : Download 9.5 **encrypted** NATIVE_FIRM (0004013820000002.bin) from the web, along with 0004013820000002/cetk. Change the name of cetk to 0004013820000002_cetk.bin and copy 2 files to rxTools/firm.

If you get the *make: Interrupt/Exception caught (code = 0xc00000fd, addr = 0x4227d3)* error on Windows, look at your PATH and move any path with parenthesis to the end of it.

Make sure that the path to rxTools doesn't include space (' ').

## How to launch with Spider
rxTools *now* stores its code.bin in rxTools/sys/code.bin (formerly /rxTools.dat). To launch rxTools using the Spider exploit visit https://dukesrg.github.io/?rxTools/sys/code.bin using the 3DS Internet Browser. (Requires ver 9.2 or older).

## How to install DS Profile (MSET) Exploit (requires MSET 6.x)
Upon building, there will be a folder called release/mset that contains the files rxinstaller.nds and rxinstaller.bin. Using a supported NDS flash cart, launch rxinstaller.nds to install the MSET exploit. Otherwise, on firmwares 9.2 or older visit https://dukesrg.github.io/?mset/rxinstaller.bin to install the MSET exploit via the Spider exploit. If it worked, you'll see the bottom screen flash, and then the browser will appear to continue functioning normally. Press the home button, and after you see the Home Menu splash screen your DS will say "an error occurred" and ask you to reboot, but it's already done. You can then launch rxTools by going to Settings -> Profile -> DS Profile Settings.

## How to launch with Ninjhax-like *Hax (requires <= ver 9.2)
Use 3rd party loader : CtrBootMananger(http://gbatemp.net/threads/ctrbootmanager-3ds-boot-manager-loader-homemenuhax.398383/) or BootCtr(https://gbatemp.net/threads/re-release-bootctr-a-simple-boot-manager-for-3ds.401630/)

## Credits
- All the documentation on http://3dbrew.org, and to all the devs who contributed
- Roxas75, as the creator of rxTools
- Gateway team, for their work
- Archshift, who wrote some useful libs I used: https://github.com/archshift
- mid-kid and b1l1s, who developed CakeHax: https://github.com/mid-kid/CakeHax
- 173210 for New3DS support
- Reisyukaku for New3DS MSET support
- Many GBATemp users, who helped in the testing process
- Many other people who helped me in private
- All the dudes who reversed and fucked up my project, who wasted their time on making useless keeping the sources private


##License
###rxTools
Copyright (C) 2015 The PASTA Team

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
version 2 as published by the Free Software Foundation

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

See LICENSE for the details.

###CakeHax
The Source Code Form is subject to the terms of the Mozilla Public
License, v. 2.0.

See CakeHax/LICENSE.txt for the details.

###CakesROP
CakesROP is licensed by zoogie, mid-kid, bilis, and Reisyukaku
under the BSD 2-clause license.

See CakesROP/LICENSE for the details.

###PolarSSL
Copyright (C) 2006-2010, Brainspark B.V.

The program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
version 2 as published by the Free Software Foundation

The program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

See LICENSE for the details.

###JSMN
Copyright (c) 2010 Serge A. Zaitsev

The software is licensed under MIT License.

See LICENSE_JSMN for the details.

### Support the original, not the imitation!
