@ECHO OFF
SETLOCAL

SET D=%~dp0
SET MKDISPATCH="%~1mkdispatch"
SET T=%TEMP%\%RANDOM%.inl
CALL :one my
CALL :one seg
CALL :one sm
EXIT /B 0

:one
%MKDISPATCH% -p %1 "%D%twitch.txt" > %T%
IF EXIST "%D%%1Dispatch.inl" (
	FC %T% "%D%%1Dispatch.inl" > nul
	IF NOT ERRORLEVEL 1 EXIT /B 0
)
ECHO Creating "%D%%1Dispatch.inl"
MOVE /Y %T% "%D%%1Dispatch.inl"
EXIT /B 0
