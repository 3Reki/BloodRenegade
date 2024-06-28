@setlocal EnableDelayedExpansion

@if exist "%~dp0config.txt" (
	for /f "eol=# delims=" %%a in (%~dp0config.txt) do set "%%a"
) else (
	set PROJECT_PATH=%~1
	set PROJECT_NAME=%~2
	set UE5_REBUILDBAT=%~3
	set BUILD_CONFIG=%~4
	set PLATFORM=%~5
	set CONFIG=%~6
)

for /f "eol=# delims=" %%a in (%~dp0.tmpconfig) do set "%%a"
set OUTPUT=%~dp0output.log

dos2unix UpdateGameVersion.sh
sh UpdateGameVersion.sh "%PROJECT_PATH%\%PROJECT_NAME%\Config\DefaultGame.ini" Engine_%CHANGELIST_NBR%

(CALL "%UE5_REBUILDBAT%" "%BUILD_CONFIG%" "%PLATFORM%" "%CONFIG%" "%PROJECT_PATH%\%PROJECT_NAME%\%PROJECT_NAME%.uproject" -WaitMutex -FromMsBuild || echo Unreal Build Failed.) | tee "%OUTPUT%"
@rem CALL "C:\Program Files\Epic Games\UE_5.2\Engine\Build\BatchFiles\Rebuild.bat" BloodRenegadeEditor Win64 Development "C:\BloodRenegadeWorkspace\BloodRenegade\BloodRenegade.uproject" -WaitMutex -FromMsBuild

@(find "Unreal Build Failed." "%OUTPUT%" >nul && set ERROR_CODE=1) && find /i "error" "%OUTPUT%" | find /i "error" | find /v "ShadowErrors" | find /v "nostoponerror" | find /v "EngineIncludeOrderVersion.Latest" > %~dp0error.log

@rem if "%UNATTENDED%" NEQ "1" (pause)

@if "!ERROR_CODE!" == "1" (
	exit /b !ERROR_CODE!
)

@if "%ERRORLEVEL%" GTR "1" (
	exit /b %ERRORLEVEL%
)

exit /b 0
