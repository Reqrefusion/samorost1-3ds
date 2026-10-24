@echo off
setlocal
cd /d "%~dp0"
set "PYTHON=%LOCALAPPDATA%\Programs\Python\Python312-embed\python.exe"
if not exist "%PYTHON%" set "PYTHON=python"
"%PYTHON%" -B scripts\bake_animations.py %*
if errorlevel 1 pause
