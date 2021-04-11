@echo off
rem `@echo off` tells windows to not print everyline of this batch file

rem From the path to this script file get the path to the root of the project
SET PROJECT_DIR_PATH=%~dp0

if DEFINED VS2017_ISREADY goto BuildProject

rem Setup Environment variables for VS2017
call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\Common7\Tools\VsDevCmd.bat"
set VS2017_ISREADY="true"

:BuildProject

if "%1" == "" (
    call:chat_writer
    call:chat_server
	call:chat_browser
    goto:eof
)

if "%1" == "chat_writer" (
    call:chat_writer
    goto:eof
)

if "%1" == "chat_server" (
    call:chat_server
    goto:eof
)

if "%1" == "chat_browser" (
    call:chat_browser
    goto:eof
)

if "%1" == "clean" (
    call:clean
    goto:eof
)

if "%1" == "all" (
	call:chat_browser
	call:chat_server
	call:chat_writer
	goto:eof
)

echo Unkown command given : "%1"
echo expected nothing, chat_writer, chat_server, chat_browser, or clean
goto:eof

:chat_writer
    @echo on
    call cl /permissive- /GS /GL /analyze- /W4 /Gy /Zc:wchar_t /Zi /Gm- /O2 /sdl /Zc:inline /fp:precise /WX /Zc:forScope /Gd /Oy- /Oi /MD /std:c++17 /FC /EHsc /nologo /diagnostics:classic -Fechat_writer chat_writer.cpp sockets.cpp Ws2_32.lib
    @echo off
goto:eof

:chat_server
    @echo on
    call cl /permissive- /GS /GL /analyze- /W4 /Gy /Zc:wchar_t /Zi /Gm- /O2 /sdl /Zc:inline /fp:precise /WX /Zc:forScope /Gd /Oy- /Oi /MD /std:c++17 /FC /EHsc /nologo /diagnostics:classic -Fechat_server chat_server.cpp sockets.cpp Ws2_32.lib
    @echo off
goto:eof

:chat_browser
    @echo on
    call cl /permissive- /GS /GL /analyze- /W4 /Gy /Zc:wchar_t /Zi /Gm- /O2 /sdl /Zc:inline /fp:precise /WX /Zc:forScope /Gd /Oy- /Oi /MD /std:c++17 /FC /EHsc /nologo /diagnostics:classic -Fechat_browser chat_browser.cpp sockets.cpp Ws2_32.lib
    @echo off
goto:eof

:clean
    @echo on
    call del /Q *.obj *.pdb *.exe
    @echo off
goto:eof