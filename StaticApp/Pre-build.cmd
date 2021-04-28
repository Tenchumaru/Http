@ECHO OFF
SETLOCAL

CD /D %~dp0
"%~1mkapp" -a -s StaticApp.txt StaticApp.inl
