#!/bin/sh.exe

#signtool sign //v //f ../../LMSPC.pfx release/eLabClient.exe
cp release/QtCBM.exe ../../Google\ Drive/QtCBM
/c/Program\ Files/NSIS/makensis.exe QtCBM.nsi
#signtool sign //v //f ../../LMSPC.pfx LM_Setup.exe
