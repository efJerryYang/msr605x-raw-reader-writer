@echo off
set "CC_PATH=%USERPROFILE%\Documents\Tools\mingw32\bin"
set "PATH=%CC_PATH%;%PATH%"
echo %PATH%

echo Compiling msr_common.cpp...
g++ -std=c++11 -c msr_common.cpp -o msr_common.o

echo Compiling reader.exe...
g++ -std=c++11 -DREADER_MAIN -o reader.exe msr_reader.cpp msr_common.o -L. -lMSR_API -luser32
if %errorlevel% neq 0 (
    echo Compilation of reader.exe failed.
    exit /b %errorlevel%
)
echo Compilation of reader.exe successful.

echo Compiling writer.exe...
g++ -std=c++11 -DWRITER_MAIN -o writer.exe msr_writer.cpp msr_common.o -L. -lMSR_API -luser32
if %errorlevel% neq 0 (
    echo Compilation of writer.exe failed.
    exit /b %errorlevel%
)
echo Compilation of writer.exe successful.
