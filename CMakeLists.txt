cmake_minimum_required( VERSION 3.18.0 )
cmake_policy(SET CMP0075 NEW)
cmake_policy(SET CMP0091 NEW)
cmake_policy(SET CMP0015 NEW)
project( 2D-Physics-Engine-Pikuma )

set(CMAKE_VERBOSE_MAKEFILE on)
set( CMAKE_CXX_STANDARD 17 CACHE STRING "" FORCE )
set( CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE BOOL "" FORCE )

if( ${CMAKE_SYSTEM_NAME} MATCHES "Android" )
    add_library( main SHARED )
else()
    add_executable( main )
    set_target_properties( main PROPERTIES OUTPUT_NAME ${CMAKE_PROJECT_NAME} )
    add_custom_command( TARGET main POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/assets
        $<CONFIG>/assets
        )
endif()

add_subdirectory( "src" )
add_subdirectory( "lib" )
add_subdirectory( "vendor" )

if( ${CMAKE_SYSTEM_NAME} MATCHES "Emscripten" )
    target_link_options( main PRIVATE "--emrun -s DEMANGLE_SUPPORT=1 --preload-file ${CMAKE_SOURCE_DIR}/assets --bind" )
    target_link_options( main PRIVATE "-sASYNCIFY" )
    set(CMAKE_EXECUTABLE_SUFFIX .html)
    set_target_properties( main PROPERTIES OUTPUT_NAME index )
endif()

if( ${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    set( CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>" )
    set_property( DIRECTORY PROPERTY VS_STARTUP_PROJECT ${CMAKE_PROJECT_NAME} )
    set_property( TARGET main PROPERTY WIN32_EXECUTABLE true )
    target_link_options( main PRIVATE -static -static-libgcc -static-libstdc++ )
endif()