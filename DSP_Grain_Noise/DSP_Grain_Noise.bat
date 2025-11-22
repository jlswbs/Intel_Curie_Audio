@echo off

dfu-util -d 8087:0aba -a 3 -D DSP_Grain_Noise.bin :leave

pause