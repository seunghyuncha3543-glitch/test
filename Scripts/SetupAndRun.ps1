param(
    [string]$EngineRoot = "",
    [switch]$SkipBuild,
    [switch]$NoEditor,
    [switch]$SkipMapScript,
    [switch]$ForceMapScript
)

$ErrorActionPreference = "Stop"

function Write-Step($Message) {
    Write-Host ""
    Write-Host "== $Message ==" -ForegroundColor Cyan
}

function Find-UnrealEngineRoot {
    param([string]$PreferredRoot)

    $candidates = New-Object System.Collections.Generic.List[string]

    if ($PreferredRoot) {
        $candidates.Add($PreferredRoot)
    }

    foreach ($envName in @("UE_ROOT", "UE5_ROOT", "UNREAL_ENGINE_ROOT")) {
        $value = [Environment]::GetEnvironmentVariable($envName)
        if ($value) {
            $candidates.Add($value)
        }
    }

    foreach ($drive in @("C:", "D:", "E:", "F:")) {
        if (!(Test-Path $drive)) {
            continue
        }

        foreach ($epicRoot in @(
            (Join-Path "$drive\" "Program Files\Epic Games"),
            (Join-Path "$drive\" "Epic Games")
        )) {
            if (Test-Path $epicRoot) {
                Get-ChildItem -Path $epicRoot -Directory -Filter "UE_5*" -ErrorAction SilentlyContinue |
                    Sort-Object Name -Descending |
                    ForEach-Object { $candidates.Add($_.FullName) }
            }
        }
    }

    $registryPaths = @(
        "HKCU:\Software\Epic Games\Unreal Engine\Builds",
        "HKLM:\Software\EpicGames\Unreal Engine"
    )

    foreach ($registryPath in $registryPaths) {
        if (Test-Path $registryPath) {
            $item = Get-ItemProperty -Path $registryPath -ErrorAction SilentlyContinue
            if ($item) {
                $item.PSObject.Properties |
                    Where-Object { $_.Value -is [string] -and $_.Value -like "*Unreal*" } |
                    ForEach-Object { $candidates.Add($_.Value) }
            }
        }
    }

    foreach ($candidate in $candidates | Select-Object -Unique) {
        if (!$candidate) {
            continue
        }

        $editor = Join-Path $candidate "Engine\Binaries\Win64\UnrealEditor.exe"
        $buildBat = Join-Path $candidate "Engine\Build\BatchFiles\Build.bat"
        if ((Test-Path $editor) -and (Test-Path $buildBat)) {
            return (Resolve-Path $candidate).Path
        }
    }

    return ""
}

function Find-VisualStudio {
    $vswherePaths = @(
        "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe",
        "$env:ProgramFiles\Microsoft Visual Studio\Installer\vswhere.exe"
    )

    foreach ($vswhere in $vswherePaths) {
        if (Test-Path $vswhere) {
            $install = & $vswhere -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath
            if ($LASTEXITCODE -eq 0 -and $install) {
                return $install.Trim()
            }
        }
    }

    return ""
}

$projectRoot = Resolve-Path (Join-Path $PSScriptRoot "..")
$projectFile = Join-Path $projectRoot "JCRRacerV4.uproject"
$mapScript = Join-Path $projectRoot "Scripts\CreatePrototypeContent.py"
$mapFile = Join-Path $projectRoot "Content\Maps\M_JapanCountryRoad_Test.umap"

Write-Host "JCR Racer V4 setup runner" -ForegroundColor Green
Write-Host "Project: $projectFile"

Write-Step "Finding Unreal Engine"
$resolvedEngineRoot = Find-UnrealEngineRoot -PreferredRoot $EngineRoot
if (!$resolvedEngineRoot) {
    Write-Host "Unreal Engine 5 was not found on this PC." -ForegroundColor Red
    Write-Host ""
    Write-Host "Install Unreal Engine 5.4 or newer from Epic Games Launcher, then run SetupAndRun.bat again."
    Write-Host "If Unreal is installed in a custom folder, run:"
    Write-Host "  powershell -ExecutionPolicy Bypass -File Scripts\SetupAndRun.ps1 -EngineRoot `"D:\Path\To\UE_5.x`""
    exit 1
}

$editorExe = Join-Path $resolvedEngineRoot "Engine\Binaries\Win64\UnrealEditor.exe"
$buildBat = Join-Path $resolvedEngineRoot "Engine\Build\BatchFiles\Build.bat"
$generateBat = Join-Path $resolvedEngineRoot "Engine\Build\BatchFiles\GenerateProjectFiles.bat"
$ubtDll = Join-Path $resolvedEngineRoot "Engine\Binaries\DotNET\UnrealBuildTool\UnrealBuildTool.dll"

Write-Host "Unreal Engine: $resolvedEngineRoot" -ForegroundColor Green

Write-Step "Checking Visual Studio C++ tools"
$vsInstall = Find-VisualStudio
if (!$vsInstall) {
    Write-Host "Visual Studio C++ build tools were not found." -ForegroundColor Yellow
    Write-Host "Install Visual Studio 2022 with these workloads/components:"
    Write-Host "  - Game development with C++"
    Write-Host "  - MSVC v143"
    Write-Host "  - Windows 10/11 SDK"
    Write-Host ".NET SDK is also recommended for UnrealBuildTool."
    Write-Host ""
    Write-Host "Run InstallVisualStudioBuildTools.bat, install the items above, reboot if requested, then run Run_E_Drive_Unreal.bat."
    exit 2
} else {
    Write-Host "Visual Studio: $vsInstall" -ForegroundColor Green
}

if (!$SkipBuild) {
    Write-Step "Generating Visual Studio project files"
    if (Test-Path $generateBat) {
        & $generateBat -project="$projectFile" -game -engine
        if ($LASTEXITCODE -ne 0) {
            throw "GenerateProjectFiles failed with exit code $LASTEXITCODE"
        }
    } elseif (Test-Path $ubtDll) {
        & $buildBat -ProjectFiles -Project="$projectFile" -Game -Engine -Progress
        if ($LASTEXITCODE -ne 0) {
            throw "UnrealBuildTool project-file generation failed with exit code $LASTEXITCODE"
        }
    } else {
        Write-Host "Could not find GenerateProjectFiles.bat or UnrealBuildTool.dll." -ForegroundColor Yellow
        Write-Host "Skipping solution generation and trying the build step directly."
    }

    Write-Step "Compiling Unreal Editor target"
    & $buildBat JCRRacerV4Editor Win64 Development -Project="$projectFile" -WaitMutex -FromMsBuild
    if ($LASTEXITCODE -ne 0) {
        throw "Build failed with exit code $LASTEXITCODE"
    }
} else {
    Write-Step "Skipping build"
}

if ($NoEditor) {
    Write-Host ""
    Write-Host "Compile step finished. Editor launch skipped." -ForegroundColor Green
    exit 0
}

Write-Step "Opening Unreal Editor"
if (!$SkipMapScript -and (Test-Path $mapScript) -and ($ForceMapScript -or !(Test-Path $mapFile))) {
    Write-Host "Generating clean test map first..."
    $generateArgs = New-Object System.Collections.Generic.List[string]
    $generateArgs.Add("`"$projectFile`"")
    $generateArgs.Add("-ExecutePythonScript=`"$mapScript`"")
    $generateArgs.Add("-unattended")
    $generateArgs.Add("-log")
    $generator = Start-Process -FilePath $editorExe -ArgumentList $generateArgs -Wait -PassThru
    if ($generator.ExitCode -ne 0) {
        throw "Map generation editor run failed with exit code $($generator.ExitCode)"
    }
}

$argsList = New-Object System.Collections.Generic.List[string]
$argsList.Add("`"$projectFile`"")
$argsList.Add("/Game/Maps/M_JapanCountryRoad_Test")
$argsList.Add("-log")
Start-Process -FilePath $editorExe -ArgumentList $argsList

Write-Host ""
Write-Host "Unreal Editor is launching with /Game/Maps/M_JapanCountryRoad_Test. Press Play to test." -ForegroundColor Green
