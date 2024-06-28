set WORKSPACE_PATH=%1

p4 set p4user=builder
p4 set p4passwd=dIBCqe'jrT098$$89_I1ND
p4 set p4port=192.168.140.201:1666
p4 set p4client=builder_BLOOD_RENEGADE_3737
p4 sync %WORKSPACE_PATH%/...#head

FOR /F "tokens=* USEBACKQ" %%F IN (`p4 changes -m1 #have`) DO (
SET P4_CHANGELIST=%%F
)
for /f "tokens=2" %%i in ("%P4_CHANGELIST%") do set CHANGELIST_NBR=%%i
echo CHANGELIST_NBR=%CHANGELIST_NBR%>.tmpconfig