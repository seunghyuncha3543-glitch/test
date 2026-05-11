@echo off
setlocal
echo This project needs Visual Studio 2022 C++ build tools for Unreal C++ compilation.
echo.
echo Opening the official Visual Studio Build Tools download page...
echo After installing, select:
echo   - Game development with C++
echo   - MSVC v143
echo   - Windows 10/11 SDK
echo   - .NET SDK
echo.
start "" "https://visualstudio.microsoft.com/visual-cpp-build-tools/"
pause
