@echo off
setlocal

echo ========================================================================
echo               OPENMP SYSTEM AND COMPILER DIAGNOSTIC
echo ========================================================================
echo.

where g++ >nul 2>&1
if %ERRORLEVEL% NEQ 0 (
    echo [ERROR] g++ compiler not found in system PATH!
    echo Please install MinGW-w64 or MSYS2 and add g++ to your PATH.
    pause
    exit /b 1
)

echo [OK] g++ compiler found.
echo Compiling check_openmp.cpp with -fopenmp...

g++ -O3 -fopenmp -std=c++17 -o check_openmp.exe check_openmp.cpp
if %ERRORLEVEL% NEQ 0 (
    echo.
    echo [ERROR] Compilation failed!
    echo Reason: 'omp_get_wtime' is undefined when '-fopenmp' is missing.
    echo Please compile with: g++ -O3 -fopenmp -std=c++17 check_openmp.cpp
    pause
    exit /b 1
)

echo [OK] Compilation successful!
echo.
echo Running diagnostic suite...
echo.

check_openmp.exe

endlocal
