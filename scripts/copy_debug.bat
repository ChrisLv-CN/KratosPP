@echo off
chcp 65001

cd %~dp0
echo f|xcopy /i /y "..\output\Debug\KratosPP.dll" "..\output\KratosPP.dll"
echo f|xcopy /i /y "..\output\Debug\KratosPP.pdb" "..\output\KratosPP.pdb"

call run_yr
