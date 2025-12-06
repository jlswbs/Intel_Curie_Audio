@echo off

dfu-util -d 8087:0aba -a 3 -D Osc_Quadrature.bin :leave

pause