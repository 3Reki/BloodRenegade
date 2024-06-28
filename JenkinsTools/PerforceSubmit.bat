set WORKSPACE_PATH=%1

for /f "eol=# delims=" %%a in (%~dp0.tmpconfig) do set "%%a"

set UE_GAMEINI=%WORKSPACE_PATH%/BloodRenegade/Config/DefaultGame.ini

p4 set p4user=builder
p4 set p4passwd=dIBCqe'jrT098$$89_I1ND
p4 set p4port=192.168.140.201:1666
p4 set p4client=builder_BLOOD_RENEGADE_3737

p4 revert -k %WORKSPACE_PATH%/.../Binaries/...
p4 sync -k %WORKSPACE_PATH%/.../Binaries/...
p4 reconcile -e -a -f -t %WORKSPACE_PATH%/.../Binaries/...
p4 revert -k %UE_GAMEINI%
p4 sync -k %UE_GAMEINI%
p4 reconcile -e -a -f -t %UE_GAMEINI%

p4 submit -d "Submitted by Jenkins. Engine version : Engine_%CHANGELIST_NBR%"