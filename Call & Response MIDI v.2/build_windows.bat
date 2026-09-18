@echo off
setlocal enabledelayedexpansion

echo ======================================================================
echo    Call ^& Response MIDI v1.0.5 - Windows Build ^& Installer Script
echo    Viral Samples
echo ======================================================================
echo.

:: Check for CMake
where cmake >nul 2>nul
if %ERRORLEVEL% neq 0 (
    echo [ERROR] CMake is not installed or not found in PATH.
    echo Please install CMake from https://cmake.org/download/
    pause
    exit /b 1
)

:: Apply JUCE patch for special characters in target path
git -C vendor\JUCE apply --check ..\..\cmake\juce-verbatim.patch >nul 2>nul
if %ERRORLEVEL% equ 0 (
    git -C vendor\JUCE apply ..\..\cmake\juce-verbatim.patch
    echo [INFO] Applied JUCE patch for target names with ampersand.
)

:: Configure CMake with Visual Studio 2022 (x64)
echo [1/3] Configuring CMake build system...
cmake -B build -G "Visual Studio 17 2022" -A x64
if %ERRORLEVEL% neq 0 (
    echo [ERROR] CMake configuration failed.
    pause
    exit /b 1
)

:: Build Release configuration
echo.
echo [2/3] Compiling 64-bit VST3 and Standalone binaries...
cmake --build build --config Release --parallel
if %ERRORLEVEL% neq 0 (
    echo [ERROR] Build failed.
    pause
    exit /b 1
)

:: Build Inno Setup installer if installed
echo.
echo [3/3] Generating Windows Installer Setup.exe...
set ISCC_PATH="C:\Program Files (x86)\Inno Setup 6\ISCC.exe"
if exist %ISCC_PATH% (
    %ISCC_PATH% installer\windows\installer.iss
    echo [SUCCESS] Installer generated in dist\ directory!
) else (
    echo [NOTICE] Inno Setup 6 compiler not found at %ISCC_PATH%.
    echo The 64-bit VST3 and Standalone binaries were built successfully in:
    echo   build\CallAndResponseMIDI_artefacts\Release\
    echo To build the installer, install Inno Setup 6 from https://jrsoftware.org/isdl.php
)

echo.
echo ======================================================================
echo Build process completed!
echo ======================================================================
pause
