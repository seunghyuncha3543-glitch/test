@echo off
setlocal
powershell -ExecutionPolicy Bypass -File "%~dp0Scripts\SetupAndRun.ps1" -SkipBuild %*
pause
