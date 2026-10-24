@echo off
cd /d "%~dp0"
python "%~dp0build\build_player_runtime.py" %*
if errorlevel 1 pause
