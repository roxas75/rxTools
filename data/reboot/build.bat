@echo off
make
del *.elf  *.out
copy reboot.bin ..\reboot.bin
pause
