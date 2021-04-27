@ECHO OFF
SETLOCAL

SET D=%~1.vs
CD /D %~dp0
FOR %%I IN (key pem) DO IF EXIST "%D%\certificate.%%I" DEL "%D%\certificate.%%I"
IF NOT EXIST "%D%" MD "%D%"
"C:\Program Files\Git\mingw64\bin\openssl.exe" req -batch -x509 -sha256 -nodes -newkey rsa:2048 -keyout "%D%\certificate.key" -out "%D%\certificate.pem"
