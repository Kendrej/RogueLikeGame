# === Auto-konfiguracja VS 2022: host x64, arch x64 ===

$vsDevCmd = "C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\Tools\VsDevCmd.bat"

# Sprawdü, czy PATH nie ma juø HostX64\x64
$hasX64 = ($env:PATH -split ';' | Select-String 'MSVC\\.*\\bin\\HostX64\\x64' -Quiet)

if (-not $hasX64 -and (Test-Path $vsDevCmd)) {
    cmd /c "`"$vsDevCmd`" -host_arch=x64 -arch=x64 && set" |
      ForEach-Object {
        if ($_ -match '^(INCLUDE|LIB|LIBPATH|PATH)=') {
          $n,$v = $_ -split '=',2
          Set-Item -Path Env:$n -Value $v
        }
      }
}
