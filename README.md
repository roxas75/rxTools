# rxTools - Roxas75 3DS Toolkit

[![Join the chat at https://gitter.im/roxas75/rxTools](https://badges.gitter.im/Join%20Chat.svg)](https://gitter.im/roxas75/rxTools?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)

rxTools is a collection of hacking tools for Nintendo 3DS/3DSXL/2DS, compatible with all the system versions from 4.1 to 9.2.

rxMode is a free custom firmware which works through rxTools, not meant to replace any existing software, but just my personal
work made public and easy for the end-user.

It was not meant, at the beginning, to be open-source, so you can expect some messed up code here.

I do not support piracy in any way, all the data and information in my software have already been made public.

## Current functionality
### General
- Support for Web Browser (SPIDER) entrypoint for Old3DS (ver 4.1-9.2)
- Support for System Settings (MSET) entrypoint for Old3DS (ver 4.1-4.5 and 6.0-6.4)
- Support for System Settings (MSET) downgrading for Old3DS
- Support for Ninjhax (Not Stable)
- Hacking tools collection

### rxMode
- Support for the latest emuNAND version
- Support for sysNAND, in case an emuNAND is not found
- Access to the eShop and to online playing (requires an updated emuNAND)
- Support for 100% of the games (newer ones will require emuNAND)
- Signatures Checks disabled, which should allow just homebrew, but the world is cruel...
- Support for installing FBI (a CIA Manager) in both emuNAND and sysNAND
- Support for AGB (GBA Virtual Console) on both sysNAND and emuNAND
- Support for TWL (DS/DSi cartridges and DSiWare) **only on sysNAND**
- Dynamic RAM dumping (just for debug purposes)

## How to build
Requires ImageMagick (9.8.9 >) - Do not use Ubuntu 14.04 as the package is outdated.

Requirements for Windows:
- git, clone this repository with the following command: *git  clone --recursive https://github.com/roxas75/rxTools.git*
- [Python 2.7](https://www.python.org)
- [devkitPRO](http://sourceforge.net/projects/devkitpro) with devkitARM option and suboptions installed (in case make.exe fails with stack trace dump, install [GnuWin Make](http://gnuwin32.sourceforge.net) and place it in path before devkitPRO/msys/bin)
- GCC (for Windows [MinGW](http://sourceforge.net/projects/mingw) with mingw32-base will be fine)
- [ImageMagick](http://www.imagemagick.org)

Requirements for GNU/Linux:
- [git](https://apps.ubuntu.com/cat/applications/git/), clone this repository with the following command: *git  clone --recursive https://github.com/roxas75/rxTools.git*
- [devkitPRO and devkitARM](http://3dbrew.org/wiki/Setting_up_Development_Environment) (follow the tutorial for Linux)
- [ImageMagick](http://www.imagemagick.org) (on Debian/Ubuntu, [install it using apt](https://apps.ubuntu.com/cat/applications/imagemagick/))

Just run **make** and rxTools/system directory should be generated in the **release** folder, along with all the other packets.
If you get the *make: Interrupt/Exception caught (code = 0xc00000fd, addr = 0x4227d3)* error on Windows, look at your PATH and move any path with parenthesis to the end of it

Make sure that the path to rxTools doesn't include space (' ').

## Credits
- All the documentation on http://3dbrew.org, and to all the devs who contributed
- Roxas75, myself, as the creator of rxTools
- Gateway team, for their work
- Reisyukaku for N3DS MSET 
- Archshift, who wrote some useful libs I used: https://github.com/archshift
- Many GBATemp users, who helped in the testing process
- patois, who developed BRAHMA: https://github.com/patois/Brahma
- mid-kid and b1l1s, who developed CakeHax: https://github.com/mid-kid/CakeHax
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
