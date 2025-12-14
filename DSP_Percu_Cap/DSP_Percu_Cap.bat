@echo off

dfu-util -d 8087:0aba -a 3 -D DSP_Percu_Cap.bin :leave

pause