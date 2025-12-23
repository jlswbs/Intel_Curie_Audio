@echo off

dfu-util -d 8087:0aba -a 3 -D Misc_SamplePWMshift.bin :leave

pause