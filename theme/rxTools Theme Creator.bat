@echo off
echo ******************** PASTA CFW THEME CREATOR ******************
mkdir UI
for /r %%i in (*.png) do (
echo Preparing %%~ni
del %%~pi\UI\%%~ni.bin command > nul 2>&1
del %%~pi\UI\%%~ni.bgr command > nul 2>&1
convert -rotate 90 %%i %%~ni.bgr
rename %%~ni.bgr %%~ni.bin
)
Move *.bin UI/ >nul
echo ************************** THEME READY! ************************
pause