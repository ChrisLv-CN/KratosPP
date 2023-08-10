@echo off
chcp 65001
cd %~dp0
echo f|xcopy /i /D /y "..\output\*.dll" "C:\Games\Yuri's Revenge\*.dll"
echo f|xcopy /i /D /y "..\output\*.pdb" "C:\Games\Yuri's Revenge\*.pdb"
cd "C:\Games\Yuri's Revenge\"
call RunAres
