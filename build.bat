@echo off

cls
echo BUILDING RXTOOLS...
cd rxtools
make
cd ..
tools\fill_with_zero.exe rxtools\rxtools.bin 917504
tools\addxor_tool.exe rxtools\rxtools.bin payload.bin 0x67893421 0x12756342
pause

cls
echo BUILDING RXMODE...
cd rxmode
mkdir build
cd source\arm9
make
cd ..\..
make
make clean
move patch.bin ..\data\patch.bin
cd ..
pause

cls
echo BUILDING DATA...
tools\pack_tool.exe data/titlekey.bin data/reboot.bin data/patch.bin data/top_bg.bin data.bin
tools\xor.exe data.bin tools\xorpad\data.xor
del data.bin
ren data.bin.out data.bin
pause

cls
echo BUILDING SPIDERHAX...
copy spiderhax\Launcher.dat rxTools.dat
tools\insert.py rxTools.dat payload.bin 0x20000
tools\insert.py rxTools.dat data.bin 0x100000
tools\fill_with_crap.exe rxTools.dat 4194304
pause

cls BUILDING NINJHAX...
cd ninjhax
cd data\loader
make
del *.elf  *.out
cd ..\..
..\tools\addxor_tool.exe data/loader/loader.bin data/payload.bin 0x12832738 0x76298987
make
del *.elf
cd ..
pause

cls
echo BUILDING MSET...
cd msethax
cd rxinstaller
make
copy rxinstaller.nds ..\rxinstaller.nds
cd ..
mkdir build
cd loader
make
del *.elf  *.out
copy loader.bin ..\build\arm9_code.bin
cd ..
make
cd ..
tools\insert.py rxTools.dat msethax\mset.bin 0
mkdir release\mset
pause

cls
echo MAKING RELEASE...
copy rxTools.dat release\rxTools.dat
mkdir release\ninjhax
copy ninjhax\ninjhax.3dsx release\ninjhax\rxtools.3dsx
copy ninjhax\ninjhax.smdh release\ninjhax\rxtools.smdh
mkdir release\mset
copy msethax\rxinstaller.nds release\mset\rxinstaller.nds
copy rxTools.dat G:\rxTools.dat

echo CLEANING...
del *.bin *.out
echo DONE!
pause
