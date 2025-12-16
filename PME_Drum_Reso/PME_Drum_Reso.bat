@echo off

dfu-util -d 8087:0aba -a 3 -D PME_Drum_Reso.bin :leave

pause