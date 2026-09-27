@echo off
set /p PORT=Enter Arduino Nano COM Port (e.g. COM3): 

for /f "delims=" %%i in ('dir /b /s "%LOCALAPPDATA%\Arduino15\packages\arduino\tools\avrdude\avrdude.exe"') do set AVRDUDE="%%i"
for /f "delims=" %%i in ('dir /b /s "%LOCALAPPDATA%\Arduino15\packages\arduino\tools\avrdude\avrdude.conf"') do set CONF="%%i"

echo Found AVRDUDE at: %AVRDUDE%
echo Found CONF at: %CONF%

%AVRDUDE% -C %CONF% -c stk500v1 -P %PORT% -b 19200 -p t85 -U lfuse:w:0xe1:m -U hfuse:w:0xdd:m -U efuse:w:0xfe:m -U flash:w:"C:\t85_default.hex":i

pause