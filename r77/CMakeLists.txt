project(r77 C CXX)

################################################################################
# Source groups
################################################################################
set(Fichiers_d27en-tte
    "FileBothDirInformationEx.h"
    "FileDirectoryInformationEx.h"
    "FileFullDirInformationEx.h"
    "FileIdBothDirInformationEx.h"
    "FileIdFullDirInformationEx.h"
    "FileInformationClassEx.h"
    "FileNamesInformationEx.h"
    "MinHook/buffer.h"
    "MinHook/HDE/hde32.c"
    "MinHook/HDE/hde32.h"
    "MinHook/HDE/hde64.c"
    "MinHook/HDE/hde64.h"
    "MinHook/HDE/pstdint.h"
    "MinHook/HDE/table32.h"
    "MinHook/HDE/table64.h"
    "MinHook/MinHook.h"
    "MinHook/trampoline.h"
    "r77.h"
    "Rootkit.h"
    "SystemProcessInformationEx.h"
)
source_group("Fichiers d%27en-tête" FILES ${Fichiers_d27en-tte})

set(Fichiers_sources
    "MinHook/buffer.c"
    "MinHook/hook.c"
    "MinHook/trampoline.c"
    "Rootkit.cpp"
)
source_group("Fichiers sources" FILES ${Fichiers_sources})

set(ALL_FILES
    ${Fichiers_d27en-tte}
    ${Fichiers_sources}
)

################################################################################
# Target
################################################################################
add_library(${PROJECT_NAME} STATIC ${ALL_FILES})

use_props(${PROJECT_NAME} "${CMAKE_CONFIGURATION_TYPES}" "${DEFAULT_CXX_PROPS}")
set(ROOT_NAMESPACE r77)

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
            $<$<CONFIG:Release>:
                /OPT:REF;
                /OPT:ICF
            >
            /SUBSYSTEM:CONSOLE
        )
    endif()
endif()

