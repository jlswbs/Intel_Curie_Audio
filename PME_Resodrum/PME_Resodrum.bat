@echo off

dfu-util -d 8087:0aba -a 3 -D PME_Resodrum.bin :leave

pause