@echo off
%1
set PATH=%~dp0;%PATH%
cd /D %~dp0
vldlog2tasks.exe %2 %3 %4 %5
