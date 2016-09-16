@ECHO OFF 
ECHO CMAKE SCRIPT TO BUILD THIS SYSTEM
ECHO =================================
FOR /D %%p IN ("..\build\*.*") DO rmdir "%%p" /s /q
cd ..\build
cmake -G "Visual Studio 12" ..
cd ..\build_script

