@ECHO OFF
SETLOCAL

CD /D %~dp0
"%~1mkapp" -s StaticApp.txt StaticApp.inl
