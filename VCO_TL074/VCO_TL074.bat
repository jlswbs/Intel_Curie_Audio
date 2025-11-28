@echo off

dfu-util -d 8087:0aba -a 3 -D VCO_TL074.bin :leave

pause