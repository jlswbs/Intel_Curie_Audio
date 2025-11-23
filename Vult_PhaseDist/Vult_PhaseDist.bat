@echo off

dfu-util -d 8087:0aba -a 3 -D Vult_PhaseDist.bin :leave

pause