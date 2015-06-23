@echo off
make
del *.elf  *.out
copy loader.bin ..\build\arm9_code.bin
pause
