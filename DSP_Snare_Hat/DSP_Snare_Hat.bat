@echo off

dfu-util -d 8087:0aba -a 3 -D DSP_Snare_Hat.bin :leave

pause