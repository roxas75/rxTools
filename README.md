# rxTools - Roxas75 3DS Toolkit

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
- DevMode, a semplification and automation of what you all call "PastaCFW", which i used for debug and always hidden in the menu

### rxMode
- Support for the latest emuNAND version
- Support for sysNAND, in case an emuNAND is not found
- Access to the eShop and to online playing (requires an updated emuNAND)
- Support for 100% of the games (newer ones will require emuNAND)
- Signatures Checks disabled, which should allow just homebrew, but the world is cruel...
- Support for installing FBI (a CIA Manager) in both emuNAND and sysNAND
- Dynamic ram dumping (just for debug purpouses)

## How to build
The setup is meant to work under Windows, just becouse i'm not good on coding Makefiles (it's in my TODO list) and i used some ms-dos scripting.
Since the project is open-source you are welcome to implement other some systems setup.

What is actually necessary is:
- Python 2.7
- [devkitPRO](http://sourceforge.net/projects/devkitpro) with devkitARM, libctru, libnds installed
- [armips] (https://github.com/Kingcom/armips)
- GCC (for Windows [MinGW](http://sourceforge.net/projects/mingw) with gcc-base will be fine)

Just run the **make.exe** and rxTools.dat should be generated in the **release** folder, along with all the other packets.

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
