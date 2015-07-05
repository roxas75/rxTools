# rxTools - Roxas75 3DS Toolkit

[![Join the chat at https://gitter.im/roxas75/rxTools](https://badges.gitter.im/Join%20Chat.svg)](https://gitter.im/roxas75/rxTools?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)

rxTools is a collection of hacking tools for Nintendo 3DS/3DSXL/2DS, compatible with all the system versions between 4.1 and 9.2.

rxMode is a free custom firmware wich works through rxTools, not meant to substitute any existing software, but just my personal 
work made public and easy for the end-user.

It was not meant, from the beginning, to be open-source, so you can expect some messed up code here.

I do not support piracy in any way, all the data and informations in my software have already been made public.

## Current functionalities
### General
- Support for Web Browser (SPIDER) entrypoint for Old3DS (ver 4.1-9.2)
- Support for System Settings (MSET) entrypoint for Old3DS (ver 4.1-4.5)
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
- Support for TWL (DS/DSi cardriges and Virtual Console) **only on sysNAND**
- Dynamic ram dumping (just for debug purpouses)

## How to build
The setup is meant to work under Windows, just becouse i'm not good on coding Makefiles (it's in my TODO list) and i used some ms-dos scripting.
Since the project is open-source you are welcome to implement other some systems setup.

What is actually necessary is:
- [Python 2.7](https://www.python.org)
- [devkitPRO](http://sourceforge.net/projects/devkitpro) with devkitARM option and suboptions installed (in case make.exe fails with stack trace dump, install [GnuWin Make](http://gnuwin32.sourceforge.net) and place it in path before devkitPRO/msys/bin)
- [armips] (https://github.com/Kingcom/armips) (for Windows [Visual Studio 2013 Express for Windows](https://www.visualstudio.com/en-us/downloads/download-visual-studio-vs.aspx) or higher is required to build armips)
- GCC (for Windows [MinGW](http://sourceforge.net/projects/mingw) with mingw32-base will be fine)
- [ImageMagik](http://www.imagemagick.org)

Just run the **make** and rxTools.dat should be generated in the **release** folder, along with all the other packets.
If you get the "make: Interrupt/Exception caught (code = 0xc00000fd, addr = 0x4227d3)" error on Windows, look at your PATH and move any path with parenthesis at the end of it

## Credits
- All the documentation on http://3dbrew.org, and to all the devs who contributed
- Roxas75, myself, as the only developer of rxTools
- Gateway team, for their work 
- Archshift, who wrote some useful libs i used : https://github.com/archshift
- Many GbaTemp users, who helped in the testing process
- patois, who developed BRAHMA : https://github.com/patois/Brahma
- Many other people who helped me in private
- All the dudes who reversed and fucked up my project, who lost their time on making useless keeping the sources private

### Support the original, not the imitation!
