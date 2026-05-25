cmake_minimum_required(VERSION 3.20)
project(chanh LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ---------------------------------------------------------------------------
# Sources (Đã xóa no_crt.cpp)
# ---------------------------------------------------------------------------
set(SOURCES
    src/main.cpp
    src/ChanhData.cpp
    src/ChanhEngine.cpp
    src/InputInjector.cpp
    src/KeyboardHookManager.cpp
    src/MacroManager.cpp
    src/SettingsUI.cpp
    src/ExcludeManager.cpp
    resource.rc
)

add_executable(chanh WIN32 ${SOURCES})

target_include_directories(chanh PRIVATE src)

# ---------------------------------------------------------------------------
# Cấu hình MSVC Runtime Library
# Dùng Static CRT (/MT) để file .exe tự do chạy trên máy khác mà không cần cài VC Redist
# ---------------------------------------------------------------------------
set_property(TARGET chanh PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

# ---------------------------------------------------------------------------
# Compiler + linker flags (MSVC)
# Đã dọn dẹp các cờ cấm thư viện chuẩn, cho phép dùng C++ STL thoải mái
# ---------------------------------------------------------------------------
if(MSVC)
    target_compile_options(chanh PRIVATE
        /W3
        $<$<CONFIG:Release>:/O2>
        /DUNICODE /D_UNICODE
        /DWIN32 /D_WINDOWS
        /utf-8
    )

    target_link_options(chanh PRIVATE
        /SUBSYSTEM:WINDOWS
        $<$<CONFIG:Release>:/OPT:REF>
        $<$<CONFIG:Release>:/OPT:ICF> 
        /machine:x64
    )

    # Win32 API Standard Libraries
    target_link_libraries(chanh PRIVATE
        kernel32.lib
        user32.lib
        gdi32.lib
        shell32.lib
        advapi32.lib
        comctl32.lib
    )
endif()