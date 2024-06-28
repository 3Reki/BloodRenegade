@echo off

setlocal EnableDelayedExpansion

set DO_COPY=1
set COPY_DEST=C:\Share\Builds\Logs

echo Reading config...
if exist "%~dp0config.txt" (
	for /f "eol=# delims=" %%a in (%~dp0config.txt) do set "%%a"
) else (
	set PROJECT_PATH=%~1
	set PROJECT_NAME=%~2
	set UE5_PATH=%~3
	set PLATFORM=%~4
	set CONFIG=%~5
)

for /f "eol=# delims=" %%a in (%~dp0.tmpconfig) do set "%%a"
set RUNUAT_BAT=!UE5_PATH!\Engine\Build\BatchFiles\RunUAT.bat
set UE_CMD=!UE5_PATH!\Engine\Binaries\Win64\UnrealEditor-Cmd.exe
set OUTPUT=%~dp0PackageOutput\output.log
set ERROR_LOG=%~dp0PackageOutput\error.log
set YYMMDD=%DATE:~12,4%%DATE:~4,2%%DATE:~7,2%
set BUILD_VERSION=%YYMMDD%_%CHANGELIST_NBR%

"%~dp0SendDiscordWebhook.py" "[Package] Starting build"

dos2unix UpdateGameVersion.sh
sh UpdateGameVersion.sh "%PROJECT_PATH%\%PROJECT_NAME%\Config\DefaultGame.ini" "%YYMMDD%_%CHANGELIST_NBR%"

echo Launching compile...
@echo on
(CALL "%RUNUAT_BAT%" -ScriptsForProject="%PROJECT_PATH%\%PROJECT_NAME%\%PROJECT_NAME%.uproject" Turnkey -command=VerifySdk -platform=%PLATFORM% -UpdateIfNeeded -EditorIO -EditorIOPort=60922^
  -project="%PROJECT_PATH%\%PROJECT_NAME%\%PROJECT_NAME%.uproject" BuildCookRun -nop4 -utf8output -nocompileeditor -skipbuildeditor -cook^
  -project="%PROJECT_PATH%\%PROJECT_NAME%\%PROJECT_NAME%.uproject" -target=%PROJECT_NAME% -unrealexe="%UE_CMD%" -platform=%PLATFORM% -installed -stage -archive -package -build -pak -iostore^
 -compressed -prereqs -archivedirectory="%PROJECT_PATH%\Exe" -CrashReporter -clientconfig=%CONFIG% || echo Packaging Failed.) | tee "%OUTPUT%"
@echo off

echo Writing warnings to separate file...
call FindPackageWarnings.bat "%OUTPUT%" "%ERROR_LOG%"

if "!DO_COPY!" == "1" (
	echo Copying logs to !COPY_DEST!
	copy "%OUTPUT%" "!COPY_DEST!"
	copy "%ERROR_LOG%" "!COPY_DEST!"
)

find "Packaging Failed." "%OUTPUT%" >nul && goto ErrorFound

sh UpdateGameVersion.sh "%PROJECT_PATH%\Exe\Windows\Engine\Restricted\NoRedist\Programs\CrashReportClient\Config\DefaultEngine.ini" "%BUILD_VERSION%"

symbol-upload-win.exe -i 6662ec960285c1dd388ba201c6d7153f -s WFp17ll3rKOasRluXe5kGBLDr+RKiX5t -b c_oger_rubika_edu_com -a BloodRenegade -v %BUILD_VERSION% -f "**/*.+(exe|dll|pdb)" -d "%PROJECT_PATH%\Exe\Windows"

rem if "%UNATTENDED%" NEQ "1" (pause)
"%~dp0SendDiscordWebhook.py" "[Package] Packaging successful :white_check_mark:"
exit /b 0

:ErrorFound
echo error found
"%~dp0SendDiscordWebhook.py" "[Package] Packaging failed :x:"
exit /b 1