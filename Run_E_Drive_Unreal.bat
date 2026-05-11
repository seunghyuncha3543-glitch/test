@echo off
setlocal
powershell -ExecutionPolicy Bypass -File "%~dp0Scripts\SetupAndRun.ps1" -EngineRoot "E:\Epic Games\UE_5.7" %*
pause
