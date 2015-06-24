@echo off
make
del *.elf  *.out
copy loader.bin ..\payload.bin
pause
