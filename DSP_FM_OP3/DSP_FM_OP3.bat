@echo off

dfu-util -d 8087:0aba -a 3 -D DSP_FM_OP3.bin :leave

pause