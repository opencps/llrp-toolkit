@echo off

rem this is a batch file copy of the makefile ok
rem allow make argument to work cross-platform

if "%1"=="gen" goto gen
if "%1"=="source" goto source
if "%1"=="llrp" goto all
if "%1"=="clean" goto clean
if "%1"=="" goto all

:gen
cd codegen
javac codegen.java
cd ..\edu\uark\csce\llrp
java -cp ..\..\..\..\codegen codegen ..\..\..\..\codegen\llrp.xml
cd ..\..\..\..
goto end

:source
cd codegen
javac codegen.java
cd ..\edu\uark\csce\llrp
java -cp ..\..\..\..\codegen codegen ..\..\..\..\codegen\llrp.xml
cd ..\..\..\..
copy byhand\*.java edu\uark\csce\llrp
goto end

:clean
rmdir /s /q classes
del edu\uark\csce\llrp\*.java
del edu\uark\csce\llrp\*~
del byhand\*.java~
del codegen\*.class
del codegen\*~
goto end

:all
rmdir /s /q classes
mkdir classes
set PACKAGE=edu\uark\csce\llrp
set SRC=%PACKAGE%\*.java  
javac -g -d classes %SRC%
pushd classes
jar cvf ..\..\lib\llrp.jar edu\uark\csce\llrp
popd
goto end

:use
echo "make clean|source|llrp|all"
goto end

:end
