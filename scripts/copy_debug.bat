@echo off
chcp 65001

cd %~dp0
echo f|xcopy /i /D /y "..\output\Debug\KratosPP.dll" "..\output\KratosPP.dll"
echo f|xcopy /i /D /y "..\output\Debug\KratosPP.pdb" "..\output\KratosPP.pdb"

