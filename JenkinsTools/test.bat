@echo off

setlocal EnableDelayedExpansion

for /f "eol=# delims=" %%a in (%~dp0config.txt) do set "%%a"

set RUNUAT_BAT=!UE5_PATH!\Engine\Build\BatchFiles\RunUAT.bat
set UE_CMD=!UE5_PATH!\Engine\Binaries\Win64\UnrealEditor-Cmd.exe
set OUTPUT=%~dp0PackageOutput\output.log
set ERROR_LOG=%~dp0PackageOutput\error.log
set YYMMDD=%DATE:~12,4%%DATE:~4,2%%DATE:~7,2%

@echo on
(CALL "%RUNUAT_BAT%" -ScriptsForProject="%PROJECT_PATH%\%PROJECT_NAME%\%PROJECT_NAME%.uproject" Turnkey -command=VerifySdk -platform=%PLATFORM% -UpdateIfNeeded -EditorIO -EditorIOPort=60922^
  -project="%PROJECT_PATH%\%PROJECT_NAME%\%PROJECT_NAME%.uproject" BuildCookRun -nop4 -utf8output -nocompileeditor -skipbuildeditor -cook^
  -project="%PROJECT_PATH%\%PROJECT_NAME%\%PROJECT_NAME%.uproject" -target=%PROJECT_NAME% -unrealexe="%UE_CMD%" -platform=%PLATFORM% -installed -stage -archive -package -build -pak -iostore^
 -compressed -prereqs -archivedirectory="%PROJECT_PATH%\Exe" -CrashReporter -clientconfig=%CONFIG% || echo Packaging Failed.) | tee "%OUTPUT%"
@echo off

exit /b 0

cmd.exe /c ""C:/Program Files/EpicGames/UE_5.2/Engine/Build/BatchFiles/RunUAT.bat"
  -ScriptsForProject="D:/BloodRenegade/BloodRenegade/BloodRenegade.uproject" Turnkey -command=VerifySdk -platform=Win64 -UpdateIfNeeded -EditorIO -EditorIOPort=52725
  -project="D:/BloodRenegade/BloodRenegade/BloodRenegade.uproject" BuildCookRun -nop4 -utf8output -nocompileeditor -skipbuildeditor -cook
  -project="D:/BloodRenegade/BloodRenegade/BloodRenegade.uproject" -target=BloodRenegade
  -unrealexe="C:\Program Files\EpicGames\UE_5.2\Engine\Binaries\Win64\UnrealEditor-Cmd.exe" -platform=Win64 -installed -stage -archive -package -build -pak -iostore -compressed -prereqs
  -archivedirectory="D:/BloodRenegade/BloodRenegade/LocalBuilds/Development" -CrashReporter -clientconfig=Development" -nocompile -nocompileuat
