@echo off
cd rxinstaller
make
copy rxinstaller.nds ..\rxinstaller.nds
cd ..

cls
mkdir build
cd loader
make
del *.elf  *.out
copy loader.bin ..\build\arm9_code.bin
cd ..

make
pause