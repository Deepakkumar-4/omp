@echo off
setlocal

echo ========================================================================
echo        RUNNING COMPLETE OPENMP ALGORITHM AND CODE TEST SUITE
echo ========================================================================
echo.

where make >nul 2>&1
if %ERRORLEVEL% EQU 0 (
    make -f Makefile.test_all test-all
    goto end
)

where mingw32-make >nul 2>&1
if %ERRORLEVEL% EQU 0 (
    mingw32-make -f Makefile.test_all test-all
    goto end
)

echo [ERROR] Neither 'make' nor 'mingw32-make' was found in PATH.
echo Please ensure MinGW or MSYS2 is in your environment PATH.

:end
endlocal

