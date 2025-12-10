@echo off

dfu-util -d 8087:0aba -a 3 -D Misc_SPO_Groove.bin :leave

pause