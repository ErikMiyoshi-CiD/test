@ECHO OFF

SET TARGET=iDProx.bin
SET STLINKCLI=%1

CALL :reset
if %ERRORLEVEL% == 1 EXIT /B 1
CALL :full-erase
if %ERRORLEVEL% == 1 EXIT /B 1
CALL :flash-target
if %ERRORLEVEL% == 1 EXIT /B 1
CALL :reset
if %ERRORLEVEL% == 1 EXIT /B 1
EXIT /B 0

:: Erase all the Flash memory sectors of the connected device
:full-erase
ECHO Erasing all the Flash memory sectors
%STLINKCLI% -c SWD UR -ME -Q
if %ERRORLEVEL% == 0 EXIT /B 0
EXIT /B 1

:: Flashes target to address 0x08000000
:flash-target
ECHO Flashing %TARGET%
%STLINKCLI% -c SWD UR -P %TARGET% 0x08000000 ske -V -Q
if %ERRORLEVEL% == 0 EXIT /B 0
EXIT /B 1


:: Reset
:reset
ECHO Reseting device
%STLINKCLI% -c SWD UR -Rst -Q
if %ERRORLEVEL% == 0 EXIT /B 0
EXIT /B 1