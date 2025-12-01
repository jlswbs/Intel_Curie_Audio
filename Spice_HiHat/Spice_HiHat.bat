@echo off

dfu-util -d 8087:0aba -a 3 -D Spice_HiHat.bin :leave

pause