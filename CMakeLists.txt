cmake_minimum_required(VERSION 3.16)

project(BB_Launcher VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

set(PROJECT_SOURCES
    main.cpp
    bblauncher.cpp
    bblauncher.h
    bblauncher.ui
	settings/toml.hpp
    settings/LauncherSettings.h
    settings/LauncherSettings.cpp
	settings/LauncherSettings.ui
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(BB_Launcher
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        settings/LauncherSettings.h settings/LauncherSettings.cpp settings/LauncherSettings.ui
    )
else()
    if(ANDROID)
        add_library(BB_Launcher SHARED
            ${PROJECT_SOURCES}
        )
    else()
        add_executable(BB_Launcher
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(BB_Launcher PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.BB_Launcher)
endif()
set_target_properties(BB_Launcher PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS BB_Launcher
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if (WIN32)
    target_sources(BB_Launcher PRIVATE bblauncher.rc)
endif()

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(BB_Launcher)
endif()
