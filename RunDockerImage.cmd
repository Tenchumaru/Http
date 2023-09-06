@ECHO OFF
SETLOCAL

SET CONTAINER_NAME=Http
SET IMAGE_NAME=development
docker inspect %CONTAINER_NAME% > NUL
IF ERRORLEVEL 1 (
	docker image inspect %IMAGE_NAME% > NUL 2> NUL
	IF ERRORLEVEL 1 (
		ECHO %IMAGE_NAME% not available
	) ELSE (
		docker run -v C:\local:/local -v %~dp0..\Yarborough:/Yarborough -v %~dp0:/Http --name %CONTAINER_NAME% -it --privileged %~1 %IMAGE_NAME%
	)
) ELSE (
	docker inspect %CONTAINER_NAME% | find "Status" | find "running" > NUL
	IF ERRORLEVEL 1 (
		docker start %CONTAINER_NAME%
	)
	docker attach %CONTAINER_NAME%
)
