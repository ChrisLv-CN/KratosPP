@echo off
chcp 65001
cd %~dp0
echo f|xcopy /i /y "..\output\*.dll" "D:\Games\YR_WWSB\*.dll"
echo f|xcopy /i /y "..\output\*.pdb" "D:\Games\YR_WWSB\*.pdb"
cd "D:\Games\YR_WWSB\"
call RunAres
