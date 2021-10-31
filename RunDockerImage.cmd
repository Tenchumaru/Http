@ECHO OFF
SETLOCAL

SET IMAGE_NAME=development
docker image inspect %IMAGE_NAME% > NUL 2> NUL
IF ERRORLEVEL 1 (
	ECHO %IMAGE_NAME% not available
) ELSE (
	docker run -v C:\local:/local -v %~dp0..\Yarborough:/Yarborough -v %~dp0:/Http --name Http -it --privileged %~1 %IMAGE_NAME%
)
