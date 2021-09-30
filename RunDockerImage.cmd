@ECHO OFF
SETLOCAL

SET IMAGE_NAME=clang-libssl
docker image inspect %IMAGE_NAME% > NUL 2> NUL
IF ERRORLEVEL 1 (
	ECHO %IMAGE_NAME% not available
) ELSE (
	docker run -v C:\local:/local -v %~dp0:/Http %~1 -it %IMAGE_NAME%
)
