@echo off

rem Variables to set
set OPTIMISED_FLAG=true
set INTERNAL_FLAG=false
set ALLOW_ASSERTS=false
set LANE_WIDTH=1

set USE_SDL=false

rem Change active directory
cd %~dp0

rem Require Visual Studio's cl
where cl >nul 2>nul
IF %ERRORLEVEL% NEQ 0 (
    echo "Error: cl is not in the path. Please setup Visual Studio to do cl builds by calling vcvarsall.bat"
    goto skipEverything
)

rem Warnings to ignore
set WARNINGS=-wd4189 -wd4706 -wd4996 -wd4100 -wd4127 -wd4267 -wd4505 -wd4820 -wd4365 -wd4514 -wd4062 -wd4061 -wd4668 -wd4389 -wd4018 -wd4711 -wd4987 -wd4710 -wd4625 -wd4626 -wd4350 -wd4826 -wd4640 -wd4571 -wd4986 -wd4388 -wd4129 -wd4201 -wd4577 -wd4244 -wd4623 -wd4204 -wd4101 -wd4255 -wd4191 -wd4477 -wd4242 -wd4464 -wd5045 -wd5220
set LIBS=kernel32.lib user32.lib gdi32.lib 

rem INTERNAL macro
if "%INTERNAL_FLAG%"=="true" (
    set INTERNAL=-DINTERNAL=1
) else (
    set INTERNAL=-DINTERNAL=0
)

rem Enable/disable ASSERTs
if "%ALLOW_ASSERTS%"=="true" (
    set ALLOW_ASSERTS=-DALLOW_ASSERTS=1
) else (
    set ALLOW_ASSERTS=-DALLOW_ASSERTS=0
)

rem Use SDL. For testing only really.
if "%USE_SDL%"=="true" (
    set USE_SDL=-DUSE_SDL=1
    set LIBS=%LIBS% SDL2.lib SDL2main.lib SDL2test.lib shell32.lib
) else (
    set USE_SDL=-DUSE_SDL=0
)

rem Compiler flags
set COMPILER_FLAGS=-nologo -Gm- -GR- %WARNINGS% -FC -Zi -Oi -GS- -Gs9999999 -Wall %INTERNAL% %ALLOW_ASSERTS% %USE_SDL% -DLANE_WIDTH=%LANE_WIDTH% -DRANDOM_SAMPLE_ANTI_ALIASING=1

rem Optimised
if "%OPTIMISED_FLAG%"=="true" (
    set COMPILER_FLAGS=%COMPILER_FLAGS% -MT -fp:fast -EHa- -O2
) else (
    set COMPILER_FLAGS=%COMPILER_FLAGS% -MTd -EHa- -Od
)

rem Pass -arch:AVX2 to prevent vunpcklps call from using best (causes issues on older architectures).
if "%LANE_WIDTH%"=="8" (
    set COMPILER_FLAGS=%COMPILER_FLAGS% -arch:AVX2
)

rem Make build directory.
IF NOT EXIST "build" mkdir "build"

rem Build raytracer
pushd "build"
echo Building raytracer
cl -Feraytracer %COMPILER_FLAGS% "../src/build.cpp" -FmMirror.map -link %LIBS% -stack:0x100000,0x100000 -subsystem:windows,5.2
popd

:skipEverything