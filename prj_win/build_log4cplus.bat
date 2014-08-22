@echo off
::¶¯Ì¬¿â
echo "log4cplus build begin"

if defined VS100COMNTOOLS (call "%VS100COMNTOOLS%\vsvars32.bat")

devenv log4cplus.vcxproj /Rebuild "Release" 
devenv log4cplus.vcxproj /Rebuild "Debug" 

rmdir /s /q .\debug
rmdir /s /q .\release
rmdir /s /q .\ipch

echo "log4cplus build over"
