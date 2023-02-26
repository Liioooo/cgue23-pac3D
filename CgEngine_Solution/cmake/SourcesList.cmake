cmake_minimum_required(VERSION 3.22)
file(GLOB_RECURSE Header_Files
    "src/CgEngine/*.h"
    "src/Game/*.h"
)
source_group("Header Files" FILES ${Header_Files})
file(GLOB_RECURSE Source_Files
        "src/CgEngine/*cpp"
        "src/Game/*cpp"
)
list(APPEND Source_Files "src/Main.cpp")
source_group("Source Files" FILES ${Source_Files})
set(All_Sources ${Header_Files} ${Source_Files})
