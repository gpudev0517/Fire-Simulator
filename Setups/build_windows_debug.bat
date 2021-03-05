@echo off
echo ..Building FRI3D Windows Setup

set QTVERSION=5_11_1

if "%1" == "" goto qtdefault
if NOT %1 == "" goto qtdir

:qtdir
set QTDIR=%1

:qtdefault
set QTDIR=D:\Qt\5.15.0\msvc2019_64\



set DATAFOLDER=..\Data
set TARGETFOLDER=Generator\packages\FRI3D\Debug
set EXTERNALLIBSFOLDER=..\Build\Debug
set EXTERNALNDYNLIBSFOLDER=..\NDYN_Debug\lib
set EXTERNALWINLIBSFOLDER=..\NDYN_Debug\External\windows\lib
set QTBINDIR=%QTDIR%\bin
set JOM=%QTDIR%\..\..\Tools\QtCreator\bin\jom.exe

echo rmdir /s /q .\%TARGETFOLDER%
rmdir /s /q .\%TARGETFOLDER%
REM set /p DUMMY=Hit ENTER to continue...

echo rmdir /s /q %DATAFOLDER%\FRI3D\untitled
rmdir /s /q %DATAFOLDER%\FRI3D\untitled


set BINARYPATH=%EXTERNALLIBSFOLDER%\FRI3D.exe
set VCDIR=C:\Program Files (x86)\Microsoft Visual Studio\2019\Enterprise\VC\Auxiliary\Build\
set QMAKESPEC=win32-msvc2019
set vc=vc142

set OLDDIR=%CD%


echo %VCDIR%
set VCENV="%VCDIR%vcvarsall.bat"
if not defined DevEnvDir (
    @call %VCENV% x64
)

rem echo ..Building Neutrino...
rem @echo off
rem chdir /d %BINARYPATH%/..
rem %QTBINDIR%\qmake ..\Code\Neutrino.pro -r -spec %QMAKESPEC% "USE_GUI=true"
rem %JOM% -j 8 install
chdir /d %OLDDIR%


echo ..Making Target folder %TARGETFOLDER%
@echo off
mkdir %TARGETFOLDER%
mkdir %TARGETFOLDER%\platforms

echo ..Collecting files
@echo off
xcopy %DATAFOLDER% %TARGETFOLDER% /E /I /Q /R /Y
xcopy %BINARYPATH% %TARGETFOLDER% /E /I /Q /R /Y

xcopy Generator\Utility\VC14\vc14_redist_x64.exe %TARGETFOLDER% /I /Q /R /Y
xcopy Generator\Utility\VC14\AccessDatabaseEngine_X64.exe %TARGETFOLDER% /I /Q /R /Y
xcopy Generator\Utility\VC14\msvcp110.dll %TARGETFOLDER% /I /Q /R /Y
xcopy Generator\Utility\VC14\msvcr110.dll %TARGETFOLDER% /I /Q /R /Y
xcopy Generator\Utility\VC14\msvcp140.dll %TARGETFOLDER% /I /Q /R /Y
xcopy Generator\Utility\VC14\vcomp140.dll %TARGETFOLDER% /I /Q /R /Y
xcopy Generator\Utility\VC14\vcruntime140.dll %TARGETFOLDER% /I /Q /R /Y

xcopy Generator\Utility\FRI3D.bat %TARGETFOLDER% /I /Q /R /Y
xcopy Generator\Utility\Shortcut.exe %TARGETFOLDER% /I /Q /R /Y

xcopy %EXTERNALLIBSFOLDER%\*.* %TARGETFOLDER% /I /Q /R /Y
xcopy %EXTERNALLIBSFOLDER%\Scripts\* %TARGETFOLDER%\Scripts /I /Q /R /Y
xcopy %EXTERNALLIBSFOLDER%\x64\* %TARGETFOLDER%\x64 /I /Q /R /Y
xcopy %EXTERNALLIBSFOLDER%\x86\* %TARGETFOLDER%\x86 /I /Q /R /Y


xcopy %EXTERNALNDYNLIBSFOLDER%\*.dll %TARGETFOLDER% /I /Q /R /Y
xcopy %EXTERNALWINLIBSFOLDER%\*.dll %TARGETFOLDER% /I /Q /R /Y

xcopy %QTBINDIR%\Qt5QuickTemplates2d.dll %TARGETFOLDER% /I /Q /R /Y
xcopy %QTBINDIR%\Qt5QuickControls2d.dll %TARGETFOLDER% /I /Q /R /Y
xcopy %QTBINDIR%\Qt5QuickWidgetsd.dll %TARGETFOLDER% /I /Q /R /Y
xcopy %QTBINDIR%\Qt5Quickd.dll %TARGETFOLDER% /I /Q /R /Y
xcopy %QTBINDIR%\Qt5QmlWorkerScriptd.dll %TARGETFOLDER% /I /Q /R /Y
xcopy %QTBINDIR%\Qt5QmlModelsd.dll %TARGETFOLDER% /I /Q /R /Y
xcopy %QTBINDIR%\Qt5Qmld.dll %TARGETFOLDER% /I /Q /R /Y

xcopy %QTBINDIR%\Qt5Widgetsd.dll %TARGETFOLDER% /I /Q /R /Y
xcopy %QTBINDIR%\Qt5PrintSupportd.dll %TARGETFOLDER% /I /Q /R /Y
xcopy %QTBINDIR%\Qt5Networkd.dll %TARGETFOLDER% /I /Q /R /Y
xcopy %QTBINDIR%\Qt5Guid.dll %TARGETFOLDER% /I /Q /R /Y
xcopy %QTBINDIR%\Qt5Cored.dll %TARGETFOLDER% /I /Q /R /Y

xcopy %QTBINDIR%\Qt5Opengld.dll %TARGETFOLDER% /I /Q /R /Y
xcopy %QTBINDIR%\Qt5Xmld.dll %TARGETFOLDER% /I /Q /R /Y


xcopy %QTDIR%\plugins\platforms\qwindowsd.dll %TARGETFOLDER%\platforms /E /I /Q /R /Y
xcopy %QTDIR%\plugins\styles %TARGETFOLDER%\styles /E /I /Q /R /Y


xcopy %QTDIR%\qml\QtQml %TARGETFOLDER%\QtQml /E /I /Q /R /Y
xcopy %QTDIR%\qml\QtGraphicalEffects %TARGETFOLDER%\QtGraphicalEffects /E /I /Q /R /Y

xcopy %QTDIR%\qml\QtQuick %TARGETFOLDER%\QtQuick % /E /I /Q /R /Y 
xcopy %QTDIR%\qml\QtQuick.2 %TARGETFOLDER%\QtQuick.2 /E /I /Q /R /Y

rem xcopy %QTBINDIR%\icudt54.dll %TARGETFOLDER% /I /Q /R /Y
rem xcopy %QTBINDIR%\icuin54.dll %TARGETFOLDER% /I /Q /R /Y
rem xcopy %QTBINDIR%\icuuc54.dll %TARGETFOLDER% /I /Q /R /Y


rem xcopy Generator\Utility\Intel\Shared_Libraries\redist\intel64_win\compiler\libiomp5md.dll %TARGETFOLDER% /I /Q /R /Y
rem xcopy Generator\Utility\Intel\Shared_Libraries\redist\intel64_win\compiler\libmmd.dll %TARGETFOLDER% /I /Q /R /Y

rem xcopy %windir%\system32\OpenCL.dll %TARGETFOLDER% /I /Q /R /Y
rem xcopy "%CUDA_PATH%\bin\cudart64_80.dll" %TARGETFOLDER% /I /Q /R /Y

echo ..Building FRI3D_Windows.exe
@echo off
set dt=%date:~10,4%-%date:~4,2%-%date:~7,2%
Generator\qtif_win\binarycreator.exe --offline-only -c Generator/config/config_win.xml -p Generator/packages Windows/%dt%_FRI3D_Windows_%vc%.exe

rem rem Generator\qtif_win\repogen.exe -p  Generator/packages/ Neutrino_Windows_%vc%
rem rem Generator\qtif_win\binarycreator.exe --online-only -c Generator/config/config_win_online.xml -p Generator/packages Windows/FRI3D_Windows_%vc%.exe

echo FRI3D_Windows.exe is generated under /Windows
