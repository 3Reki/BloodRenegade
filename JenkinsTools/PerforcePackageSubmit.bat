set WORKSPACE_PATH=%1

for /f "eol=# delims=" %%a in (%~dp0.tmpconfig) do set "%%a"

set UE_GAMEINI=%WORKSPACE_PATH%/BloodRenegade/Config/DefaultGame.ini
set YYMMDD=%DATE:~12,4%%DATE:~4,2%%DATE:~7,2%

p4 set p4user=builder
p4 set p4passwd=dIBCqe'jrT098$$89_I1ND
p4 set p4port=192.168.140.201:1666
p4 set p4client=builder_BLOOD_RENEGADE_3737

p4 revert -k %WORKSPACE_PATH%/Exe/...
p4 sync -k %WORKSPACE_PATH%/Exe/...
p4 reconcile -e -a -f -t %WORKSPACE_PATH%/Exe/...
p4 reopen -t +S %WORKSPACE_PATH%/Exe/...
p4 revert %UE_GAMEINI%
p4 submit -d "Submitted by Jenkins. Packaged Build: %YYMMDD%_%CHANGELIST_NBR%"

"%~dp0SendDiscordWebhook.py" "[Package] Packaging sent to perforce. ID : %YYMMDD%_%CHANGELIST_NBR%"