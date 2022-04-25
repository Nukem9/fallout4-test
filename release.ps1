# Menu is at the end of the file

function Clean-ProjectDirectory
{
Remove-Item -Path .\Build -Recurse
Remove-Item -Path .\x64 -Recurse
Remove-Item -Path .\x86 -Recurse
}

function Make-GeneralRelease
{
[Reflection.Assembly]::LoadWithPartialName("System.IO.Compression.FileSystem")
$compression = [System.IO.Compression.CompressionLevel]::Optimal

#
# DLLs
#
mkdir "Build"
copy "fallout4_test.ini" "Build\fallout4_test.ini"

cd "x64\Release"
copy "winhttp.dll" "..\..\Build\winhttp.dll"
copy "tbb.dll" "..\..\Build\tbb.dll"
copy "tbbmalloc.dll" "..\..\Build\tbbmalloc.dll"

cd ..
cd ..
[System.IO.Compression.ZipFile]::CreateFromDirectory("Build", "CK64Fixes Release X.zip", $compression, $false) # Don't include base dir
}

function Write-VersionFile
{
$versionFileInfo = @" 
#pragma once

#define VER_CURRENT_COMMIT_ID "<COMMITID>"
#define VER_CURRENT_DATE "<DATE>"
"@

$commitId = (git rev-parse --short HEAD).ToUpper()
$currDate = (Get-Date)

$versionFileInfo = $versionFileInfo -Replace "<COMMITID>", $commitId
$versionFileInfo = $versionFileInfo -Replace "<DATE>", $currDate

#$targetDir = "fallout4_test\src\"

#if (!(Test-Path -Path $targetDir)) {
#    $targetDir = "src\"
#}
$targetDir = ""

$versionFileInfo | Out-File -FilePath ($targetDir + "version_info.h") -Encoding ASCII
}

function Build-Project
{
$buildCmd = "/c `"`"%VS2019INSTALLDIR%\Common7\Tools\VsDevCmd.bat`" & msbuild fallout4_test.sln -m -t:Build -p:Configuration=Release;Platform=x64`""
Start-Process "cmd.exe" $buildCmd
}

# Check for params passed on the command line
$input = $args[0]

if ([string]::IsNullOrWhiteSpace($input)) {
    Write-Host "==================================="
    Write-Host "1: Clean project directory"
    Write-Host "2: Build project"
    Write-Host "3: Create release build archives"
    Write-Host "4: Write version file"
    Write-Host "==================================="

    $input = Read-Host "Selection"
}

switch ($input)
{
    '1' {
        cls
        Clean-ProjectDirectory
    } '2' {
        cls
        Build-Project
    } '3' {
        cls
        Make-GeneralRelease
    } '4' {
        cls
        Write-VersionFile
    }
}

exit