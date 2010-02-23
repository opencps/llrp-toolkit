
@echo off

del *.bin *.xml

echo.
echo.
echo.
echo ================================================================
echo == Run dx101 on standard 'a' input to verify function.
echo ================================================================
copy ..\..\Tests\dx101\dx101_a.bin refa.bin > NUL
copy ..\..\Tests\dx101\dx101_a.xml refa.xml > NUL
..\LLRP2XML\bin\Debug\llrp2xml.exe refa.bin   > tmpa-1.xml
..\XML2LLRP\bin\Debug\xml2llrp.exe refa.xml   > tmpa-1.bin
..\XML2LLRP\bin\Debug\xml2llrp.exe tmpa-1.xml > tmpa-2.bin
..\LLRP2LLRP\bin\Debug\llrp2llrp.exe refa.bin > tmpa-3.bin
fc /b refa.bin tmpa-1.bin > NUL
if %errorlevel% neq 0 goto :dx101_a_fail
fc /b refa.bin tmpa-2.bin > NUL
if %errorlevel% neq 0 goto :dx101_a_fail
fc /b refa.bin tmpa-3.bin > NUL
if %errorlevel% neq 0 goto :dx101_a_fail
echo DX101 A -- PASSED
goto :dx101_c
:dx101_a_fail
echo DX101 A -- FAILED



:dx101_c
echo.
echo.
echo.
echo ================================================================
echo == Run dx101 on standard 'c' input to verify function.
echo == Includes tag report variations including a very large report
echo ================================================================
copy ..\..\Tests\dx101\dx101_c.bin refc.bin > NUL
copy ..\..\Tests\dx101\dx101_c.xml refc.xml > NUL
..\LLRP2XML\bin\Debug\llrp2xml.exe refc.bin   > tmpc-1.xml
..\XML2LLRP\bin\Debug\xml2llrp.exe refc.xml   > tmpc-1.bin
..\XML2LLRP\bin\Debug\xml2llrp.exe tmpc-1.xml > tmpc-2.bin
..\LLRP2LLRP\bin\Debug\llrp2llrp.exe refc.bin > tmpc-3.bin
fc /b refc.bin tmpc-1.bin > NUL
if %errorlevel% neq 0 goto :dx101_c_fail
fc /b refc.bin tmpc-2.bin > NUL
if %errorlevel% neq 0 goto :dx101_c_fail
fc /b refc.bin tmpc-3.bin > NUL
if %errorlevel% neq 0 goto :dx101_c_fail
echo DX101 C -- PASSED
goto :dx101_d
:dx101_c_fail
echo DX101 C -- FAILED



:dx101_d
echo.
echo.
echo.
echo ================================================================
echo == Run dx101 on standard 'd' input to verify function.
echo == Includes AccessSpecs of varying shapes and sizes.
echo ================================================================
copy ..\..\Tests\dx101\dx101_d.bin refd.bin > NUL
copy ..\..\Tests\dx101\dx101_d.xml refd.xml > NUL
..\LLRP2XML\bin\Debug\llrp2xml.exe refd.bin   > tmpd-1.xml
..\XML2LLRP\bin\Debug\xml2llrp.exe refd.xml   > tmpd-1.bin
..\XML2LLRP\bin\Debug\xml2llrp.exe tmpd-1.xml > tmpd-2.bin
..\LLRP2LLRP\bin\Debug\llrp2llrp.exe refd.bin > tmpd-3.bin
fc /b refd.bin tmpd-1.bin > NUL
if %errorlevel% neq 0 goto :dx101_d_fail
fc /b refd.bin tmpd-2.bin > NUL
if %errorlevel% neq 0 goto :dx101_d_fail
fc /b refd.bin tmpd-3.bin > NUL
if %errorlevel% neq 0 goto :dx101_d_fail
echo DX101 D -- PASSED
goto :dx101_e
:dx101_d_fail
echo DX101 D -- FAILED



:dx101_e
echo.
echo.
echo.
echo ================================================================
echo == Run dx101 on standard 'e' input to verify function.
echo == Includes Custom parameters at each extension point.
echo ================================================================
copy ..\..\Tests\dx101\dx101_e.bin refe.bin > NUL
copy ..\..\Tests\dx101\dx101_e.xml refe.xml > NUL
..\LLRP2XML\bin\Debug\llrp2xml.exe refe.bin   > tmpe-1.xml
..\XML2LLRP\bin\Debug\xml2llrp.exe refe.xml   > tmpe-1.bin
..\XML2LLRP\bin\Debug\xml2llrp.exe tmpe-1.xml > tmpe-2.bin
..\LLRP2LLRP\bin\Debug\llrp2llrp.exe refe.bin > tmpe-3.bin
fc /b refe.bin tmpe-1.bin > NUL
if %errorlevel% neq 0 goto :dx101_e_fail
fc /b refe.bin tmpe-2.bin > NUL
if %errorlevel% neq 0 goto :dx101_e_fail
fc /b refe.bin tmpe-3.bin > NUL
if %errorlevel% neq 0 goto :dx101_e_fail
echo DX101 E -- PASSED
goto :dx101_f
:dx101_e_fail
echo DX101 E -- FAILED


:dx101_f
echo.
echo.
echo.
echo ================================================================
echo == Run dx101 on standard 'f' input to verify function.
echo == XML with extra whitespace etc
echo ================================================================
copy ..\..\Tests\dx101\dx101_f.bin reff.bin > NUL
copy ..\..\Tests\dx101\dx101_f.xml reff.xml > NUL
..\LLRP2XML\bin\Debug\llrp2xml.exe reff.bin   > tmpf-1.xml
..\XML2LLRP\bin\Debug\xml2llrp.exe reff.xml   > tmpf-1.bin
..\XML2LLRP\bin\Debug\xml2llrp.exe tmpf-1.xml > tmpf-2.bin
..\LLRP2LLRP\bin\Debug\llrp2llrp.exe reff.bin > tmpf-3.bin
fc /b reff.bin tmpf-1.bin > NUL
if %errorlevel% neq 0 goto :dx101_f_fail
fc /b reff.bin tmpf-2.bin > NUL
if %errorlevel% neq 0 goto :dx101_f_fail
fc /b reff.bin tmpf-3.bin > NUL
if %errorlevel% neq 0 goto :dx101_f_fail
echo DX101 F -- PASSED
goto :eof
:dx101_f_fail
echo DX101 F -- FAILED
