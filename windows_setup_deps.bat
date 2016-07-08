@ECHO OFF

:update dependencies
@echo:
git submodule init
git submodule update --recursive

@echo:
set /p a="Download mman?(y/n)"
IF %a% EQU y (
	IF NOT EXIST "windows"  mkdir windows 	
	IF NOT EXIST "windows\mman" git clone https://github.com/witwall/mman-win32 windows/mman 
	goto Continue1
	
) ELSE (
	goto Continue1
)


:Continue1
@echo:
set /p a="Download zlib?(y/n)"
IF %a% EQU y (
	IF NOT EXIST "windows" mkdir windows
	IF NOT EXIST "windows\zlib" git clone https://github.com/madler/zlib windows/zlib
	goto Continue2
} ELSE (
	goto Continue2
)


:Continue2
@echo:
set /p a="Download latest protobuf?(y/n)"
IF %a% EQU y (
	IF NOT EXIST "windows" mkdir windows
	IF NOT EXIST "windows\protobuf" git clone https://github.com/google/protobuf windows/protobuf
	goto Continue3
} ELSE (
	goto Continue3
)


:Continue3
@echo:
ECHO Succesfully installed dependencies!