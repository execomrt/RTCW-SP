@echo off
mkdir build
cd build
cmake -G "Visual Studio 17 2022" -A Win32 .. -DPROJECT_SUFFIX="_win64_vs2022" -DOUTPUT_PATH="../bin"
cd ..
echo Visual Studio project generated in build/
pause
