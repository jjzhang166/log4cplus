@echo off

echo "demo build begin"

if defined VS100COMNTOOLS (call "%VS100COMNTOOLS%\vsvars32.bat")

devenv demo.vcxproj /Rebuild "Release" 
devenv demo.vcxproj /Rebuild "Debug" 

devenv demoS.vcxproj /Rebuild "Release" 
devenv demoS.vcxproj /Rebuild "Debug" 

rmdir /s /q .\debug
rmdir /s /q .\release
rmdir /s /q .\ipch

echo "demo build over"
