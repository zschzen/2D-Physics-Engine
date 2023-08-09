# Add the libs subdirectory to the include path

# Add the SDL2_gfx source files
FILE(GLOB_RECURSE SDL2_GFX_FILES "SDL2_gfx/*.c" "SDL2_gfx/*.h")
target_sources(main PRIVATE ${SDL2_GFX_FILES})