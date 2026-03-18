@echo off
echo setting up virtual drive and calling environement variables
subst w: C:\EngineV2
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" x64
echo %INCLUDE%