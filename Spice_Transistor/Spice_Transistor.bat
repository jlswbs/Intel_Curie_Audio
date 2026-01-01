@echo off

dfu-util -d 8087:0aba -a 3 -D Spice_Transistor.bin :leave

pause