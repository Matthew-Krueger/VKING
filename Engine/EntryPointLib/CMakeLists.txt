# Engine/CMakeLists.txt

# Console entry point (all platforms)
add_library(VKING_EntryPoint_Console OBJECT ConsoleEntryPoint.cpp)

# GUI entry point (Windows: separate library, POSIX: alias to Console)
if(WIN32)
    add_library(VKING_EntryPoint_GUI OBJECT Win32GUIEntryPoint.cpp)
    set_target_properties(VKING_EntryPoint_GUI PROPERTIES
            LINK_FLAGS "/SUBSYSTEM:WINDOWS /ENTRY:wWinMainCRTStartup"
    )

    add_library(VKING::EntryPoint::GUI ALIAS VKING_EntryPoint_GUI)
else()
    # On POSIX, GUI is just an alias to Console (no subsystem distinction)
    add_library(VKING_EntryPoint_GUI ALIAS VKING_EntryPoint_Console)
    add_library(VKING::EntryPoint::GUI ALIAS VKING_EntryPoint_Console)
endif()

add_library(VKING::EntryPoint::Console ALIAS VKING_EntryPoint_Console)
