cmake_minimum_required(VERSION 3.0)
project(ftp-server LANGUAGES C)

set(SRC
	src/console.c
	src/file.c
	src/ftp.c
	src/ftp_commands.c
	src/main.c
	src/network.c
	src/utils.c
)
include_directories(src)

find_program(GDB "gdb")

add_executable(server ${SRC})
add_custom_target(run COMMAND ./server -p 2121)
add_custom_target(run-gdb COMMAND ${GDB} --args ./server -p 2121)
