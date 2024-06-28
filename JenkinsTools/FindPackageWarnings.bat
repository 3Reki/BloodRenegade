@echo off && setlocal enabledelayedexpansion

set INPUT=%~1
set OUTPUT=%~2

if "%OUTPUT%" == "" (
  echo No output file given, correct command is FindPackageWarnings.bat input_file output_file
  exit /b 1
)

echo Deleting old error output...
del /q /f "%OUTPUT%"

echo Writing warnings to error output...
set "_line=0" & for /f "delims=" %%i in ('type "%INPUT%"
    ')do if "!_line!" == "0" (echo;"%%~i"| findstr /c:"Warning/Error Summary (Unique only)" >nul && set "_line=1" )else (
         echo\"%%~i" | findstr /c:"NOTE: Only first 50 warnings displayed" >nul && set "_line=0" || call :write %%~i)
		 
echo ----------------------------------->> "%OUTPUT%"
echo NOTE: Only first 50 warnings displayed.>> "%OUTPUT%"
         
endlocal  

echo Error log written to %OUTPUT%
exit /b

:write
set str=%*
set word=LogInit: Display: 
call set str=%%str:%word%=%%
echo\%str%>> "%OUTPUT%"