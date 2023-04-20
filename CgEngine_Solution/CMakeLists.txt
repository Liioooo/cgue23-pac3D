cmake_minimum_required(VERSION 3.23)

function(find_additional_cmake_file outputVariableName filename)
    if (NOT DEFINED outputVariableName OR NOT DEFINED filename)
        message(FATAL_ERROR "Wrong usage of find_and_include_additional_cmake_file(). Need both arguments.")
    endif ()
    find_file(${outputVariableName} ${filename}
            PATHS "../cmake/" "../cmake_gl/" "./cmake/"
            NO_PACKAGE_ROOT_PATH NO_CMAKE_PATH NO_CMAKE_ENVIRONMENT_PATH NO_SYSTEM_ENVIRONMENT_PATH NO_CMAKE_SYSTEM_PATH)
    if (NOT ${outputVariableName})
        message(FATAL_ERROR "Could not find ${filename}")
    endif ()
endfunction()

find_additional_cmake_file(SOURCES_LIST_CMAKE SourcesList.cmake)
include(${SOURCES_LIST_CMAKE})

if (NOT All_Sources)
    message(FATAL_ERROR "Even though included SourcesList.cmake, the expected variable seems to be empty.")
endif ()

project(CgEngine_Solution VERSION 0.1.0)
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})

# OpenGL
find_package(OpenGL REQUIRED)

# All_Sources is imported from SourcesList.cmake
message(STATUS "All_Sources: ${All_Sources}")
add_executable(${PROJECT_NAME} ${All_Sources})

set(ROOT_NAMESPACE ${PROJECT_NAME})

################################################################################
# Output directory
################################################################################
set_target_properties(${PROJECT_NAME} PROPERTIES
        OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../bin/CgEngine_Solution/$<CONFIG>/"
        )

################################################################################
# MSVC runtime library
################################################################################
get_property(MSVC_RUNTIME_LIBRARY_DEFAULT TARGET ${PROJECT_NAME} PROPERTY MSVC_RUNTIME_LIBRARY)
string(CONCAT "MSVC_RUNTIME_LIBRARY_STR"
        $<$<CONFIG:Debug>:
        MultiThreadedDebug
        >
        $<$<CONFIG:Release>:
        MultiThreaded
        >
        $<$<NOT:$<OR:$<CONFIG:Debug>,$<CONFIG:Release>>>:${MSVC_RUNTIME_LIBRARY_DEFAULT}>
        )
set_target_properties(${PROJECT_NAME} PROPERTIES MSVC_RUNTIME_LIBRARY ${MSVC_RUNTIME_LIBRARY_STR})

################################################################################
# Compile definitions
################################################################################
if (WIN32)
    target_compile_definitions(${PROJECT_NAME} PRIVATE PLATFORM_WIN32)
endif ()
if (UNIX)
    target_compile_definitions(${PROJECT_NAME} PRIVATE PLATFORM_UNIX)
endif ()

if (DEFINED ENV{TOOLCHAIN_ACTIVE})
    target_compile_definitions(${PROJECT_NAME} PRIVATE
            NDEBUG
            )
else ()
    target_compile_definitions(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Debug>:
            _DEBUG
            >
            $<$<CONFIG:Release>:
            NDEBUG
            >
            )
endif ()

################################################################################
# Dependencies
################################################################################
set(ADDITIONAL_LIBRARY_DEPENDENCIES)
set(ADDITIONAL_INCLUDE_DIRECTORIES)

# GLFW #########################################################################
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(USE_MSVC_RUNTIME_LIBRARY_DLL OFF CACHE BOOL "" FORCE)
set(GLFW_USE_HYBRID_HPG ON CACHE BOOL "" FORCE)
target_compile_definitions(${PROJECT_NAME} PRIVATE GLFW_INCLUDE_NONE)
add_subdirectory(vendor/glfw)
list(APPEND ADDITIONAL_LIBRARY_DEPENDENCIES glfw)
list(APPEND ADDITIONAL_INCLUDE_DIRECTORIES "${PROJECT_SOURCE_DIR}/vendor/glfw/include")
################################################################################

# GLAD #########################################################################
add_subdirectory(vendor/glad)
list(APPEND ADDITIONAL_LIBRARY_DEPENDENCIES glad)
list(APPEND ADDITIONAL_INCLUDE_DIRECTORIES "${PROJECT_SOURCE_DIR}/vendor/glad/include")
################################################################################

# GLM ##########################################################################
list(APPEND ADDITIONAL_INCLUDE_DIRECTORIES "${PROJECT_SOURCE_DIR}/vendor/glm")
################################################################################

# SPDLOG #######################################################################
list(APPEND ADDITIONAL_INCLUDE_DIRECTORIES "${PROJECT_SOURCE_DIR}/vendor/spdlog/include")
################################################################################

# INIH #########################################################################
list(APPEND ADDITIONAL_INCLUDE_DIRECTORIES "${PROJECT_SOURCE_DIR}/vendor/inih")
################################################################################

# PUGIXML ######################################################################
set(PUGIXML_STATIC_CRT ON CACHE BOOL "" FORCE)
add_subdirectory(vendor/pugixml)
list(APPEND ADDITIONAL_LIBRARY_DEPENDENCIES pugixml)
list(APPEND ADDITIONAL_INCLUDE_DIRECTORIES "${PROJECT_SOURCE_DIR}/vendor/pugixml/src")
################################################################################

# STBI #########################################################################
list(APPEND ADDITIONAL_INCLUDE_DIRECTORIES "${PROJECT_SOURCE_DIR}/vendor/stbi/include")
################################################################################

# ASSIMP #######################################################################
set(BUILD_SHARED_LIBS ON CACHE BOOL "" FORCE)
set(ASSIMP_NO_EXPORT ON CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ALL_EXPORTERS_BY_DEFAULT OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(ASSIMP_INSTALL OFF CACHE BOOL "" FORCE)
set(USE_STATIC_CRT ON CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ASSIMP_VIEW  OFF CACHE BOOL "" FORCE)
add_subdirectory(vendor/assimp)
list(APPEND ADDITIONAL_LIBRARY_DEPENDENCIES assimp)
list(APPEND ADDITIONAL_INCLUDE_DIRECTORIES "${PROJECT_SOURCE_DIR}/vendor/assimp/include")
list(APPEND ADDITIONAL_INCLUDE_DIRECTORIES "${PROJECT_BINARY_DIR}/vendor/assimp/include")
################################################################################

# PHYSX ########################################################################
list(APPEND ADDITIONAL_INCLUDE_DIRECTORIES "${PROJECT_SOURCE_DIR}/vendor/PhysX/include")
target_link_libraries(${PROJECT_NAME} PRIVATE debug "${PROJECT_SOURCE_DIR}/vendor/PhysX/lib/checked/PhysX_64.lib")
target_link_libraries(${PROJECT_NAME} PRIVATE debug "${PROJECT_SOURCE_DIR}/vendor/PhysX/lib/checked/PhysXCommon_64.lib")
target_link_libraries(${PROJECT_NAME} PRIVATE debug "${PROJECT_SOURCE_DIR}/vendor/PhysX/lib/checked/PhysXFoundation_64.lib")
target_link_libraries(${PROJECT_NAME} PRIVATE debug "${PROJECT_SOURCE_DIR}/vendor/PhysX/lib/checked/PhysXCooking_64.lib")
target_link_libraries(${PROJECT_NAME} PRIVATE debug "${PROJECT_SOURCE_DIR}/vendor/PhysX/lib/checked/PhysXExtensions_static_64.lib")
target_link_libraries(${PROJECT_NAME} PRIVATE debug "${PROJECT_SOURCE_DIR}/vendor/PhysX/lib/checked/PhysXCharacterKinematic_static_64.lib")
target_link_libraries(${PROJECT_NAME} PRIVATE debug "${PROJECT_SOURCE_DIR}/vendor/PhysX/lib/checked/PhysXPvdSDK_static_64.lib")
target_link_libraries(${PROJECT_NAME} PRIVATE optimized "${PROJECT_SOURCE_DIR}/vendor/PhysX/lib/release/PhysX_64.lib")
target_link_libraries(${PROJECT_NAME} PRIVATE optimized "${PROJECT_SOURCE_DIR}/vendor/PhysX/lib/release/PhysXCommon_64.lib")
target_link_libraries(${PROJECT_NAME} PRIVATE optimized "${PROJECT_SOURCE_DIR}/vendor/PhysX/lib/release/PhysXFoundation_64.lib")
target_link_libraries(${PROJECT_NAME} PRIVATE optimized "${PROJECT_SOURCE_DIR}/vendor/PhysX/lib/release/PhysXCooking_64.lib")
target_link_libraries(${PROJECT_NAME} PRIVATE optimized "${PROJECT_SOURCE_DIR}/vendor/PhysX/lib/release/PhysXExtensions_static_64.lib")
target_link_libraries(${PROJECT_NAME} PRIVATE optimized "${PROJECT_SOURCE_DIR}/vendor/PhysX/lib/release/PhysXCharacterKinematic_static_64.lib")

set(PHYSX_DLLS)
list(APPEND PHYSX_DEBUG_DLLS
        "${PROJECT_SOURCE_DIR}/vendor/PhysX/lib/checked/PhysX_64.dll"
        "${PROJECT_SOURCE_DIR}/vendor/PhysX/lib/checked/PhysXCommon_64.dll"
        "${PROJECT_SOURCE_DIR}/vendor/PhysX/lib/checked/PhysXFoundation_64.dll"
        "${PROJECT_SOURCE_DIR}/vendor/PhysX/lib/checked/PhysXCooking_64.dll"
        )
set(PHYSX_RELEASE_DLLS)
list(APPEND PHYSX_RELEASE_DLLS
        "${PROJECT_SOURCE_DIR}/vendor/PhysX/lib/release/PhysX_64.dll"
        "${PROJECT_SOURCE_DIR}/vendor/PhysX/lib/release/PhysXCommon_64.dll"
        "${PROJECT_SOURCE_DIR}/vendor/PhysX/lib/release/PhysXFoundation_64.dll"
        "${PROJECT_SOURCE_DIR}/vendor/PhysX/lib/release/PhysXCooking_64.dll"
        )
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "$<IF:$<CONFIG:Release>,${PHYSX_RELEASE_DLLS},${PHYSX_DEBUG_DLLS}>" $<TARGET_FILE_DIR:CgEngine_Solution>
        COMMAND_EXPAND_LISTS
        )
################################################################################


target_include_directories(${PROJECT_NAME} PUBLIC
        "${PROJECT_BINARY_DIR}"
        "${PROJECT_SOURCE_DIR}/src/CgEngine"
        "${ADDITIONAL_INCLUDE_DIRECTORIES}"
        )

target_link_libraries(${PROJECT_NAME} PRIVATE "${ADDITIONAL_LIBRARY_DEPENDENCIES}")

################################################################################
# Copy DLLs
################################################################################
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_RUNTIME_DLLS:CgEngine_Solution> $<TARGET_FILE_DIR:CgEngine_Solution>
        COMMAND_EXPAND_LISTS
        )

################################################################################
# Precompiled Header
################################################################################
target_precompile_headers(${PROJECT_NAME} PRIVATE "src/pch.h")

################################################################################
# Assets handling
################################################################################
add_custom_target(copy_assets COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_LIST_DIR}/assets ${PROJECT_BINARY_DIR}/assets)
add_dependencies(${PROJECT_NAME} copy_assets)
