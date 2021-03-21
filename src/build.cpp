#include "common_includes.cpp"

#include "raytracer.cpp"
#include "raytracer_cpu.cpp"
#include "scene_builder.cpp"
#include "main.cpp"

#if OS_WIN32
// Do nothing - we split Windows into DLL and EXE.
#elif OS_LINUX
// TODO: Certainly don't need all these...
#include <X11/Xos.h>
#include <GL/glx.h>
#include <sys/mman.h>
#include <sys/sysinfo.h>
#include <pthread.h>
#include "platform_linux.cpp"
#else
#error "Unknown OS"
#endif
