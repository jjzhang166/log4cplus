@echo off

echo "demo build begin"

if defined VS100COMNTOOLS (call "%VS100COMNTOOLS%\vsvars32.bat")

devenv log4cplus_demo.vcxproj /Rebuild "Release" 
devenv log4cplus_demo.vcxproj /Rebuild "Debug" 

devenv log4cplusS_demo.vcxproj /Rebuild "Release" 
devenv log4cplusS_demo.vcxproj /Rebuild "Debug" 

devenv log4cpluswapperS_demo.vcxproj /Rebuild "Release" 
devenv log4cpluswapperS_demo.vcxproj /Rebuild "Debug" 

rmdir /s /q .\debug
rmdir /s /q .\release
rmdir /s /q .\ipch

echo "demo build over"
