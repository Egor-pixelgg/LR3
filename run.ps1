param(
    [Parameter(Mandatory = $true, Position = 0)]
    [ValidateSet("server-sync", "client-sync", "server-async", "client-async")]
    [string]$Mode,

    [Parameter()]
    [string]$ServerHost = "127.0.0.1",

    [Parameter()]
    [int]$Port = 5555,

    [Parameter()]
    [string]$Message = "Hello world",

    [Parameter()]
    [int]$Threads = 4
)

$ErrorActionPreference = "Stop"

$Root = Split-Path -Parent $MyInvocation.MyCommand.Path
$BuildDir = Join-Path $Root "cmake-build-debug"
$Exe = Join-Path $BuildDir "lab3.exe"
$Cmake = "D:\MySys2\ucrt64\bin\cmake.exe"
$MsysUcrtBin = "D:\MySys2\ucrt64\bin"
$MsysUsrBin = "D:\MySys2\usr\bin"

if (Test-Path $MsysUcrtBin) {
    $env:Path = "$MsysUcrtBin;$env:Path"
}
if (Test-Path $MsysUsrBin) {
    $env:Path = "$MsysUsrBin;$env:Path"
}

& $Cmake --build $BuildDir --target lab3 -j 14

switch ($Mode) {
    "server-sync" {
        & $Exe "server-sync" "$Port"
    }
    "client-sync" {
        & $Exe "client-sync" "$ServerHost" "$Port" "$Message"
    }
    "server-async" {
        & $Exe "server-async" "$Port" "$Threads"
    }
    "client-async" {
        & $Exe "client-async" "$ServerHost" "$Port" "$Message"
    }
}
