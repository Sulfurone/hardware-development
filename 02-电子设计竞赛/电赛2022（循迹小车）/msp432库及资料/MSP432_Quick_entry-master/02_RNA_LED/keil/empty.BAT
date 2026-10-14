SET PATH=E:\keil5ARM\ARM\ARMCLANG\Bin;C:\Program Files (x86)\Common Files\Intel\Shared Libraries\redist\intel64\compiler;C:\Windows\system32;C:\Windows;C:\Windows\System32\Wbem;C:\Windows\System32\WindowsPowerShell\v1.0\;C:\Windows\System32\OpenSSH\;E:\matlab\runtime\win64;E:\matlab\bin;E:\matlab\polyspace\bin;C:\Program Files\dotnet\;C:\Program Files (x86)\NVIDIA Corporation\PhysX\Common;%USERPROFILE%\AppData\Local\Microsoft\WindowsApps;C:\Program Files (x86)\Tencent\QQGameTempest\Hall.57768\
SET CPU_TYPE=MSP432P401R
SET CPU_VENDOR=Texas Instruments
SET UV2_TARGET=empty
SET CPU_CLOCK=0x00B71B00
"E:\keil5ARM\ARM\ARMCLANG\Bin\ArmClang.exe" @".\build\main.__i"
"E:\keil5ARM\ARM\ARMCLANG\Bin\ArmClang.exe" @".\build\system_msp432p401r.__i"
"E:\keil5ARM\ARM\ARMCLANG\Bin\ArmClang.exe" @".\build\startup_msp432p401r_uvision.__i"
"E:\keil5ARM\ARM\ARMCLANG\Bin\ArmClang.exe" @".\build\led.__i"
"E:\keil5ARM\ARM\ARMCLANG\Bin\ArmLink" --Via ".\Build\empty.lnp"
