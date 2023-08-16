@if not defined _echo echo off

rem Builds Kratos Debug.

rem Ensure we're in correct directory.
cd /D "%~dp0"

call run_msbuild /maxCpuCount /consoleloggerparameters:NoSummary /property:Configuration=Debug

cd %~dp0

call run_yr
