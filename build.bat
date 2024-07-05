@echo off

rem download mingw here: https://github.com/skeeto/w64devkit

gcc examples/noise.c src/cri_common.c src/cri_win.c -Os -s -std=c99 -Iinclude -lgdi32 -lwinmm -ldwmapi -lole32 -o noise.exe
