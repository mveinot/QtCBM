@echo off

REM signtool sign /v /f ..\..\LMSPC.pfx release\eLabClient.exe
copy release\QtCBM.exe "..\..\Google Drive\QtCBM"
"c:\Program Files\NSIS\makensis.exe" QtCBM.nsi
REM signtool sign /v /f ..\..\LMSPC.pfx LM_Setup.exe
