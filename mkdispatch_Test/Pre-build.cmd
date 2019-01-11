@ECHO OFF
SETLOCAL

SET T=%TEMP%\%RANDOM%.inl
"%~1mkdispatch" > %T%
IF EXIST "%~dp0Dispatch.inl" (
	FC %T% "%~dp0Dispatch.inl" > nul
	IF NOT ERRORLEVEL 1 EXIT /B 0
)
ECHO Creating "%~dp0Dispatch.inl"
MOVE /Y %T% "%~dp0Dispatch.inl"
EXIT /B 0
