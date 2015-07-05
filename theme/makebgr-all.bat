@echo off
REM This script was written by Lavanoid (WhoAmI?). Enjoy!
mkdir UI
@echo Converting...
cd %~dp0
set i=0
Title = Converting images. Processed: %i%
for %%f in (*.png) do call :convertfile "%%f"
@echo --------------------------------------------
@echo All files have been processed.
@echo.
@echo         Script written by Lavanoid (WhoAmI?)
@echo --------------------------------------------
@echo Press any key.
pause >nul
goto :end
:convertfile
set /a i=%i%+1
echo File: %~nx1
echo Bin: %~n1.bin
if exist "%~n1.bin" @echo Removing "%~n1.bin"...
if exist "%~n1.bin" del /f "%~n1.bin"
if exist "%~n1.bin" @echo Failed to remove "%~n1.bin"! & goto :end
convert -rotate 90 %~nx1 %~n1.bgr
if exist "%~n1.bgr" @echo Successfully generated "%~n1.bin"!
Title = Converting images. Processed: %i%
:end
rename *.bgr *.bin
Move *.bin UI/ >nul
