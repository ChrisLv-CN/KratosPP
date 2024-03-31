@echo off
chcp 65001

cd %~dp0
echo f|xcopy /i /y "..\output\Debug\*.dll" "..\output\*.dll"
echo f|xcopy /i /y "..\output\Debug\*.pdb" "..\output\*.pdb"

call run_yr
