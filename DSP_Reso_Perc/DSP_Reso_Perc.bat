@echo off

dfu-util -d 8087:0aba -a 3 -D DSP_Reso_Perc.bin :leave

pause