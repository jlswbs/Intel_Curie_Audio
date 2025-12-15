@echo off

dfu-util -d 8087:0aba -a 3 -D DSP_Sine_Chili.bin :leave

pause