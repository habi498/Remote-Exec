cmake_policy(SET CMP0091 NEW)

cmake_minimum_required(VERSION 3.16)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

project("remexec")
add_library(remexec MODULE "main.cpp" "Core.cpp")
set_target_properties(remexec PROPERTIES PREFIX "")
set(CMAKE_BUILD_TYPE Release)

if(FORCE_32)
	set_target_properties(remexec PROPERTIES COMPILE_FLAGS "-m32" LINK_FLAGS "-m32")
	set_target_properties(remexec PROPERTIES OUTPUT_NAME "remexec04rel32")
elseif(CMAKE_SIZEOF_VOID_P EQUAL 8)
	set_target_properties(remexec PROPERTIES OUTPUT_NAME "remexec04rel64")
else()
	set_target_properties(remexec PROPERTIES OUTPUT_NAME "remexec04rel32")
endif()
add_custom_command(TARGET remexec POST_BUILD 
  COMMAND "${CMAKE_COMMAND}" -E copy 
     "$<TARGET_FILE:remexec>"
     "${CMAKE_CURRENT_SOURCE_DIR}/out/binaries/$<TARGET_FILE_NAME:remexec>" 
  COMMENT "Copying to output directory")
