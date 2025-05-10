cmake_minimum_required(VERSION 3.15)
# Return to Castle Wolfenstein 3D
project(WolfSP)

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Enable C++
enable_language(CXX)

# Helper macro to glob sources and create a library
macro(add_glob_library name type dir)
    file(GLOB_RECURSE ${name}_SRC CONFIGURE_DEPENDS ${dir}/*.c ${dir}/*.cpp)
    add_library(${name} ${type} ${${name}_SRC})
    target_include_directories(${name} PUBLIC ${dir})
endmacro()

# Static libraries
add_glob_library(botlib       STATIC src/botlib)
target_compile_definitions(botlib PRIVATE BOTLIB LIB)

# add_glob_library(extractfuncs STATIC src/extractfuncs)


add_glob_library(freetype2    STATIC src/ft2)
add_glob_library(jpeg         STATIC src/jpeg-6)
add_glob_library(splines      STATIC src/splines)

file(GLOB_RECURSE CGAME_SRC
    src/cgame/*.c
    src/game/bg_animation.c
    src/game/bg_lib.c
    src/game/bg_misc.c
    src/game/bg_pmove.c
    src/game/bg_slidemove.c
    src/game/q_shared.c
    src/ui/ui_shared.c
    src/game/q_math.c

)
if(WIN32)
    list(APPEND CGAME_SRC src/cgame/cgame.def)
endif()

add_library(cgame SHARED ${CGAME_SRC})
target_compile_definitions(cgame PRIVATE CGAMEDLL)

# Shared libraries
file(GLOB_RECURSE UI_SRC
    src/ui/ui_atoms.c
    src/ui/ui_gameinfo.c
    src/ui/ui_main.c
    src/ui/ui_players.c
    src/ui/ui_shared.c
    src/ui/ui_syscalls.c
    src/ui/ui_util.c
    src/game/bg_lib.c
    src/game/bg_misc.c
    src/game/q_shared.c
    src/game/q_math.c

)


if(WIN32)
    list(APPEND UI_SRC src/ui/ui.def)
endif()



add_library(ui SHARED ${UI_SRC})
target_include_directories(ui PUBLIC src/ui)
target_compile_definitions(ui PRIVATE UI_EXPORTS _USRDLL)


file(GLOB_RECURSE GAME_SRC
    src/botai/*.c
    src/game/*.c
)

add_library(game SHARED ${GAME_SRC})


target_compile_definitions(game PRIVATE BUILDING_REF_GL GAMEDLL)

file(GLOB_RECURSE RENDERER_SRC
    src/renderer/*.c
    src/Win32/win_gamma.c
    src/Win32/win_glimp.c
    src/Win32/win_gamma.c
    src/Win32/win_qgl.c    
    src/renderer/ref_trin.def
)


add_library(renderer STATIC ${RENDERER_SRC})
target_compile_definitions(renderer PRIVATE __USEA3D __A3D_GEOM BUILD_FREETYPE)

target_link_libraries(renderer PRIVATE freetype2 jpeg)
target_include_directories(renderer PUBLIC src/glew/include)

# Executable (Win32 app)
file(GLOB_RECURSE WOLFSP_SRC CONFIGURE_DEPENDS
    src/client/*.c
    src/Win32/*.c
    src/qcommon/*.c
    src/server/*.c
    src/game/q_math.c
    src/glew/src/glew.c
    src/win32/wglewInit.cpp
    src/win32/winquake.rc
)

# Exclude vm_x86.c (by full path match)
foreach(f ${WOLFSP_SRC})
    if(f MATCHES ".*[/\\]vm_x86_64\\.c$")
        list(REMOVE_ITEM WOLFSP_SRC ${f})
    endif()
endforeach()

list(APPEND WOLFSP_SRC src/qcommon/vm_x86.c)


set_target_properties(cgame PROPERTIES OUTPUT_NAME "cgamex86")
set_target_properties(ui PROPERTIES OUTPUT_NAME "uix86")
set_target_properties(game PROPERTIES OUTPUT_NAME "qagamex86")


add_executable(WolfSP WIN32 ${WOLFSP_SRC})
target_include_directories(WolfSP PUBLIC src/client src/Win32)
target_compile_definitions(WolfSP PRIVATE GLEW_STATIC __USEA3D __A3D_GEOM BUILD_FREETYPE)
# Link all libraries to the executable
target_link_libraries(WolfSP
    PRIVATE
        botlib
        cgame
        game
        renderer
        ui
        splines
)
target_link_libraries(WolfSP PRIVATE winmm ws2_32)

