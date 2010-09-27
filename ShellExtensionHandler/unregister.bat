dir .\ReleaseMinDependency\OpenWithExt.dll

regsvr32 /u .\ReleaseMinDependency\OpenWithExt.dll
pskill explorer.exe
call explorer.exe
