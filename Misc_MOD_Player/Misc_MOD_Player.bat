@echo off

dfu-util -d 8087:0aba -a 3 -D Misc_MOD_Player.bin :leave

pause