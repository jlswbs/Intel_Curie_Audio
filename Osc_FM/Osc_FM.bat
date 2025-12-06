@echo off

dfu-util -d 8087:0aba -a 3 -D Osc_FM.bin :leave

pause