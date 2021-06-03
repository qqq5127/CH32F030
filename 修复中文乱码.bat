@echo off
set DIR=D:\WCH\CH32F103
for /R %DIR% %%i in (*.java *.xml) do (
echo %%i
native2ascii -encoding UTF-8 %%i %DIR%\temp
native2ascii -reverse %DIR%\temp %%i
)
pause