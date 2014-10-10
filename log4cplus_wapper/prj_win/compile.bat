@echo off

echo "log4cplusWapperS build begin"

if defined VS100COMNTOOLS (call "%VS100COMNTOOLS%\vsvars32.bat")

devenv log4cplusWapperS.vcxproj /Rebuild "Release" 
devenv log4cplusWapperS.vcxproj /Rebuild "Debug" 

rmdir /s /q .\debug
rmdir /s /q .\release
rmdir /s /q .\ipch

echo "log4cplusWapperS build over"
