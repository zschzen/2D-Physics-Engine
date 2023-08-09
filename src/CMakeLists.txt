
FILE(GLOB_RECURSE SRC_FILES *.cpp *.h *.hpp)

target_sources( main PRIVATE
    "main.cpp"
    ${SRC_FILES}
    )
