# Copyright (C) 2022 The Qt Company Ltd.
# SPDX-License-Identifier: BSD-3-Clause

# FindGStreamer
# ---------
#
# Locate the gstreamer-1.0 library and some of its plugins.
# Defines the following imported target:
#
#   ``GStreamer::GStreamer``
#       If the gstreamer-1.0 library is available and target GStreamer::Base,
#       GStreamer::Audio, GStreamer::Video, GStreamer::Pbutils and
#       GStreamer::Allocators exist
#
#  If target GStreamer::GStreamer exists, the following targets may be defined:
#
#   ``GStreamer::App``
#       If the gstapp-1.0 library is available and its dependencies exist
#   ``GStreamer::Photography``
#       If the gstphotography-1.0 library is available and its dependencies exist
#   ``GStreamer::Gl``
#       If the gstgl-1.0 library is available and its dependencies exist
#   ``GStreamer::GlEgl``
#       If the gstreamer-gl-egl-1.0 library is available and its dependencies exist
#   ``GStreamer::GlWayland``
#       If the gstreamer-gl-wayland-1.0 library is available and its dependencies exist
#   ``GStreamer::GlX11``
#       If the gstreamer-gl-x11-1.0 library is available and its dependencies exist
#

include(CMakeFindDependencyMacro)
find_dependency(GObject)

find_package(PkgConfig QUIET)

function(find_gstreamer_component component)
    cmake_parse_arguments(PARSE_ARGV 1 ARGS "" "PC_NAME;HEADER;LIBRARY" "DEPENDENCIES")

    set(pkgconfig_name ${ARGS_PC_NAME})
    set(header ${ARGS_HEADER})
    set(library ${ARGS_LIBRARY})

    set(target GStreamer::${component})

    if(NOT TARGET ${target})
        if(PKG_CONFIG_FOUND)
            string(TOUPPER ${component} upper)
            pkg_check_modules(PC_GSTREAMER_${upper} IMPORTED_TARGET ${pkgconfig_name}>=${GStreamer_FIND_VERSION} )
            if(TARGET PkgConfig::PC_GSTREAMER_${upper})
                add_library(GStreamer::${component} INTERFACE IMPORTED)
                target_link_libraries(GStreamer::${component} INTERFACE PkgConfig::PC_GSTREAMER_${upper})
            endif()
        else()
            foreach(dependency IN LISTS ARGS_DEPENDENCIES)
                if (NOT TARGET ${dependency})
                    set(GStreamer_${component}_FOUND FALSE PARENT_SCOPE)
                    return()
                endif()
            endforeach()

            find_path(GStreamer_${component}_INCLUDE_DIR
                NAMES ${header}
                PATH_SUFFIXES gstreamer-1.0
            )
            find_library(GStreamer_${component}_LIBRARY
                NAMES ${library}
            )

            if(${component} STREQUAL "Gl")
                # search the gstglconfig.h include dir under the same root where the library is found
                get_filename_component(gstglLibDir "${GStreamer_Gl_LIBRARY}" PATH)
                find_path(GStreamer_GlConfig_INCLUDE_DIR
                    NAMES gst/gl/gstglconfig.h
                    PATH_SUFFIXES gstreamer-1.0/include
                    HINTS ${PC_GSTREAMER_GL_INCLUDE_DIRS} ${PC_GSTREAMER_GL_INCLUDEDIR} "${gstglLibDir}"
                )
                if(GStreamer_GlConfig_INCLUDE_DIR)
                    list(APPEND GStreamer_Gl_INCLUDE_DIR "${GStreamer_GlConfig_INCLUDE_DIR}")
                    list(REMOVE_DUPLICATES GStreamer_Gl_INCLUDE_DIR)
                endif()
            endif()
            if(GStreamer_${component}_LIBRARY AND GStreamer_${component}_INCLUDE_DIR)

                if (NOT TARGET DUMMY_GStreamer::${component})

                    string(REGEX MATCH "^([0-9]+)\\.([0-9]+)" GStreamerVersionMatch ${GStreamer_FIND_VERSION})
                    set(VERSION_MAJOR ${CMAKE_MATCH_1})
                    set(VERSION_MINOR ${CMAKE_MATCH_2})

                    # hack alert: we cannot obtain the installed gstreamer version without a try_compile. however we
                    # cannot try_compile without creating a target, as try_compile does not accept include paths. So we
                    # need to create a dummy target

                    add_library(DUMMY_GStreamer::${component} INTERFACE IMPORTED)
                    target_include_directories(DUMMY_GStreamer::${component} INTERFACE ${GStreamer_${component}_INCLUDE_DIR})
                    target_link_libraries(DUMMY_GStreamer::${component} INTERFACE ${GStreamer_${component}_LIBRARY})
                    if(ARGS_DEPENDENCIES)
                        target_link_libraries(DUMMY_GStreamer::${component} INTERFACE ${ARGS_DEPENDENCIES})
                    endif()

                    qt_config_compile_test(gstreamer_version_check_${component}
                        LABEL "GStreamer Version test"
                        LIBRARIES
                            DUMMY_GStreamer::${component}
                        CODE
                    "#include <gst/gstversion.h>

                    static_assert(GST_CHECK_VERSION(${VERSION_MAJOR}, ${VERSION_MINOR}, 0),
                                  \"Minimum required GStreamer version is ${VERSION_MAJOR}.${VERSION_MINOR}\");

                    int main()
                    {
                        return 0;
                    }")

                    # Run the test immediately because we can't rely on lazy evaluation here
                    qt_run_config_compile_test("gstreamer_version_check_${component}")

                    if (${TEST_gstreamer_version_check_${component}})
                        add_library(GStreamer::${component} INTERFACE IMPORTED)

                        target_include_directories(GStreamer::${component} INTERFACE ${GStreamer_${component}_INCLUDE_DIR})
                        target_link_libraries(GStreamer::${component} INTERFACE ${GStreamer_${component}_LIBRARY})
                        if(ARGS_DEPENDENCIES)
                            target_link_libraries(GStreamer::${component} INTERFACE ${ARGS_DEPENDENCIES})
                        endif()
                    endif()
                endif()
            endif()
            mark_as_advanced(GStreamer_${component}_INCLUDE_DIR GStreamer_${component}_LIBRARY)
        endif()
    endif()

    if(TARGET ${target})
        set(GStreamer_${component}_FOUND TRUE PARENT_SCOPE)
    endif()
endfunction()

# GStreamer required dependencies
find_gstreamer_component(Core
    PC_NAME gstreamer-1.0
    HEADER gst/gst.h
    LIBRARY gstreamer-1.0
    DEPENDENCIES GObject::GObject)
find_gstreamer_component(Base
    PC_NAME gstreamer-base-1.0
    HEADER gst/gst.h
    LIBRARY gstbase-1.0
    DEPENDENCIES GStreamer::Core)
find_gstreamer_component(Audio
    PC_NAME gstreamer-audio-1.0
    HEADER gst/audio/audio.h
    LIBRARY gstaudio-1.0
    DEPENDENCIES GStreamer::Base)
find_gstreamer_component(Video
    PC_NAME gstreamer-video-1.0
    HEADER gst/video/video.h
    LIBRARY gstvideo-1.0
    DEPENDENCIES GStreamer::Base)
find_gstreamer_component(Pbutils
    PC_NAME gstreamer-pbutils-1.0
    HEADER gst/pbutils/pbutils.h
    LIBRARY gstpbutils-1.0
    DEPENDENCIES GStreamer::Audio GStreamer::Video)
find_gstreamer_component(Allocators
    PC_NAME gstreamer-allocators-1.0
    HEADER gst/allocators/allocators.h
    LIBRARY gstallocators-1.0
    DEPENDENCIES GStreamer::Core)

if(Play IN_LIST GStreamer_FIND_COMPONENTS)
   find_gstreamer_component(Play
       PC_NAME gstreamer-play-1.0
       HEADER gst/gst.h
       LIBRARY gstplay-1.0
       DEPENDENCIES GStreamer::Core GStreamer::Video)
endif()

if(App IN_LIST GStreamer_FIND_COMPONENTS)
    find_gstreamer_component(App
        PC_NAME gstreamer-app-1.0
        HEADER gst/app/gstappsink.h
        LIBRARY gstapp-1.0
        DEPENDENCIES GStreamer::Base)
endif()

if(Photography IN_LIST GStreamer_FIND_COMPONENTS)
    find_gstreamer_component(Photography
        PC_NAME gstreamer-photography-1.0
        HEADER gst/interfaces/photography.h
        LIBRARY gstphotography-1.0
        DEPENDENCIES GStreamer::Core)
endif()

if(Gl IN_LIST GStreamer_FIND_COMPONENTS)
    find_gstreamer_component(Gl
        PC_NAME gstreamer-gl-1.0
        HEADER gst/gl/gl.h
        LIBRARY gstgl-1.0
        DEPENDENCIES GStreamer::Core)
endif()

if(GlEgl IN_LIST GStreamer_FIND_COMPONENTS)
    find_gstreamer_component(GlEgl
        PC_NAME gstreamer-gl-egl-1.0
        HEADER gst/gl/egl/gstgldisplay_egl.h
        LIBRARY gstgl-1.0
        DEPENDENCIES GStreamer::Video GStreamer::Base GStreamer::Core GStreamer::Gl EGL::EGL )
endif()

if(GlX11 IN_LIST GStreamer_FIND_COMPONENTS)
    find_gstreamer_component(GlX11
        PC_NAME gstreamer-gl-x11-1.0
        HEADER gst/gl/x11/gstgldisplay_x11.h
        LIBRARY gstgl-1.0
        DEPENDENCIES GStreamer::Video GStreamer::Base GStreamer::Core GStreamer::Gl XCB::XCB )
endif()

if(GlWayland IN_LIST GStreamer_FIND_COMPONENTS)
    find_gstreamer_component(GlWayland
        PC_NAME gstreamer-gl-wayland-1.0
        HEADER gst/gl/wayland/gstgldisplay_wayland.h
        LIBRARY gstgl-1.0
        DEPENDENCIES GStreamer::Video GStreamer::Base GStreamer::Core GStreamer::Gl Wayland::Client )
endif()

# Create target GStreamer::GStreamer
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(GStreamer
                                REQUIRED_VARS
                                GStreamer_Core_FOUND
                                GStreamer_Base_FOUND
                                GStreamer_Audio_FOUND
                                GStreamer_Video_FOUND
                                GStreamer_Pbutils_FOUND
                                GStreamer_Allocators_FOUND
                                HANDLE_COMPONENTS
)

if(GStreamer_FOUND AND NOT TARGET GStreamer::GStreamer)
    add_library(GStreamer::GStreamer INTERFACE IMPORTED)
    target_link_libraries(GStreamer::GStreamer INTERFACE
                            GStreamer::Core
                            GStreamer::Base
                            GStreamer::Audio
                            GStreamer::Video
                            GStreamer::Pbutils
                            GStreamer::Allocators
    )
endif()

if(TARGET PkgConfig::PC_GSTREAMER_GL)
    get_target_property(_qt_incs PkgConfig::PC_GSTREAMER_GL INTERFACE_INCLUDE_DIRECTORIES)
    set(__qt_fixed_incs)
    foreach(path IN LISTS _qt_incs)
        if(IS_DIRECTORY "${path}")
            list(APPEND __qt_fixed_incs "${path}")
        endif()
    endforeach()
    set_property(TARGET PkgConfig::PC_GSTREAMER_GL PROPERTY INTERFACE_INCLUDE_DIRECTORIES "${__qt_fixed_incs}")
endif()
