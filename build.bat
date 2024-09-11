@echo off
echo Compiling reader.exe...
g++ -std=c++11 -o reader.exe msr_reader.cpp -L. -lMSR_API -luser32
if %errorlevel% neq 0 (
    echo Compilation of reader.exe failed.
    exit /b %errorlevel%
)
echo Compilation of reader.exe successful.

echo Compiling writer.exe...
g++ -std=c++11 -o writer.exe msr_writer.cpp -L. -lMSR_API -luser32
if %errorlevel% neq 0 (
    echo Compilation of writer.exe failed.
    exit /b %errorlevel%
)
echo Compilation of writer.exe successful.
