@echo off
chcp 65001
cd %~dp0
echo f|xcopy /i /y "..\output\*.dll" "D:\Games\YR\*.dll"
echo f|xcopy /i /y "..\output\*.pdb" "D:\Games\YR\*.pdb"
cd "D:\Games\YR\"
call RunAres
