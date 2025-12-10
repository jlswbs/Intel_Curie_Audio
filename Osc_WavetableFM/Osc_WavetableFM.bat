@echo off

dfu-util -d 8087:0aba -a 3 -D Osc_WavetableFM.bin :leave

pause