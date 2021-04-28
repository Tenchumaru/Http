@ECHO OFF
SETLOCAL

CD /D %~dp0
COPY /Y "%~1packages\openssl-vc140-static-32_64.1.1.1.1\lib\native\libs\%~2\static\%~3\*.pdb" %4
