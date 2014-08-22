@echo off
::¾²Ì¬¿â
echo "log4cplusS build begin"

if defined VS100COMNTOOLS (call "%VS100COMNTOOLS%\vsvars32.bat")

devenv log4cplusS.vcxproj /Rebuild "Release" 
devenv log4cplusS.vcxproj /Rebuild "Debug"

rmdir /s /q .\debug
rmdir /s /q .\release
rmdir /s /q .\ipch

echo "log4cplusS build over"
