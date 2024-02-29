@echo off
chcp 65001

cd %~dp0
echo f|xcopy /i /y "..\output\Release\KratosPP.dll" "..\output\KratosPP.dll"
echo f|xcopy /i /y "..\output\Release\KratosPP.pdb" "..\output\KratosPP.pdb"

