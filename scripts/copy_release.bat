@echo off
chcp 65001

cd %~dp0
echo f|xcopy /i /y "..\output\Release\*.dll" "..\output\*.dll"
echo f|xcopy /i /y "..\output\Release\*.pdb" "..\output\*.pdb"

call run_yr
