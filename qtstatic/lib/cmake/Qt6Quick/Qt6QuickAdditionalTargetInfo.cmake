# Additional target information for Qt6Quick
if(NOT DEFINED QT_DEFAULT_IMPORT_CONFIGURATION)
    set(QT_DEFAULT_IMPORT_CONFIGURATION RELEASE)
endif()
__qt_internal_promote_target_to_global_checked(Qt6::Quick)
get_target_property(_qt_imported_location Qt6::Quick IMPORTED_LOCATION_RELEASE)
get_target_property(_qt_imported_location_default Qt6::Quick IMPORTED_LOCATION_${QT_DEFAULT_IMPORT_CONFIGURATION})

# Import target "Qt6::Quick" for configuration "RelWithDebInfo"
set_property(TARGET Qt6::Quick APPEND PROPERTY IMPORTED_CONFIGURATIONS RELWITHDEBINFO)

if(_qt_imported_location)
    set_property(TARGET Qt6::Quick PROPERTY IMPORTED_LOCATION_RELWITHDEBINFO "${_qt_imported_location}")
endif()

# Import target "Qt6::Quick" for configuration "MinSizeRel"
set_property(TARGET Qt6::Quick APPEND PROPERTY IMPORTED_CONFIGURATIONS MINSIZEREL)

if(_qt_imported_location)
    set_property(TARGET Qt6::Quick PROPERTY IMPORTED_LOCATION_MINSIZEREL "${_qt_imported_location}")
endif()

# Default configuration
if(_qt_imported_location_default)
    set_property(TARGET Qt6::Quick PROPERTY IMPORTED_LOCATION "${_qt_imported_location_default}")
endif()
__qt_internal_promote_target_to_global_checked(Qt6::Quick_resources_1)
get_target_property(_qt_imported_objects Qt6::Quick_resources_1 IMPORTED_OBJECTS_RELEASE)
get_target_property(_qt_imported_clr Qt6::Quick_resources_1 IMPORTED_COMMON_LANGUAGE_RUNTIME_RELEASE)
get_target_property(_qt_imported_objects_default Qt6::Quick_resources_1 IMPORTED_OBJECTS_${QT_DEFAULT_IMPORT_CONFIGURATION})
get_target_property(_qt_imported_clr_default Qt6::Quick_resources_1 IMPORTED_COMMON_LANGUAGE_RUNTIME_${QT_DEFAULT_IMPORT_CONFIGURATION})

# Import target "Qt6::Quick_resources_1" for configuration "RelWithDebInfo"
set_property(TARGET Qt6::Quick_resources_1 APPEND PROPERTY IMPORTED_CONFIGURATIONS RELWITHDEBINFO)

if(_qt_imported_objects)
    set_property(TARGET Qt6::Quick_resources_1 PROPERTY IMPORTED_OBJECTS_RELWITHDEBINFO "${_qt_imported_objects}")
endif()
if(_qt_imported_clr)
    set_property(TARGET Qt6::Quick_resources_1 PROPERTY IMPORTED_COMMON_LANGUAGE_RUNTIME_RELWITHDEBINFO "${_qt_imported_clr}")
endif()

# Import target "Qt6::Quick_resources_1" for configuration "MinSizeRel"
set_property(TARGET Qt6::Quick_resources_1 APPEND PROPERTY IMPORTED_CONFIGURATIONS MINSIZEREL)

if(_qt_imported_objects)
    set_property(TARGET Qt6::Quick_resources_1 PROPERTY IMPORTED_OBJECTS_MINSIZEREL "${_qt_imported_objects}")
endif()
if(_qt_imported_clr)
    set_property(TARGET Qt6::Quick_resources_1 PROPERTY IMPORTED_COMMON_LANGUAGE_RUNTIME_MINSIZEREL "${_qt_imported_clr}")
endif()

# Default configuration
if(_qt_imported_objects_default)
    set_property(TARGET Qt6::Quick_resources_1 PROPERTY IMPORTED_OBJECTS "${_qt_imported_objects_default}")
endif()
if(_qt_imported_clr_default)
    set_property(TARGET Qt6::Quick_resources_1 PROPERTY IMPORTED_COMMON_LANGUAGE_RUNTIME "${_qt_imported_clr_default}")
endif()
__qt_internal_promote_target_to_global_checked(Qt6::Quick_resources_2)
get_target_property(_qt_imported_objects Qt6::Quick_resources_2 IMPORTED_OBJECTS_RELEASE)
get_target_property(_qt_imported_clr Qt6::Quick_resources_2 IMPORTED_COMMON_LANGUAGE_RUNTIME_RELEASE)
get_target_property(_qt_imported_objects_default Qt6::Quick_resources_2 IMPORTED_OBJECTS_${QT_DEFAULT_IMPORT_CONFIGURATION})
get_target_property(_qt_imported_clr_default Qt6::Quick_resources_2 IMPORTED_COMMON_LANGUAGE_RUNTIME_${QT_DEFAULT_IMPORT_CONFIGURATION})

# Import target "Qt6::Quick_resources_2" for configuration "RelWithDebInfo"
set_property(TARGET Qt6::Quick_resources_2 APPEND PROPERTY IMPORTED_CONFIGURATIONS RELWITHDEBINFO)

if(_qt_imported_objects)
    set_property(TARGET Qt6::Quick_resources_2 PROPERTY IMPORTED_OBJECTS_RELWITHDEBINFO "${_qt_imported_objects}")
endif()
if(_qt_imported_clr)
    set_property(TARGET Qt6::Quick_resources_2 PROPERTY IMPORTED_COMMON_LANGUAGE_RUNTIME_RELWITHDEBINFO "${_qt_imported_clr}")
endif()

# Import target "Qt6::Quick_resources_2" for configuration "MinSizeRel"
set_property(TARGET Qt6::Quick_resources_2 APPEND PROPERTY IMPORTED_CONFIGURATIONS MINSIZEREL)

if(_qt_imported_objects)
    set_property(TARGET Qt6::Quick_resources_2 PROPERTY IMPORTED_OBJECTS_MINSIZEREL "${_qt_imported_objects}")
endif()
if(_qt_imported_clr)
    set_property(TARGET Qt6::Quick_resources_2 PROPERTY IMPORTED_COMMON_LANGUAGE_RUNTIME_MINSIZEREL "${_qt_imported_clr}")
endif()

# Default configuration
if(_qt_imported_objects_default)
    set_property(TARGET Qt6::Quick_resources_2 PROPERTY IMPORTED_OBJECTS "${_qt_imported_objects_default}")
endif()
if(_qt_imported_clr_default)
    set_property(TARGET Qt6::Quick_resources_2 PROPERTY IMPORTED_COMMON_LANGUAGE_RUNTIME "${_qt_imported_clr_default}")
endif()
__qt_internal_promote_target_to_global_checked(Qt6::Quick_resources_3)
get_target_property(_qt_imported_objects Qt6::Quick_resources_3 IMPORTED_OBJECTS_RELEASE)
get_target_property(_qt_imported_clr Qt6::Quick_resources_3 IMPORTED_COMMON_LANGUAGE_RUNTIME_RELEASE)
get_target_property(_qt_imported_objects_default Qt6::Quick_resources_3 IMPORTED_OBJECTS_${QT_DEFAULT_IMPORT_CONFIGURATION})
get_target_property(_qt_imported_clr_default Qt6::Quick_resources_3 IMPORTED_COMMON_LANGUAGE_RUNTIME_${QT_DEFAULT_IMPORT_CONFIGURATION})

# Import target "Qt6::Quick_resources_3" for configuration "RelWithDebInfo"
set_property(TARGET Qt6::Quick_resources_3 APPEND PROPERTY IMPORTED_CONFIGURATIONS RELWITHDEBINFO)

if(_qt_imported_objects)
    set_property(TARGET Qt6::Quick_resources_3 PROPERTY IMPORTED_OBJECTS_RELWITHDEBINFO "${_qt_imported_objects}")
endif()
if(_qt_imported_clr)
    set_property(TARGET Qt6::Quick_resources_3 PROPERTY IMPORTED_COMMON_LANGUAGE_RUNTIME_RELWITHDEBINFO "${_qt_imported_clr}")
endif()

# Import target "Qt6::Quick_resources_3" for configuration "MinSizeRel"
set_property(TARGET Qt6::Quick_resources_3 APPEND PROPERTY IMPORTED_CONFIGURATIONS MINSIZEREL)

if(_qt_imported_objects)
    set_property(TARGET Qt6::Quick_resources_3 PROPERTY IMPORTED_OBJECTS_MINSIZEREL "${_qt_imported_objects}")
endif()
if(_qt_imported_clr)
    set_property(TARGET Qt6::Quick_resources_3 PROPERTY IMPORTED_COMMON_LANGUAGE_RUNTIME_MINSIZEREL "${_qt_imported_clr}")
endif()

# Default configuration
if(_qt_imported_objects_default)
    set_property(TARGET Qt6::Quick_resources_3 PROPERTY IMPORTED_OBJECTS "${_qt_imported_objects_default}")
endif()
if(_qt_imported_clr_default)
    set_property(TARGET Qt6::Quick_resources_3 PROPERTY IMPORTED_COMMON_LANGUAGE_RUNTIME "${_qt_imported_clr_default}")
endif()
__qt_internal_promote_target_to_global_checked(Qt6::Quick_resources_4)
get_target_property(_qt_imported_objects Qt6::Quick_resources_4 IMPORTED_OBJECTS_RELEASE)
get_target_property(_qt_imported_clr Qt6::Quick_resources_4 IMPORTED_COMMON_LANGUAGE_RUNTIME_RELEASE)
get_target_property(_qt_imported_objects_default Qt6::Quick_resources_4 IMPORTED_OBJECTS_${QT_DEFAULT_IMPORT_CONFIGURATION})
get_target_property(_qt_imported_clr_default Qt6::Quick_resources_4 IMPORTED_COMMON_LANGUAGE_RUNTIME_${QT_DEFAULT_IMPORT_CONFIGURATION})

# Import target "Qt6::Quick_resources_4" for configuration "RelWithDebInfo"
set_property(TARGET Qt6::Quick_resources_4 APPEND PROPERTY IMPORTED_CONFIGURATIONS RELWITHDEBINFO)

if(_qt_imported_objects)
    set_property(TARGET Qt6::Quick_resources_4 PROPERTY IMPORTED_OBJECTS_RELWITHDEBINFO "${_qt_imported_objects}")
endif()
if(_qt_imported_clr)
    set_property(TARGET Qt6::Quick_resources_4 PROPERTY IMPORTED_COMMON_LANGUAGE_RUNTIME_RELWITHDEBINFO "${_qt_imported_clr}")
endif()

# Import target "Qt6::Quick_resources_4" for configuration "MinSizeRel"
set_property(TARGET Qt6::Quick_resources_4 APPEND PROPERTY IMPORTED_CONFIGURATIONS MINSIZEREL)

if(_qt_imported_objects)
    set_property(TARGET Qt6::Quick_resources_4 PROPERTY IMPORTED_OBJECTS_MINSIZEREL "${_qt_imported_objects}")
endif()
if(_qt_imported_clr)
    set_property(TARGET Qt6::Quick_resources_4 PROPERTY IMPORTED_COMMON_LANGUAGE_RUNTIME_MINSIZEREL "${_qt_imported_clr}")
endif()

# Default configuration
if(_qt_imported_objects_default)
    set_property(TARGET Qt6::Quick_resources_4 PROPERTY IMPORTED_OBJECTS "${_qt_imported_objects_default}")
endif()
if(_qt_imported_clr_default)
    set_property(TARGET Qt6::Quick_resources_4 PROPERTY IMPORTED_COMMON_LANGUAGE_RUNTIME "${_qt_imported_clr_default}")
endif()
__qt_internal_promote_target_to_global_checked(Qt6::Quick_resources_5)
get_target_property(_qt_imported_objects Qt6::Quick_resources_5 IMPORTED_OBJECTS_RELEASE)
get_target_property(_qt_imported_clr Qt6::Quick_resources_5 IMPORTED_COMMON_LANGUAGE_RUNTIME_RELEASE)
get_target_property(_qt_imported_objects_default Qt6::Quick_resources_5 IMPORTED_OBJECTS_${QT_DEFAULT_IMPORT_CONFIGURATION})
get_target_property(_qt_imported_clr_default Qt6::Quick_resources_5 IMPORTED_COMMON_LANGUAGE_RUNTIME_${QT_DEFAULT_IMPORT_CONFIGURATION})

# Import target "Qt6::Quick_resources_5" for configuration "RelWithDebInfo"
set_property(TARGET Qt6::Quick_resources_5 APPEND PROPERTY IMPORTED_CONFIGURATIONS RELWITHDEBINFO)

if(_qt_imported_objects)
    set_property(TARGET Qt6::Quick_resources_5 PROPERTY IMPORTED_OBJECTS_RELWITHDEBINFO "${_qt_imported_objects}")
endif()
if(_qt_imported_clr)
    set_property(TARGET Qt6::Quick_resources_5 PROPERTY IMPORTED_COMMON_LANGUAGE_RUNTIME_RELWITHDEBINFO "${_qt_imported_clr}")
endif()

# Import target "Qt6::Quick_resources_5" for configuration "MinSizeRel"
set_property(TARGET Qt6::Quick_resources_5 APPEND PROPERTY IMPORTED_CONFIGURATIONS MINSIZEREL)

if(_qt_imported_objects)
    set_property(TARGET Qt6::Quick_resources_5 PROPERTY IMPORTED_OBJECTS_MINSIZEREL "${_qt_imported_objects}")
endif()
if(_qt_imported_clr)
    set_property(TARGET Qt6::Quick_resources_5 PROPERTY IMPORTED_COMMON_LANGUAGE_RUNTIME_MINSIZEREL "${_qt_imported_clr}")
endif()

# Default configuration
if(_qt_imported_objects_default)
    set_property(TARGET Qt6::Quick_resources_5 PROPERTY IMPORTED_OBJECTS "${_qt_imported_objects_default}")
endif()
if(_qt_imported_clr_default)
    set_property(TARGET Qt6::Quick_resources_5 PROPERTY IMPORTED_COMMON_LANGUAGE_RUNTIME "${_qt_imported_clr_default}")
endif()
__qt_internal_promote_target_to_global_checked(Qt6::Quick_resources_6)
get_target_property(_qt_imported_objects Qt6::Quick_resources_6 IMPORTED_OBJECTS_RELEASE)
get_target_property(_qt_imported_clr Qt6::Quick_resources_6 IMPORTED_COMMON_LANGUAGE_RUNTIME_RELEASE)
get_target_property(_qt_imported_objects_default Qt6::Quick_resources_6 IMPORTED_OBJECTS_${QT_DEFAULT_IMPORT_CONFIGURATION})
get_target_property(_qt_imported_clr_default Qt6::Quick_resources_6 IMPORTED_COMMON_LANGUAGE_RUNTIME_${QT_DEFAULT_IMPORT_CONFIGURATION})

# Import target "Qt6::Quick_resources_6" for configuration "RelWithDebInfo"
set_property(TARGET Qt6::Quick_resources_6 APPEND PROPERTY IMPORTED_CONFIGURATIONS RELWITHDEBINFO)

if(_qt_imported_objects)
    set_property(TARGET Qt6::Quick_resources_6 PROPERTY IMPORTED_OBJECTS_RELWITHDEBINFO "${_qt_imported_objects}")
endif()
if(_qt_imported_clr)
    set_property(TARGET Qt6::Quick_resources_6 PROPERTY IMPORTED_COMMON_LANGUAGE_RUNTIME_RELWITHDEBINFO "${_qt_imported_clr}")
endif()

# Import target "Qt6::Quick_resources_6" for configuration "MinSizeRel"
set_property(TARGET Qt6::Quick_resources_6 APPEND PROPERTY IMPORTED_CONFIGURATIONS MINSIZEREL)

if(_qt_imported_objects)
    set_property(TARGET Qt6::Quick_resources_6 PROPERTY IMPORTED_OBJECTS_MINSIZEREL "${_qt_imported_objects}")
endif()
if(_qt_imported_clr)
    set_property(TARGET Qt6::Quick_resources_6 PROPERTY IMPORTED_COMMON_LANGUAGE_RUNTIME_MINSIZEREL "${_qt_imported_clr}")
endif()

# Default configuration
if(_qt_imported_objects_default)
    set_property(TARGET Qt6::Quick_resources_6 PROPERTY IMPORTED_OBJECTS "${_qt_imported_objects_default}")
endif()
if(_qt_imported_clr_default)
    set_property(TARGET Qt6::Quick_resources_6 PROPERTY IMPORTED_COMMON_LANGUAGE_RUNTIME "${_qt_imported_clr_default}")
endif()
__qt_internal_promote_target_to_global_checked(Qt6::Quick_resources_7)
get_target_property(_qt_imported_objects Qt6::Quick_resources_7 IMPORTED_OBJECTS_RELEASE)
get_target_property(_qt_imported_clr Qt6::Quick_resources_7 IMPORTED_COMMON_LANGUAGE_RUNTIME_RELEASE)
get_target_property(_qt_imported_objects_default Qt6::Quick_resources_7 IMPORTED_OBJECTS_${QT_DEFAULT_IMPORT_CONFIGURATION})
get_target_property(_qt_imported_clr_default Qt6::Quick_resources_7 IMPORTED_COMMON_LANGUAGE_RUNTIME_${QT_DEFAULT_IMPORT_CONFIGURATION})

# Import target "Qt6::Quick_resources_7" for configuration "RelWithDebInfo"
set_property(TARGET Qt6::Quick_resources_7 APPEND PROPERTY IMPORTED_CONFIGURATIONS RELWITHDEBINFO)

if(_qt_imported_objects)
    set_property(TARGET Qt6::Quick_resources_7 PROPERTY IMPORTED_OBJECTS_RELWITHDEBINFO "${_qt_imported_objects}")
endif()
if(_qt_imported_clr)
    set_property(TARGET Qt6::Quick_resources_7 PROPERTY IMPORTED_COMMON_LANGUAGE_RUNTIME_RELWITHDEBINFO "${_qt_imported_clr}")
endif()

# Import target "Qt6::Quick_resources_7" for configuration "MinSizeRel"
set_property(TARGET Qt6::Quick_resources_7 APPEND PROPERTY IMPORTED_CONFIGURATIONS MINSIZEREL)

if(_qt_imported_objects)
    set_property(TARGET Qt6::Quick_resources_7 PROPERTY IMPORTED_OBJECTS_MINSIZEREL "${_qt_imported_objects}")
endif()
if(_qt_imported_clr)
    set_property(TARGET Qt6::Quick_resources_7 PROPERTY IMPORTED_COMMON_LANGUAGE_RUNTIME_MINSIZEREL "${_qt_imported_clr}")
endif()

# Default configuration
if(_qt_imported_objects_default)
    set_property(TARGET Qt6::Quick_resources_7 PROPERTY IMPORTED_OBJECTS "${_qt_imported_objects_default}")
endif()
if(_qt_imported_clr_default)
    set_property(TARGET Qt6::Quick_resources_7 PROPERTY IMPORTED_COMMON_LANGUAGE_RUNTIME "${_qt_imported_clr_default}")
endif()
__qt_internal_promote_target_to_global_checked(Qt6::Quick_resources_8)
get_target_property(_qt_imported_objects Qt6::Quick_resources_8 IMPORTED_OBJECTS_RELEASE)
get_target_property(_qt_imported_clr Qt6::Quick_resources_8 IMPORTED_COMMON_LANGUAGE_RUNTIME_RELEASE)
get_target_property(_qt_imported_objects_default Qt6::Quick_resources_8 IMPORTED_OBJECTS_${QT_DEFAULT_IMPORT_CONFIGURATION})
get_target_property(_qt_imported_clr_default Qt6::Quick_resources_8 IMPORTED_COMMON_LANGUAGE_RUNTIME_${QT_DEFAULT_IMPORT_CONFIGURATION})

# Import target "Qt6::Quick_resources_8" for configuration "RelWithDebInfo"
set_property(TARGET Qt6::Quick_resources_8 APPEND PROPERTY IMPORTED_CONFIGURATIONS RELWITHDEBINFO)

if(_qt_imported_objects)
    set_property(TARGET Qt6::Quick_resources_8 PROPERTY IMPORTED_OBJECTS_RELWITHDEBINFO "${_qt_imported_objects}")
endif()
if(_qt_imported_clr)
    set_property(TARGET Qt6::Quick_resources_8 PROPERTY IMPORTED_COMMON_LANGUAGE_RUNTIME_RELWITHDEBINFO "${_qt_imported_clr}")
endif()

# Import target "Qt6::Quick_resources_8" for configuration "MinSizeRel"
set_property(TARGET Qt6::Quick_resources_8 APPEND PROPERTY IMPORTED_CONFIGURATIONS MINSIZEREL)

if(_qt_imported_objects)
    set_property(TARGET Qt6::Quick_resources_8 PROPERTY IMPORTED_OBJECTS_MINSIZEREL "${_qt_imported_objects}")
endif()
if(_qt_imported_clr)
    set_property(TARGET Qt6::Quick_resources_8 PROPERTY IMPORTED_COMMON_LANGUAGE_RUNTIME_MINSIZEREL "${_qt_imported_clr}")
endif()

# Default configuration
if(_qt_imported_objects_default)
    set_property(TARGET Qt6::Quick_resources_8 PROPERTY IMPORTED_OBJECTS "${_qt_imported_objects_default}")
endif()
if(_qt_imported_clr_default)
    set_property(TARGET Qt6::Quick_resources_8 PROPERTY IMPORTED_COMMON_LANGUAGE_RUNTIME "${_qt_imported_clr_default}")
endif()
__qt_internal_promote_target_to_global_checked(Qt6::Quick_resources_9)
get_target_property(_qt_imported_objects Qt6::Quick_resources_9 IMPORTED_OBJECTS_RELEASE)
get_target_property(_qt_imported_clr Qt6::Quick_resources_9 IMPORTED_COMMON_LANGUAGE_RUNTIME_RELEASE)
get_target_property(_qt_imported_objects_default Qt6::Quick_resources_9 IMPORTED_OBJECTS_${QT_DEFAULT_IMPORT_CONFIGURATION})
get_target_property(_qt_imported_clr_default Qt6::Quick_resources_9 IMPORTED_COMMON_LANGUAGE_RUNTIME_${QT_DEFAULT_IMPORT_CONFIGURATION})

# Import target "Qt6::Quick_resources_9" for configuration "RelWithDebInfo"
set_property(TARGET Qt6::Quick_resources_9 APPEND PROPERTY IMPORTED_CONFIGURATIONS RELWITHDEBINFO)

if(_qt_imported_objects)
    set_property(TARGET Qt6::Quick_resources_9 PROPERTY IMPORTED_OBJECTS_RELWITHDEBINFO "${_qt_imported_objects}")
endif()
if(_qt_imported_clr)
    set_property(TARGET Qt6::Quick_resources_9 PROPERTY IMPORTED_COMMON_LANGUAGE_RUNTIME_RELWITHDEBINFO "${_qt_imported_clr}")
endif()

# Import target "Qt6::Quick_resources_9" for configuration "MinSizeRel"
set_property(TARGET Qt6::Quick_resources_9 APPEND PROPERTY IMPORTED_CONFIGURATIONS MINSIZEREL)

if(_qt_imported_objects)
    set_property(TARGET Qt6::Quick_resources_9 PROPERTY IMPORTED_OBJECTS_MINSIZEREL "${_qt_imported_objects}")
endif()
if(_qt_imported_clr)
    set_property(TARGET Qt6::Quick_resources_9 PROPERTY IMPORTED_COMMON_LANGUAGE_RUNTIME_MINSIZEREL "${_qt_imported_clr}")
endif()

# Default configuration
if(_qt_imported_objects_default)
    set_property(TARGET Qt6::Quick_resources_9 PROPERTY IMPORTED_OBJECTS "${_qt_imported_objects_default}")
endif()
if(_qt_imported_clr_default)
    set_property(TARGET Qt6::Quick_resources_9 PROPERTY IMPORTED_COMMON_LANGUAGE_RUNTIME "${_qt_imported_clr_default}")
endif()
__qt_internal_promote_target_to_global_checked(Qt6::Quick_resources_10)
get_target_property(_qt_imported_objects Qt6::Quick_resources_10 IMPORTED_OBJECTS_RELEASE)
get_target_property(_qt_imported_clr Qt6::Quick_resources_10 IMPORTED_COMMON_LANGUAGE_RUNTIME_RELEASE)
get_target_property(_qt_imported_objects_default Qt6::Quick_resources_10 IMPORTED_OBJECTS_${QT_DEFAULT_IMPORT_CONFIGURATION})
get_target_property(_qt_imported_clr_default Qt6::Quick_resources_10 IMPORTED_COMMON_LANGUAGE_RUNTIME_${QT_DEFAULT_IMPORT_CONFIGURATION})

# Import target "Qt6::Quick_resources_10" for configuration "RelWithDebInfo"
set_property(TARGET Qt6::Quick_resources_10 APPEND PROPERTY IMPORTED_CONFIGURATIONS RELWITHDEBINFO)

if(_qt_imported_objects)
    set_property(TARGET Qt6::Quick_resources_10 PROPERTY IMPORTED_OBJECTS_RELWITHDEBINFO "${_qt_imported_objects}")
endif()
if(_qt_imported_clr)
    set_property(TARGET Qt6::Quick_resources_10 PROPERTY IMPORTED_COMMON_LANGUAGE_RUNTIME_RELWITHDEBINFO "${_qt_imported_clr}")
endif()

# Import target "Qt6::Quick_resources_10" for configuration "MinSizeRel"
set_property(TARGET Qt6::Quick_resources_10 APPEND PROPERTY IMPORTED_CONFIGURATIONS MINSIZEREL)

if(_qt_imported_objects)
    set_property(TARGET Qt6::Quick_resources_10 PROPERTY IMPORTED_OBJECTS_MINSIZEREL "${_qt_imported_objects}")
endif()
if(_qt_imported_clr)
    set_property(TARGET Qt6::Quick_resources_10 PROPERTY IMPORTED_COMMON_LANGUAGE_RUNTIME_MINSIZEREL "${_qt_imported_clr}")
endif()

# Default configuration
if(_qt_imported_objects_default)
    set_property(TARGET Qt6::Quick_resources_10 PROPERTY IMPORTED_OBJECTS "${_qt_imported_objects_default}")
endif()
if(_qt_imported_clr_default)
    set_property(TARGET Qt6::Quick_resources_10 PROPERTY IMPORTED_COMMON_LANGUAGE_RUNTIME "${_qt_imported_clr_default}")
endif()
__qt_internal_promote_target_to_global_checked(Qt6::Quick_resources_11)
get_target_property(_qt_imported_objects Qt6::Quick_resources_11 IMPORTED_OBJECTS_RELEASE)
get_target_property(_qt_imported_clr Qt6::Quick_resources_11 IMPORTED_COMMON_LANGUAGE_RUNTIME_RELEASE)
get_target_property(_qt_imported_objects_default Qt6::Quick_resources_11 IMPORTED_OBJECTS_${QT_DEFAULT_IMPORT_CONFIGURATION})
get_target_property(_qt_imported_clr_default Qt6::Quick_resources_11 IMPORTED_COMMON_LANGUAGE_RUNTIME_${QT_DEFAULT_IMPORT_CONFIGURATION})

# Import target "Qt6::Quick_resources_11" for configuration "RelWithDebInfo"
set_property(TARGET Qt6::Quick_resources_11 APPEND PROPERTY IMPORTED_CONFIGURATIONS RELWITHDEBINFO)

if(_qt_imported_objects)
    set_property(TARGET Qt6::Quick_resources_11 PROPERTY IMPORTED_OBJECTS_RELWITHDEBINFO "${_qt_imported_objects}")
endif()
if(_qt_imported_clr)
    set_property(TARGET Qt6::Quick_resources_11 PROPERTY IMPORTED_COMMON_LANGUAGE_RUNTIME_RELWITHDEBINFO "${_qt_imported_clr}")
endif()

# Import target "Qt6::Quick_resources_11" for configuration "MinSizeRel"
set_property(TARGET Qt6::Quick_resources_11 APPEND PROPERTY IMPORTED_CONFIGURATIONS MINSIZEREL)

if(_qt_imported_objects)
    set_property(TARGET Qt6::Quick_resources_11 PROPERTY IMPORTED_OBJECTS_MINSIZEREL "${_qt_imported_objects}")
endif()
if(_qt_imported_clr)
    set_property(TARGET Qt6::Quick_resources_11 PROPERTY IMPORTED_COMMON_LANGUAGE_RUNTIME_MINSIZEREL "${_qt_imported_clr}")
endif()

# Default configuration
if(_qt_imported_objects_default)
    set_property(TARGET Qt6::Quick_resources_11 PROPERTY IMPORTED_OBJECTS "${_qt_imported_objects_default}")
endif()
if(_qt_imported_clr_default)
    set_property(TARGET Qt6::Quick_resources_11 PROPERTY IMPORTED_COMMON_LANGUAGE_RUNTIME "${_qt_imported_clr_default}")
endif()
__qt_internal_promote_target_to_global_checked(Qt6::Quick_resources_12)
get_target_property(_qt_imported_objects Qt6::Quick_resources_12 IMPORTED_OBJECTS_RELEASE)
get_target_property(_qt_imported_clr Qt6::Quick_resources_12 IMPORTED_COMMON_LANGUAGE_RUNTIME_RELEASE)
get_target_property(_qt_imported_objects_default Qt6::Quick_resources_12 IMPORTED_OBJECTS_${QT_DEFAULT_IMPORT_CONFIGURATION})
get_target_property(_qt_imported_clr_default Qt6::Quick_resources_12 IMPORTED_COMMON_LANGUAGE_RUNTIME_${QT_DEFAULT_IMPORT_CONFIGURATION})

# Import target "Qt6::Quick_resources_12" for configuration "RelWithDebInfo"
set_property(TARGET Qt6::Quick_resources_12 APPEND PROPERTY IMPORTED_CONFIGURATIONS RELWITHDEBINFO)

if(_qt_imported_objects)
    set_property(TARGET Qt6::Quick_resources_12 PROPERTY IMPORTED_OBJECTS_RELWITHDEBINFO "${_qt_imported_objects}")
endif()
if(_qt_imported_clr)
    set_property(TARGET Qt6::Quick_resources_12 PROPERTY IMPORTED_COMMON_LANGUAGE_RUNTIME_RELWITHDEBINFO "${_qt_imported_clr}")
endif()

# Import target "Qt6::Quick_resources_12" for configuration "MinSizeRel"
set_property(TARGET Qt6::Quick_resources_12 APPEND PROPERTY IMPORTED_CONFIGURATIONS MINSIZEREL)

if(_qt_imported_objects)
    set_property(TARGET Qt6::Quick_resources_12 PROPERTY IMPORTED_OBJECTS_MINSIZEREL "${_qt_imported_objects}")
endif()
if(_qt_imported_clr)
    set_property(TARGET Qt6::Quick_resources_12 PROPERTY IMPORTED_COMMON_LANGUAGE_RUNTIME_MINSIZEREL "${_qt_imported_clr}")
endif()

# Default configuration
if(_qt_imported_objects_default)
    set_property(TARGET Qt6::Quick_resources_12 PROPERTY IMPORTED_OBJECTS "${_qt_imported_objects_default}")
endif()
if(_qt_imported_clr_default)
    set_property(TARGET Qt6::Quick_resources_12 PROPERTY IMPORTED_COMMON_LANGUAGE_RUNTIME "${_qt_imported_clr_default}")
endif()

unset(_qt_imported_location)
unset(_qt_imported_location_default)
unset(_qt_imported_soname)
unset(_qt_imported_soname_default)
unset(_qt_imported_link_dependencies)
unset(_qt_imported_link_dependencies_default)
unset(_qt_imported_objects)
unset(_qt_imported_objects_default)
unset(_qt_imported_clr)
unset(_qt_imported_clr_default)
unset(_qt_imported_configs)