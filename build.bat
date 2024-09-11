@echo off
echo Compiling msr_reader.exe...
g++ -std=c++11 -o msr_reader.exe main.cpp -L. -lMSR_API -luser32
if %errorlevel% neq 0 (
    echo Compilation failed.

    exit /b %errorlevel%
)
echo Compilation successful. msr_reader.exe has been created.
