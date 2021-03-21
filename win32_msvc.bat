@echo off

rem Variables to set
set OPTIMISED_FLAG=true
set INTERNAL_FLAG=false
set ALLOW_ASSERTS=false
set LANE_WIDTH=8
set DEBUG_WINDOW_FLAG=false
set USE_OPENGL_WINDOW_FLAG=true
set OPENCL_FLAG=false

rem Change active directory
cd %~dp0

rem Require Visual Studio's cl
where cl >nul 2>nul
IF %ERRORLEVEL% NEQ 0 (
    echo "Error: cl is not in the path. Please setup Visual Studio to do cl builds by calling vcvarsall.bat"
    goto skipEverything
)

set WARNINGS=-wd4189 -wd4706 -wd4996 -wd4100 -wd4127 -wd4267 -wd4505 -wd4820 -wd4365 -wd4514 -wd4062 -wd4061 -wd4668 -wd4389 -wd4018 -wd4711 -wd4987 -wd4710 -wd4625 -wd4626 -wd4350 -wd4826 -wd4640 -wd4571 -wd4986 -wd4388 -wd4129 -wd4201 -wd4577 -wd4244 -wd4623 -wd4204 -wd4101 -wd4255 -wd4191 -wd4477 -wd4242 -wd4464 -wd5045 -wd5220
set LIBS=kernel32.lib user32.lib gdi32.lib opengl32.lib

if "%OPENCL_FLAG%"=="true" (
    rem Link to opencl dynamically?
    set LIBS=%LIBS% opencl.lib
)

if "%INTERNAL_FLAG%"=="true" (
    set INTERNAL=-DINTERNAL=1
) else (
    set INTERNAL=-DINTERNAL=0
)

if "%ALLOW_ASSERTS%"=="true" (
    set ALLOW_ASSERTS=-DALLOW_ASSERTS=1
) else (
    set ALLOW_ASSERTS=-DALLOW_ASSERTS=0
)

if "%DEBUG_WINDOW_FLAG%"=="true" (
    set DEBUG_WINDOW=-DDEBUG_WINDOW=1
) else (
    set DEBUG_WINDOW=-DDEBUG_WINDOW=0
)

if "%USE_OPENGL_WINDOW_FLAG%"=="true" (
    set USE_OPENGL_WINDOW=-DUSE_OPENGL_WINDOW=1
) else (
    set USE_OPENGL_WINDOW=-DUSE_OPENGL_WINDOW=0
)

if "%OPENCL_FLAG%"=="true" (
    set USE_OPENCL=-DUSE_OPENCL=1
) else (
    set USE_OPENCL=-DUSE_OPENCL=0
)

set COMPILER_FLAGS=-nologo -Gm- -GR- %WARNINGS% -FC -Zi -Oi -GS- -Gs9999999 -Wall %INTERNAL% %ALLOW_ASSERTS% %DEBUG_WINDOW% %USE_OPENCL% %USE_OPENGL_WINDOW% -DLANE_WIDTH=%LANE_WIDTH% -DRANDOM_SAMPLE_ANTI_ALIASING=1

if "%OPTIMISED_FLAG%"=="true" (
    set COMPILER_FLAGS=%COMPILER_FLAGS% -MT -fp:fast -EHa- -O2
) else (
    set COMPILER_FLAGS=%COMPILER_FLAGS% -MTd -EHa- -Od
)

rem Pass -arch:AVX2 to prevent vunpcklps call from using best (causes issues on older architectures).
if "%LANE_WIDTH%"=="8" (
    set COMPILER_FLAGS=%COMPILER_FLAGS% -arch:AVX2
)

IF NOT EXIST "build" mkdir "build"

pushd "build"
echo Building raytracer

del *_raytracer.pdb > NUL > NUL

cl -LD -FeRaytracer %COMPILER_FLAGS% "../src/build.cpp" -FmRaytracer.map -link -EXPORT:handle_input_and_render -PDB:%random%_raytracer.pdb -stack:0x100000,0x100000 -subsystem:windows,5.2
cl -FeWin32 %COMPILER_FLAGS% "../src/platform_win32.cpp" -FmWin32.map -link %LIBS% -stack:0x100000,0x100000 -subsystem:windows,5.2
popd

:skipEverything