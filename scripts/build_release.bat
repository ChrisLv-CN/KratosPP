@if not defined _echo echo off

rem Builds Kratos Release.

rem Ensure we're in correct directory.
cd /D "%~dp0"

call run_msbuild /maxCpuCount /consoleloggerparameters:NoSummary /property:Configuration=Release

cd %~dp0
rd /S /Q "..\output"
echo f|xcopy /i /D /y "..\Release\KratosPP.dll" "..\output\KratosPP.dll"
echo f|xcopy /i /D /y "..\Release\KratosPP.pdb" "..\output\KratosPP.pdb"

call run_yr