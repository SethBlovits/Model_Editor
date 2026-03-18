@echo off
echo compiling shaders for project
setlocal enabledelayedexpansion

REM Always run from this script's directory
cd /d "%~dp0"

for %%f IN (*.hlsl) DO (
    REM %%~nf 
    fxc /Ges /Od /Zi /T ps_5_0 /E PSMain /Fo %%~nf_ps.cso %%f

    if !errorlevel! neq 0 (
        echo Pixel Shader Failed to Compile!
        echo MAKE SURE TO USE PSMain AS YOUR ENTRY POINT!
    ) else (
        echo Pixel Shader Compilation Succeeded!
    )

    fxc /Ges /Od /Zi /T vs_5_0 /E VSMain /Fo %%~nf_vs.cso %%f

    if !errorlevel! neq 0 (
        echo Vertex Shader Failed to Compile!
        echo MAKE SURE TO USE VSMain AS YOUR ENTRY POINT!
    ) else (
        echo Vertex Shader Compilation Succeded for %%f!
    )
)
endlocal 
echo calling header generation
shader_reflection.exe
echo building exe for project
echo building exe for project
cl.exe /Zi /W4 window.c /Fe:build/window.exe /EHsc /std:c++17 /I"C:/MaterialEditor/cimgui" /I"C:/MaterialEditor/Include" /link cimgui.lib User32.lib d3d12.lib dxgi.lib d3dcompiler.lib dxguid.lib Shell32.lib Comdlg32.lib Shcore.lib
