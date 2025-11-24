# Additional target information for Qt6QuickEffects
if(NOT DEFINED QT_DEFAULT_IMPORT_CONFIGURATION)
    set(QT_DEFAULT_IMPORT_CONFIGURATION RELEASE)
endif()
__qt_internal_promote_target_to_global_checked(Qt6::QuickEffects)
get_target_property(_qt_imported_location Qt6::QuickEffects IMPORTED_LOCATION_RELEASE)
get_target_property(_qt_imported_location_default Qt6::QuickEffects IMPORTED_LOCATION_${QT_DEFAULT_IMPORT_CONFIGURATION})

# Import target "Qt6::QuickEffects" for configuration "RelWithDebInfo"
set_property(TARGET Qt6::QuickEffects APPEND PROPERTY IMPORTED_CONFIGURATIONS RELWITHDEBINFO)

if(_qt_imported_location)
    set_property(TARGET Qt6::QuickEffects PROPERTY IMPORTED_LOCATION_RELWITHDEBINFO "${_qt_imported_location}")
endif()

# Import target "Qt6::QuickEffects" for configuration "MinSizeRel"
set_property(TARGET Qt6::QuickEffects APPEND PROPERTY IMPORTED_CONFIGURATIONS MINSIZEREL)

if(_qt_imported_location)
    set_property(TARGET Qt6::QuickEffects PROPERTY IMPORTED_LOCATION_MINSIZEREL "${_qt_imported_location}")
endif()

# Default configuration
if(_qt_imported_location_default)
    set_property(TARGET Qt6::QuickEffects PROPERTY IMPORTED_LOCATION "${_qt_imported_location_default}")
endif()
__qt_internal_promote_target_to_global_checked(Qt6::QuickEffects_resources_1)
get_target_property(_qt_imported_objects Qt6::QuickEffects_resources_1 IMPORTED_OBJECTS_RELEASE)
get_target_property(_qt_imported_clr Qt6::QuickEffects_resources_1 IMPORTED_COMMON_LANGUAGE_RUNTIME_RELEASE)
get_target_property(_qt_imported_objects_default Qt6::QuickEffects_resources_1 IMPORTED_OBJECTS_${QT_DEFAULT_IMPORT_CONFIGURATION})
get_target_property(_qt_imported_clr_default Qt6::QuickEffects_resources_1 IMPORTED_COMMON_LANGUAGE_RUNTIME_${QT_DEFAULT_IMPORT_CONFIGURATION})

# Import target "Qt6::QuickEffects_resources_1" for configuration "RelWithDebInfo"
set_property(TARGET Qt6::QuickEffects_resources_1 APPEND PROPERTY IMPORTED_CONFIGURATIONS RELWITHDEBINFO)

if(_qt_imported_objects)
    set_property(TARGET Qt6::QuickEffects_resources_1 PROPERTY IMPORTED_OBJECTS_RELWITHDEBINFO "${_qt_imported_objects}")
endif()
if(_qt_imported_clr)
    set_property(TARGET Qt6::QuickEffects_resources_1 PROPERTY IMPORTED_COMMON_LANGUAGE_RUNTIME_RELWITHDEBINFO "${_qt_imported_clr}")
endif()

# Import target "Qt6::QuickEffects_resources_1" for configuration "MinSizeRel"
set_property(TARGET Qt6::QuickEffects_resources_1 APPEND PROPERTY IMPORTED_CONFIGURATIONS MINSIZEREL)

if(_qt_imported_objects)
    set_property(TARGET Qt6::QuickEffects_resources_1 PROPERTY IMPORTED_OBJECTS_MINSIZEREL "${_qt_imported_objects}")
endif()
if(_qt_imported_clr)
    set_property(TARGET Qt6::QuickEffects_resources_1 PROPERTY IMPORTED_COMMON_LANGUAGE_RUNTIME_MINSIZEREL "${_qt_imported_clr}")
endif()

# Default configuration
if(_qt_imported_objects_default)
    set_property(TARGET Qt6::QuickEffects_resources_1 PROPERTY IMPORTED_OBJECTS "${_qt_imported_objects_default}")
endif()
if(_qt_imported_clr_default)
    set_property(TARGET Qt6::QuickEffects_resources_1 PROPERTY IMPORTED_COMMON_LANGUAGE_RUNTIME "${_qt_imported_clr_default}")
endif()
__qt_internal_promote_target_to_global_checked(Qt6::QuickEffects_resources_2)
get_target_property(_qt_imported_objects Qt6::QuickEffects_resources_2 IMPORTED_OBJECTS_RELEASE)
get_target_property(_qt_imported_clr Qt6::QuickEffects_resources_2 IMPORTED_COMMON_LANGUAGE_RUNTIME_RELEASE)
get_target_property(_qt_imported_objects_default Qt6::QuickEffects_resources_2 IMPORTED_OBJECTS_${QT_DEFAULT_IMPORT_CONFIGURATION})
get_target_property(_qt_imported_clr_default Qt6::QuickEffects_resources_2 IMPORTED_COMMON_LANGUAGE_RUNTIME_${QT_DEFAULT_IMPORT_CONFIGURATION})

# Import target "Qt6::QuickEffects_resources_2" for configuration "RelWithDebInfo"
set_property(TARGET Qt6::QuickEffects_resources_2 APPEND PROPERTY IMPORTED_CONFIGURATIONS RELWITHDEBINFO)

if(_qt_imported_objects)
    set_property(TARGET Qt6::QuickEffects_resources_2 PROPERTY IMPORTED_OBJECTS_RELWITHDEBINFO "${_qt_imported_objects}")
endif()
if(_qt_imported_clr)
    set_property(TARGET Qt6::QuickEffects_resources_2 PROPERTY IMPORTED_COMMON_LANGUAGE_RUNTIME_RELWITHDEBINFO "${_qt_imported_clr}")
endif()

# Import target "Qt6::QuickEffects_resources_2" for configuration "MinSizeRel"
set_property(TARGET Qt6::QuickEffects_resources_2 APPEND PROPERTY IMPORTED_CONFIGURATIONS MINSIZEREL)

if(_qt_imported_objects)
    set_property(TARGET Qt6::QuickEffects_resources_2 PROPERTY IMPORTED_OBJECTS_MINSIZEREL "${_qt_imported_objects}")
endif()
if(_qt_imported_clr)
    set_property(TARGET Qt6::QuickEffects_resources_2 PROPERTY IMPORTED_COMMON_LANGUAGE_RUNTIME_MINSIZEREL "${_qt_imported_clr}")
endif()

# Default configuration
if(_qt_imported_objects_default)
    set_property(TARGET Qt6::QuickEffects_resources_2 PROPERTY IMPORTED_OBJECTS "${_qt_imported_objects_default}")
endif()
if(_qt_imported_clr_default)
    set_property(TARGET Qt6::QuickEffects_resources_2 PROPERTY IMPORTED_COMMON_LANGUAGE_RUNTIME "${_qt_imported_clr_default}")
endif()
__qt_internal_promote_target_to_global_checked(Qt6::QuickEffects_resources_3)
get_target_property(_qt_imported_objects Qt6::QuickEffects_resources_3 IMPORTED_OBJECTS_RELEASE)
get_target_property(_qt_imported_clr Qt6::QuickEffects_resources_3 IMPORTED_COMMON_LANGUAGE_RUNTIME_RELEASE)
get_target_property(_qt_imported_objects_default Qt6::QuickEffects_resources_3 IMPORTED_OBJECTS_${QT_DEFAULT_IMPORT_CONFIGURATION})
get_target_property(_qt_imported_clr_default Qt6::QuickEffects_resources_3 IMPORTED_COMMON_LANGUAGE_RUNTIME_${QT_DEFAULT_IMPORT_CONFIGURATION})

# Import target "Qt6::QuickEffects_resources_3" for configuration "RelWithDebInfo"
set_property(TARGET Qt6::QuickEffects_resources_3 APPEND PROPERTY IMPORTED_CONFIGURATIONS RELWITHDEBINFO)

if(_qt_imported_objects)
    set_property(TARGET Qt6::QuickEffects_resources_3 PROPERTY IMPORTED_OBJECTS_RELWITHDEBINFO "${_qt_imported_objects}")
endif()
if(_qt_imported_clr)
    set_property(TARGET Qt6::QuickEffects_resources_3 PROPERTY IMPORTED_COMMON_LANGUAGE_RUNTIME_RELWITHDEBINFO "${_qt_imported_clr}")
endif()

# Import target "Qt6::QuickEffects_resources_3" for configuration "MinSizeRel"
set_property(TARGET Qt6::QuickEffects_resources_3 APPEND PROPERTY IMPORTED_CONFIGURATIONS MINSIZEREL)

if(_qt_imported_objects)
    set_property(TARGET Qt6::QuickEffects_resources_3 PROPERTY IMPORTED_OBJECTS_MINSIZEREL "${_qt_imported_objects}")
endif()
if(_qt_imported_clr)
    set_property(TARGET Qt6::QuickEffects_resources_3 PROPERTY IMPORTED_COMMON_LANGUAGE_RUNTIME_MINSIZEREL "${_qt_imported_clr}")
endif()

# Default configuration
if(_qt_imported_objects_default)
    set_property(TARGET Qt6::QuickEffects_resources_3 PROPERTY IMPORTED_OBJECTS "${_qt_imported_objects_default}")
endif()
if(_qt_imported_clr_default)
    set_property(TARGET Qt6::QuickEffects_resources_3 PROPERTY IMPORTED_COMMON_LANGUAGE_RUNTIME "${_qt_imported_clr_default}")
endif()
__qt_internal_promote_target_to_global_checked(Qt6::QuickEffects_resources_4)
get_target_property(_qt_imported_objects Qt6::QuickEffects_resources_4 IMPORTED_OBJECTS_RELEASE)
get_target_property(_qt_imported_clr Qt6::QuickEffects_resources_4 IMPORTED_COMMON_LANGUAGE_RUNTIME_RELEASE)
get_target_property(_qt_imported_objects_default Qt6::QuickEffects_resources_4 IMPORTED_OBJECTS_${QT_DEFAULT_IMPORT_CONFIGURATION})
get_target_property(_qt_imported_clr_default Qt6::QuickEffects_resources_4 IMPORTED_COMMON_LANGUAGE_RUNTIME_${QT_DEFAULT_IMPORT_CONFIGURATION})

# Import target "Qt6::QuickEffects_resources_4" for configuration "RelWithDebInfo"
set_property(TARGET Qt6::QuickEffects_resources_4 APPEND PROPERTY IMPORTED_CONFIGURATIONS RELWITHDEBINFO)

if(_qt_imported_objects)
    set_property(TARGET Qt6::QuickEffects_resources_4 PROPERTY IMPORTED_OBJECTS_RELWITHDEBINFO "${_qt_imported_objects}")
endif()
if(_qt_imported_clr)
    set_property(TARGET Qt6::QuickEffects_resources_4 PROPERTY IMPORTED_COMMON_LANGUAGE_RUNTIME_RELWITHDEBINFO "${_qt_imported_clr}")
endif()

# Import target "Qt6::QuickEffects_resources_4" for configuration "MinSizeRel"
set_property(TARGET Qt6::QuickEffects_resources_4 APPEND PROPERTY IMPORTED_CONFIGURATIONS MINSIZEREL)

if(_qt_imported_objects)
    set_property(TARGET Qt6::QuickEffects_resources_4 PROPERTY IMPORTED_OBJECTS_MINSIZEREL "${_qt_imported_objects}")
endif()
if(_qt_imported_clr)
    set_property(TARGET Qt6::QuickEffects_resources_4 PROPERTY IMPORTED_COMMON_LANGUAGE_RUNTIME_MINSIZEREL "${_qt_imported_clr}")
endif()

# Default configuration
if(_qt_imported_objects_default)
    set_property(TARGET Qt6::QuickEffects_resources_4 PROPERTY IMPORTED_OBJECTS "${_qt_imported_objects_default}")
endif()
if(_qt_imported_clr_default)
    set_property(TARGET Qt6::QuickEffects_resources_4 PROPERTY IMPORTED_COMMON_LANGUAGE_RUNTIME "${_qt_imported_clr_default}")
endif()
__qt_internal_promote_target_to_global_checked(Qt6::QuickEffects_resources_5)
get_target_property(_qt_imported_objects Qt6::QuickEffects_resources_5 IMPORTED_OBJECTS_RELEASE)
get_target_property(_qt_imported_clr Qt6::QuickEffects_resources_5 IMPORTED_COMMON_LANGUAGE_RUNTIME_RELEASE)
get_target_property(_qt_imported_objects_default Qt6::QuickEffects_resources_5 IMPORTED_OBJECTS_${QT_DEFAULT_IMPORT_CONFIGURATION})
get_target_property(_qt_imported_clr_default Qt6::QuickEffects_resources_5 IMPORTED_COMMON_LANGUAGE_RUNTIME_${QT_DEFAULT_IMPORT_CONFIGURATION})

# Import target "Qt6::QuickEffects_resources_5" for configuration "RelWithDebInfo"
set_property(TARGET Qt6::QuickEffects_resources_5 APPEND PROPERTY IMPORTED_CONFIGURATIONS RELWITHDEBINFO)

if(_qt_imported_objects)
    set_property(TARGET Qt6::QuickEffects_resources_5 PROPERTY IMPORTED_OBJECTS_RELWITHDEBINFO "${_qt_imported_objects}")
endif()
if(_qt_imported_clr)
    set_property(TARGET Qt6::QuickEffects_resources_5 PROPERTY IMPORTED_COMMON_LANGUAGE_RUNTIME_RELWITHDEBINFO "${_qt_imported_clr}")
endif()

# Import target "Qt6::QuickEffects_resources_5" for configuration "MinSizeRel"
set_property(TARGET Qt6::QuickEffects_resources_5 APPEND PROPERTY IMPORTED_CONFIGURATIONS MINSIZEREL)

if(_qt_imported_objects)
    set_property(TARGET Qt6::QuickEffects_resources_5 PROPERTY IMPORTED_OBJECTS_MINSIZEREL "${_qt_imported_objects}")
endif()
if(_qt_imported_clr)
    set_property(TARGET Qt6::QuickEffects_resources_5 PROPERTY IMPORTED_COMMON_LANGUAGE_RUNTIME_MINSIZEREL "${_qt_imported_clr}")
endif()

# Default configuration
if(_qt_imported_objects_default)
    set_property(TARGET Qt6::QuickEffects_resources_5 PROPERTY IMPORTED_OBJECTS "${_qt_imported_objects_default}")
endif()
if(_qt_imported_clr_default)
    set_property(TARGET Qt6::QuickEffects_resources_5 PROPERTY IMPORTED_COMMON_LANGUAGE_RUNTIME "${_qt_imported_clr_default}")
endif()
__qt_internal_promote_target_to_global_checked(Qt6::QuickEffects_resources_6)
get_target_property(_qt_imported_objects Qt6::QuickEffects_resources_6 IMPORTED_OBJECTS_RELEASE)
get_target_property(_qt_imported_clr Qt6::QuickEffects_resources_6 IMPORTED_COMMON_LANGUAGE_RUNTIME_RELEASE)
get_target_property(_qt_imported_objects_default Qt6::QuickEffects_resources_6 IMPORTED_OBJECTS_${QT_DEFAULT_IMPORT_CONFIGURATION})
get_target_property(_qt_imported_clr_default Qt6::QuickEffects_resources_6 IMPORTED_COMMON_LANGUAGE_RUNTIME_${QT_DEFAULT_IMPORT_CONFIGURATION})

# Import target "Qt6::QuickEffects_resources_6" for configuration "RelWithDebInfo"
set_property(TARGET Qt6::QuickEffects_resources_6 APPEND PROPERTY IMPORTED_CONFIGURATIONS RELWITHDEBINFO)

if(_qt_imported_objects)
    set_property(TARGET Qt6::QuickEffects_resources_6 PROPERTY IMPORTED_OBJECTS_RELWITHDEBINFO "${_qt_imported_objects}")
endif()
if(_qt_imported_clr)
    set_property(TARGET Qt6::QuickEffects_resources_6 PROPERTY IMPORTED_COMMON_LANGUAGE_RUNTIME_RELWITHDEBINFO "${_qt_imported_clr}")
endif()

# Import target "Qt6::QuickEffects_resources_6" for configuration "MinSizeRel"
set_property(TARGET Qt6::QuickEffects_resources_6 APPEND PROPERTY IMPORTED_CONFIGURATIONS MINSIZEREL)

if(_qt_imported_objects)
    set_property(TARGET Qt6::QuickEffects_resources_6 PROPERTY IMPORTED_OBJECTS_MINSIZEREL "${_qt_imported_objects}")
endif()
if(_qt_imported_clr)
    set_property(TARGET Qt6::QuickEffects_resources_6 PROPERTY IMPORTED_COMMON_LANGUAGE_RUNTIME_MINSIZEREL "${_qt_imported_clr}")
endif()

# Default configuration
if(_qt_imported_objects_default)
    set_property(TARGET Qt6::QuickEffects_resources_6 PROPERTY IMPORTED_OBJECTS "${_qt_imported_objects_default}")
endif()
if(_qt_imported_clr_default)
    set_property(TARGET Qt6::QuickEffects_resources_6 PROPERTY IMPORTED_COMMON_LANGUAGE_RUNTIME "${_qt_imported_clr_default}")
endif()
__qt_internal_promote_target_to_global_checked(Qt6::QuickEffects_resources_7)
get_target_property(_qt_imported_objects Qt6::QuickEffects_resources_7 IMPORTED_OBJECTS_RELEASE)
get_target_property(_qt_imported_clr Qt6::QuickEffects_resources_7 IMPORTED_COMMON_LANGUAGE_RUNTIME_RELEASE)
get_target_property(_qt_imported_objects_default Qt6::QuickEffects_resources_7 IMPORTED_OBJECTS_${QT_DEFAULT_IMPORT_CONFIGURATION})
get_target_property(_qt_imported_clr_default Qt6::QuickEffects_resources_7 IMPORTED_COMMON_LANGUAGE_RUNTIME_${QT_DEFAULT_IMPORT_CONFIGURATION})

# Import target "Qt6::QuickEffects_resources_7" for configuration "RelWithDebInfo"
set_property(TARGET Qt6::QuickEffects_resources_7 APPEND PROPERTY IMPORTED_CONFIGURATIONS RELWITHDEBINFO)

if(_qt_imported_objects)
    set_property(TARGET Qt6::QuickEffects_resources_7 PROPERTY IMPORTED_OBJECTS_RELWITHDEBINFO "${_qt_imported_objects}")
endif()
if(_qt_imported_clr)
    set_property(TARGET Qt6::QuickEffects_resources_7 PROPERTY IMPORTED_COMMON_LANGUAGE_RUNTIME_RELWITHDEBINFO "${_qt_imported_clr}")
endif()

# Import target "Qt6::QuickEffects_resources_7" for configuration "MinSizeRel"
set_property(TARGET Qt6::QuickEffects_resources_7 APPEND PROPERTY IMPORTED_CONFIGURATIONS MINSIZEREL)

if(_qt_imported_objects)
    set_property(TARGET Qt6::QuickEffects_resources_7 PROPERTY IMPORTED_OBJECTS_MINSIZEREL "${_qt_imported_objects}")
endif()
if(_qt_imported_clr)
    set_property(TARGET Qt6::QuickEffects_resources_7 PROPERTY IMPORTED_COMMON_LANGUAGE_RUNTIME_MINSIZEREL "${_qt_imported_clr}")
endif()

# Default configuration
if(_qt_imported_objects_default)
    set_property(TARGET Qt6::QuickEffects_resources_7 PROPERTY IMPORTED_OBJECTS "${_qt_imported_objects_default}")
endif()
if(_qt_imported_clr_default)
    set_property(TARGET Qt6::QuickEffects_resources_7 PROPERTY IMPORTED_COMMON_LANGUAGE_RUNTIME "${_qt_imported_clr_default}")
endif()
__qt_internal_promote_target_to_global_checked(Qt6::QuickEffects_resources_8)
get_target_property(_qt_imported_objects Qt6::QuickEffects_resources_8 IMPORTED_OBJECTS_RELEASE)
get_target_property(_qt_imported_clr Qt6::QuickEffects_resources_8 IMPORTED_COMMON_LANGUAGE_RUNTIME_RELEASE)
get_target_property(_qt_imported_objects_default Qt6::QuickEffects_resources_8 IMPORTED_OBJECTS_${QT_DEFAULT_IMPORT_CONFIGURATION})
get_target_property(_qt_imported_clr_default Qt6::QuickEffects_resources_8 IMPORTED_COMMON_LANGUAGE_RUNTIME_${QT_DEFAULT_IMPORT_CONFIGURATION})

# Import target "Qt6::QuickEffects_resources_8" for configuration "RelWithDebInfo"
set_property(TARGET Qt6::QuickEffects_resources_8 APPEND PROPERTY IMPORTED_CONFIGURATIONS RELWITHDEBINFO)

if(_qt_imported_objects)
    set_property(TARGET Qt6::QuickEffects_resources_8 PROPERTY IMPORTED_OBJECTS_RELWITHDEBINFO "${_qt_imported_objects}")
endif()
if(_qt_imported_clr)
    set_property(TARGET Qt6::QuickEffects_resources_8 PROPERTY IMPORTED_COMMON_LANGUAGE_RUNTIME_RELWITHDEBINFO "${_qt_imported_clr}")
endif()

# Import target "Qt6::QuickEffects_resources_8" for configuration "MinSizeRel"
set_property(TARGET Qt6::QuickEffects_resources_8 APPEND PROPERTY IMPORTED_CONFIGURATIONS MINSIZEREL)

if(_qt_imported_objects)
    set_property(TARGET Qt6::QuickEffects_resources_8 PROPERTY IMPORTED_OBJECTS_MINSIZEREL "${_qt_imported_objects}")
endif()
if(_qt_imported_clr)
    set_property(TARGET Qt6::QuickEffects_resources_8 PROPERTY IMPORTED_COMMON_LANGUAGE_RUNTIME_MINSIZEREL "${_qt_imported_clr}")
endif()

# Default configuration
if(_qt_imported_objects_default)
    set_property(TARGET Qt6::QuickEffects_resources_8 PROPERTY IMPORTED_OBJECTS "${_qt_imported_objects_default}")
endif()
if(_qt_imported_clr_default)
    set_property(TARGET Qt6::QuickEffects_resources_8 PROPERTY IMPORTED_COMMON_LANGUAGE_RUNTIME "${_qt_imported_clr_default}")
endif()
__qt_internal_promote_target_to_global_checked(Qt6::QuickEffects_resources_9)
get_target_property(_qt_imported_objects Qt6::QuickEffects_resources_9 IMPORTED_OBJECTS_RELEASE)
get_target_property(_qt_imported_clr Qt6::QuickEffects_resources_9 IMPORTED_COMMON_LANGUAGE_RUNTIME_RELEASE)
get_target_property(_qt_imported_objects_default Qt6::QuickEffects_resources_9 IMPORTED_OBJECTS_${QT_DEFAULT_IMPORT_CONFIGURATION})
get_target_property(_qt_imported_clr_default Qt6::QuickEffects_resources_9 IMPORTED_COMMON_LANGUAGE_RUNTIME_${QT_DEFAULT_IMPORT_CONFIGURATION})

# Import target "Qt6::QuickEffects_resources_9" for configuration "RelWithDebInfo"
set_property(TARGET Qt6::QuickEffects_resources_9 APPEND PROPERTY IMPORTED_CONFIGURATIONS RELWITHDEBINFO)

if(_qt_imported_objects)
    set_property(TARGET Qt6::QuickEffects_resources_9 PROPERTY IMPORTED_OBJECTS_RELWITHDEBINFO "${_qt_imported_objects}")
endif()
if(_qt_imported_clr)
    set_property(TARGET Qt6::QuickEffects_resources_9 PROPERTY IMPORTED_COMMON_LANGUAGE_RUNTIME_RELWITHDEBINFO "${_qt_imported_clr}")
endif()

# Import target "Qt6::QuickEffects_resources_9" for configuration "MinSizeRel"
set_property(TARGET Qt6::QuickEffects_resources_9 APPEND PROPERTY IMPORTED_CONFIGURATIONS MINSIZEREL)

if(_qt_imported_objects)
    set_property(TARGET Qt6::QuickEffects_resources_9 PROPERTY IMPORTED_OBJECTS_MINSIZEREL "${_qt_imported_objects}")
endif()
if(_qt_imported_clr)
    set_property(TARGET Qt6::QuickEffects_resources_9 PROPERTY IMPORTED_COMMON_LANGUAGE_RUNTIME_MINSIZEREL "${_qt_imported_clr}")
endif()

# Default configuration
if(_qt_imported_objects_default)
    set_property(TARGET Qt6::QuickEffects_resources_9 PROPERTY IMPORTED_OBJECTS "${_qt_imported_objects_default}")
endif()
if(_qt_imported_clr_default)
    set_property(TARGET Qt6::QuickEffects_resources_9 PROPERTY IMPORTED_COMMON_LANGUAGE_RUNTIME "${_qt_imported_clr_default}")
endif()
__qt_internal_promote_target_to_global_checked(Qt6::QuickEffects_resources_10)
get_target_property(_qt_imported_objects Qt6::QuickEffects_resources_10 IMPORTED_OBJECTS_RELEASE)
get_target_property(_qt_imported_clr Qt6::QuickEffects_resources_10 IMPORTED_COMMON_LANGUAGE_RUNTIME_RELEASE)
get_target_property(_qt_imported_objects_default Qt6::QuickEffects_resources_10 IMPORTED_OBJECTS_${QT_DEFAULT_IMPORT_CONFIGURATION})
get_target_property(_qt_imported_clr_default Qt6::QuickEffects_resources_10 IMPORTED_COMMON_LANGUAGE_RUNTIME_${QT_DEFAULT_IMPORT_CONFIGURATION})

# Import target "Qt6::QuickEffects_resources_10" for configuration "RelWithDebInfo"
set_property(TARGET Qt6::QuickEffects_resources_10 APPEND PROPERTY IMPORTED_CONFIGURATIONS RELWITHDEBINFO)

if(_qt_imported_objects)
    set_property(TARGET Qt6::QuickEffects_resources_10 PROPERTY IMPORTED_OBJECTS_RELWITHDEBINFO "${_qt_imported_objects}")
endif()
if(_qt_imported_clr)
    set_property(TARGET Qt6::QuickEffects_resources_10 PROPERTY IMPORTED_COMMON_LANGUAGE_RUNTIME_RELWITHDEBINFO "${_qt_imported_clr}")
endif()

# Import target "Qt6::QuickEffects_resources_10" for configuration "MinSizeRel"
set_property(TARGET Qt6::QuickEffects_resources_10 APPEND PROPERTY IMPORTED_CONFIGURATIONS MINSIZEREL)

if(_qt_imported_objects)
    set_property(TARGET Qt6::QuickEffects_resources_10 PROPERTY IMPORTED_OBJECTS_MINSIZEREL "${_qt_imported_objects}")
endif()
if(_qt_imported_clr)
    set_property(TARGET Qt6::QuickEffects_resources_10 PROPERTY IMPORTED_COMMON_LANGUAGE_RUNTIME_MINSIZEREL "${_qt_imported_clr}")
endif()

# Default configuration
if(_qt_imported_objects_default)
    set_property(TARGET Qt6::QuickEffects_resources_10 PROPERTY IMPORTED_OBJECTS "${_qt_imported_objects_default}")
endif()
if(_qt_imported_clr_default)
    set_property(TARGET Qt6::QuickEffects_resources_10 PROPERTY IMPORTED_COMMON_LANGUAGE_RUNTIME "${_qt_imported_clr_default}")
endif()
__qt_internal_promote_target_to_global_checked(Qt6::QuickEffects_resources_11)
get_target_property(_qt_imported_objects Qt6::QuickEffects_resources_11 IMPORTED_OBJECTS_RELEASE)
get_target_property(_qt_imported_clr Qt6::QuickEffects_resources_11 IMPORTED_COMMON_LANGUAGE_RUNTIME_RELEASE)
get_target_property(_qt_imported_objects_default Qt6::QuickEffects_resources_11 IMPORTED_OBJECTS_${QT_DEFAULT_IMPORT_CONFIGURATION})
get_target_property(_qt_imported_clr_default Qt6::QuickEffects_resources_11 IMPORTED_COMMON_LANGUAGE_RUNTIME_${QT_DEFAULT_IMPORT_CONFIGURATION})

# Import target "Qt6::QuickEffects_resources_11" for configuration "RelWithDebInfo"
set_property(TARGET Qt6::QuickEffects_resources_11 APPEND PROPERTY IMPORTED_CONFIGURATIONS RELWITHDEBINFO)

if(_qt_imported_objects)
    set_property(TARGET Qt6::QuickEffects_resources_11 PROPERTY IMPORTED_OBJECTS_RELWITHDEBINFO "${_qt_imported_objects}")
endif()
if(_qt_imported_clr)
    set_property(TARGET Qt6::QuickEffects_resources_11 PROPERTY IMPORTED_COMMON_LANGUAGE_RUNTIME_RELWITHDEBINFO "${_qt_imported_clr}")
endif()

# Import target "Qt6::QuickEffects_resources_11" for configuration "MinSizeRel"
set_property(TARGET Qt6::QuickEffects_resources_11 APPEND PROPERTY IMPORTED_CONFIGURATIONS MINSIZEREL)

if(_qt_imported_objects)
    set_property(TARGET Qt6::QuickEffects_resources_11 PROPERTY IMPORTED_OBJECTS_MINSIZEREL "${_qt_imported_objects}")
endif()
if(_qt_imported_clr)
    set_property(TARGET Qt6::QuickEffects_resources_11 PROPERTY IMPORTED_COMMON_LANGUAGE_RUNTIME_MINSIZEREL "${_qt_imported_clr}")
endif()

# Default configuration
if(_qt_imported_objects_default)
    set_property(TARGET Qt6::QuickEffects_resources_11 PROPERTY IMPORTED_OBJECTS "${_qt_imported_objects_default}")
endif()
if(_qt_imported_clr_default)
    set_property(TARGET Qt6::QuickEffects_resources_11 PROPERTY IMPORTED_COMMON_LANGUAGE_RUNTIME "${_qt_imported_clr_default}")
endif()
__qt_internal_promote_target_to_global_checked(Qt6::QuickEffects_resources_12)
get_target_property(_qt_imported_objects Qt6::QuickEffects_resources_12 IMPORTED_OBJECTS_RELEASE)
get_target_property(_qt_imported_clr Qt6::QuickEffects_resources_12 IMPORTED_COMMON_LANGUAGE_RUNTIME_RELEASE)
get_target_property(_qt_imported_objects_default Qt6::QuickEffects_resources_12 IMPORTED_OBJECTS_${QT_DEFAULT_IMPORT_CONFIGURATION})
get_target_property(_qt_imported_clr_default Qt6::QuickEffects_resources_12 IMPORTED_COMMON_LANGUAGE_RUNTIME_${QT_DEFAULT_IMPORT_CONFIGURATION})

# Import target "Qt6::QuickEffects_resources_12" for configuration "RelWithDebInfo"
set_property(TARGET Qt6::QuickEffects_resources_12 APPEND PROPERTY IMPORTED_CONFIGURATIONS RELWITHDEBINFO)

if(_qt_imported_objects)
    set_property(TARGET Qt6::QuickEffects_resources_12 PROPERTY IMPORTED_OBJECTS_RELWITHDEBINFO "${_qt_imported_objects}")
endif()
if(_qt_imported_clr)
    set_property(TARGET Qt6::QuickEffects_resources_12 PROPERTY IMPORTED_COMMON_LANGUAGE_RUNTIME_RELWITHDEBINFO "${_qt_imported_clr}")
endif()

# Import target "Qt6::QuickEffects_resources_12" for configuration "MinSizeRel"
set_property(TARGET Qt6::QuickEffects_resources_12 APPEND PROPERTY IMPORTED_CONFIGURATIONS MINSIZEREL)

if(_qt_imported_objects)
    set_property(TARGET Qt6::QuickEffects_resources_12 PROPERTY IMPORTED_OBJECTS_MINSIZEREL "${_qt_imported_objects}")
endif()
if(_qt_imported_clr)
    set_property(TARGET Qt6::QuickEffects_resources_12 PROPERTY IMPORTED_COMMON_LANGUAGE_RUNTIME_MINSIZEREL "${_qt_imported_clr}")
endif()

# Default configuration
if(_qt_imported_objects_default)
    set_property(TARGET Qt6::QuickEffects_resources_12 PROPERTY IMPORTED_OBJECTS "${_qt_imported_objects_default}")
endif()
if(_qt_imported_clr_default)
    set_property(TARGET Qt6::QuickEffects_resources_12 PROPERTY IMPORTED_COMMON_LANGUAGE_RUNTIME "${_qt_imported_clr_default}")
endif()
__qt_internal_promote_target_to_global_checked(Qt6::QuickEffects_resources_13)
get_target_property(_qt_imported_objects Qt6::QuickEffects_resources_13 IMPORTED_OBJECTS_RELEASE)
get_target_property(_qt_imported_clr Qt6::QuickEffects_resources_13 IMPORTED_COMMON_LANGUAGE_RUNTIME_RELEASE)
get_target_property(_qt_imported_objects_default Qt6::QuickEffects_resources_13 IMPORTED_OBJECTS_${QT_DEFAULT_IMPORT_CONFIGURATION})
get_target_property(_qt_imported_clr_default Qt6::QuickEffects_resources_13 IMPORTED_COMMON_LANGUAGE_RUNTIME_${QT_DEFAULT_IMPORT_CONFIGURATION})

# Import target "Qt6::QuickEffects_resources_13" for configuration "RelWithDebInfo"
set_property(TARGET Qt6::QuickEffects_resources_13 APPEND PROPERTY IMPORTED_CONFIGURATIONS RELWITHDEBINFO)

if(_qt_imported_objects)
    set_property(TARGET Qt6::QuickEffects_resources_13 PROPERTY IMPORTED_OBJECTS_RELWITHDEBINFO "${_qt_imported_objects}")
endif()
if(_qt_imported_clr)
    set_property(TARGET Qt6::QuickEffects_resources_13 PROPERTY IMPORTED_COMMON_LANGUAGE_RUNTIME_RELWITHDEBINFO "${_qt_imported_clr}")
endif()

# Import target "Qt6::QuickEffects_resources_13" for configuration "MinSizeRel"
set_property(TARGET Qt6::QuickEffects_resources_13 APPEND PROPERTY IMPORTED_CONFIGURATIONS MINSIZEREL)

if(_qt_imported_objects)
    set_property(TARGET Qt6::QuickEffects_resources_13 PROPERTY IMPORTED_OBJECTS_MINSIZEREL "${_qt_imported_objects}")
endif()
if(_qt_imported_clr)
    set_property(TARGET Qt6::QuickEffects_resources_13 PROPERTY IMPORTED_COMMON_LANGUAGE_RUNTIME_MINSIZEREL "${_qt_imported_clr}")
endif()

# Default configuration
if(_qt_imported_objects_default)
    set_property(TARGET Qt6::QuickEffects_resources_13 PROPERTY IMPORTED_OBJECTS "${_qt_imported_objects_default}")
endif()
if(_qt_imported_clr_default)
    set_property(TARGET Qt6::QuickEffects_resources_13 PROPERTY IMPORTED_COMMON_LANGUAGE_RUNTIME "${_qt_imported_clr_default}")
endif()
__qt_internal_promote_target_to_global_checked(Qt6::QuickEffects_resources_14)
get_target_property(_qt_imported_objects Qt6::QuickEffects_resources_14 IMPORTED_OBJECTS_RELEASE)
get_target_property(_qt_imported_clr Qt6::QuickEffects_resources_14 IMPORTED_COMMON_LANGUAGE_RUNTIME_RELEASE)
get_target_property(_qt_imported_objects_default Qt6::QuickEffects_resources_14 IMPORTED_OBJECTS_${QT_DEFAULT_IMPORT_CONFIGURATION})
get_target_property(_qt_imported_clr_default Qt6::QuickEffects_resources_14 IMPORTED_COMMON_LANGUAGE_RUNTIME_${QT_DEFAULT_IMPORT_CONFIGURATION})

# Import target "Qt6::QuickEffects_resources_14" for configuration "RelWithDebInfo"
set_property(TARGET Qt6::QuickEffects_resources_14 APPEND PROPERTY IMPORTED_CONFIGURATIONS RELWITHDEBINFO)

if(_qt_imported_objects)
    set_property(TARGET Qt6::QuickEffects_resources_14 PROPERTY IMPORTED_OBJECTS_RELWITHDEBINFO "${_qt_imported_objects}")
endif()
if(_qt_imported_clr)
    set_property(TARGET Qt6::QuickEffects_resources_14 PROPERTY IMPORTED_COMMON_LANGUAGE_RUNTIME_RELWITHDEBINFO "${_qt_imported_clr}")
endif()

# Import target "Qt6::QuickEffects_resources_14" for configuration "MinSizeRel"
set_property(TARGET Qt6::QuickEffects_resources_14 APPEND PROPERTY IMPORTED_CONFIGURATIONS MINSIZEREL)

if(_qt_imported_objects)
    set_property(TARGET Qt6::QuickEffects_resources_14 PROPERTY IMPORTED_OBJECTS_MINSIZEREL "${_qt_imported_objects}")
endif()
if(_qt_imported_clr)
    set_property(TARGET Qt6::QuickEffects_resources_14 PROPERTY IMPORTED_COMMON_LANGUAGE_RUNTIME_MINSIZEREL "${_qt_imported_clr}")
endif()

# Default configuration
if(_qt_imported_objects_default)
    set_property(TARGET Qt6::QuickEffects_resources_14 PROPERTY IMPORTED_OBJECTS "${_qt_imported_objects_default}")
endif()
if(_qt_imported_clr_default)
    set_property(TARGET Qt6::QuickEffects_resources_14 PROPERTY IMPORTED_COMMON_LANGUAGE_RUNTIME "${_qt_imported_clr_default}")
endif()
__qt_internal_promote_target_to_global_checked(Qt6::QuickEffects_resources_15)
get_target_property(_qt_imported_objects Qt6::QuickEffects_resources_15 IMPORTED_OBJECTS_RELEASE)
get_target_property(_qt_imported_clr Qt6::QuickEffects_resources_15 IMPORTED_COMMON_LANGUAGE_RUNTIME_RELEASE)
get_target_property(_qt_imported_objects_default Qt6::QuickEffects_resources_15 IMPORTED_OBJECTS_${QT_DEFAULT_IMPORT_CONFIGURATION})
get_target_property(_qt_imported_clr_default Qt6::QuickEffects_resources_15 IMPORTED_COMMON_LANGUAGE_RUNTIME_${QT_DEFAULT_IMPORT_CONFIGURATION})

# Import target "Qt6::QuickEffects_resources_15" for configuration "RelWithDebInfo"
set_property(TARGET Qt6::QuickEffects_resources_15 APPEND PROPERTY IMPORTED_CONFIGURATIONS RELWITHDEBINFO)

if(_qt_imported_objects)
    set_property(TARGET Qt6::QuickEffects_resources_15 PROPERTY IMPORTED_OBJECTS_RELWITHDEBINFO "${_qt_imported_objects}")
endif()
if(_qt_imported_clr)
    set_property(TARGET Qt6::QuickEffects_resources_15 PROPERTY IMPORTED_COMMON_LANGUAGE_RUNTIME_RELWITHDEBINFO "${_qt_imported_clr}")
endif()

# Import target "Qt6::QuickEffects_resources_15" for configuration "MinSizeRel"
set_property(TARGET Qt6::QuickEffects_resources_15 APPEND PROPERTY IMPORTED_CONFIGURATIONS MINSIZEREL)

if(_qt_imported_objects)
    set_property(TARGET Qt6::QuickEffects_resources_15 PROPERTY IMPORTED_OBJECTS_MINSIZEREL "${_qt_imported_objects}")
endif()
if(_qt_imported_clr)
    set_property(TARGET Qt6::QuickEffects_resources_15 PROPERTY IMPORTED_COMMON_LANGUAGE_RUNTIME_MINSIZEREL "${_qt_imported_clr}")
endif()

# Default configuration
if(_qt_imported_objects_default)
    set_property(TARGET Qt6::QuickEffects_resources_15 PROPERTY IMPORTED_OBJECTS "${_qt_imported_objects_default}")
endif()
if(_qt_imported_clr_default)
    set_property(TARGET Qt6::QuickEffects_resources_15 PROPERTY IMPORTED_COMMON_LANGUAGE_RUNTIME "${_qt_imported_clr_default}")
endif()
__qt_internal_promote_target_to_global_checked(Qt6::QuickEffects_resources_16)
get_target_property(_qt_imported_objects Qt6::QuickEffects_resources_16 IMPORTED_OBJECTS_RELEASE)
get_target_property(_qt_imported_clr Qt6::QuickEffects_resources_16 IMPORTED_COMMON_LANGUAGE_RUNTIME_RELEASE)
get_target_property(_qt_imported_objects_default Qt6::QuickEffects_resources_16 IMPORTED_OBJECTS_${QT_DEFAULT_IMPORT_CONFIGURATION})
get_target_property(_qt_imported_clr_default Qt6::QuickEffects_resources_16 IMPORTED_COMMON_LANGUAGE_RUNTIME_${QT_DEFAULT_IMPORT_CONFIGURATION})

# Import target "Qt6::QuickEffects_resources_16" for configuration "RelWithDebInfo"
set_property(TARGET Qt6::QuickEffects_resources_16 APPEND PROPERTY IMPORTED_CONFIGURATIONS RELWITHDEBINFO)

if(_qt_imported_objects)
    set_property(TARGET Qt6::QuickEffects_resources_16 PROPERTY IMPORTED_OBJECTS_RELWITHDEBINFO "${_qt_imported_objects}")
endif()
if(_qt_imported_clr)
    set_property(TARGET Qt6::QuickEffects_resources_16 PROPERTY IMPORTED_COMMON_LANGUAGE_RUNTIME_RELWITHDEBINFO "${_qt_imported_clr}")
endif()

# Import target "Qt6::QuickEffects_resources_16" for configuration "MinSizeRel"
set_property(TARGET Qt6::QuickEffects_resources_16 APPEND PROPERTY IMPORTED_CONFIGURATIONS MINSIZEREL)

if(_qt_imported_objects)
    set_property(TARGET Qt6::QuickEffects_resources_16 PROPERTY IMPORTED_OBJECTS_MINSIZEREL "${_qt_imported_objects}")
endif()
if(_qt_imported_clr)
    set_property(TARGET Qt6::QuickEffects_resources_16 PROPERTY IMPORTED_COMMON_LANGUAGE_RUNTIME_MINSIZEREL "${_qt_imported_clr}")
endif()

# Default configuration
if(_qt_imported_objects_default)
    set_property(TARGET Qt6::QuickEffects_resources_16 PROPERTY IMPORTED_OBJECTS "${_qt_imported_objects_default}")
endif()
if(_qt_imported_clr_default)
    set_property(TARGET Qt6::QuickEffects_resources_16 PROPERTY IMPORTED_COMMON_LANGUAGE_RUNTIME "${_qt_imported_clr_default}")
endif()
__qt_internal_promote_target_to_global_checked(Qt6::QuickEffects_resources_17)
get_target_property(_qt_imported_objects Qt6::QuickEffects_resources_17 IMPORTED_OBJECTS_RELEASE)
get_target_property(_qt_imported_clr Qt6::QuickEffects_resources_17 IMPORTED_COMMON_LANGUAGE_RUNTIME_RELEASE)
get_target_property(_qt_imported_objects_default Qt6::QuickEffects_resources_17 IMPORTED_OBJECTS_${QT_DEFAULT_IMPORT_CONFIGURATION})
get_target_property(_qt_imported_clr_default Qt6::QuickEffects_resources_17 IMPORTED_COMMON_LANGUAGE_RUNTIME_${QT_DEFAULT_IMPORT_CONFIGURATION})

# Import target "Qt6::QuickEffects_resources_17" for configuration "RelWithDebInfo"
set_property(TARGET Qt6::QuickEffects_resources_17 APPEND PROPERTY IMPORTED_CONFIGURATIONS RELWITHDEBINFO)

if(_qt_imported_objects)
    set_property(TARGET Qt6::QuickEffects_resources_17 PROPERTY IMPORTED_OBJECTS_RELWITHDEBINFO "${_qt_imported_objects}")
endif()
if(_qt_imported_clr)
    set_property(TARGET Qt6::QuickEffects_resources_17 PROPERTY IMPORTED_COMMON_LANGUAGE_RUNTIME_RELWITHDEBINFO "${_qt_imported_clr}")
endif()

# Import target "Qt6::QuickEffects_resources_17" for configuration "MinSizeRel"
set_property(TARGET Qt6::QuickEffects_resources_17 APPEND PROPERTY IMPORTED_CONFIGURATIONS MINSIZEREL)

if(_qt_imported_objects)
    set_property(TARGET Qt6::QuickEffects_resources_17 PROPERTY IMPORTED_OBJECTS_MINSIZEREL "${_qt_imported_objects}")
endif()
if(_qt_imported_clr)
    set_property(TARGET Qt6::QuickEffects_resources_17 PROPERTY IMPORTED_COMMON_LANGUAGE_RUNTIME_MINSIZEREL "${_qt_imported_clr}")
endif()

# Default configuration
if(_qt_imported_objects_default)
    set_property(TARGET Qt6::QuickEffects_resources_17 PROPERTY IMPORTED_OBJECTS "${_qt_imported_objects_default}")
endif()
if(_qt_imported_clr_default)
    set_property(TARGET Qt6::QuickEffects_resources_17 PROPERTY IMPORTED_COMMON_LANGUAGE_RUNTIME "${_qt_imported_clr_default}")
endif()
__qt_internal_promote_target_to_global_checked(Qt6::QuickEffects_resources_18)
get_target_property(_qt_imported_objects Qt6::QuickEffects_resources_18 IMPORTED_OBJECTS_RELEASE)
get_target_property(_qt_imported_clr Qt6::QuickEffects_resources_18 IMPORTED_COMMON_LANGUAGE_RUNTIME_RELEASE)
get_target_property(_qt_imported_objects_default Qt6::QuickEffects_resources_18 IMPORTED_OBJECTS_${QT_DEFAULT_IMPORT_CONFIGURATION})
get_target_property(_qt_imported_clr_default Qt6::QuickEffects_resources_18 IMPORTED_COMMON_LANGUAGE_RUNTIME_${QT_DEFAULT_IMPORT_CONFIGURATION})

# Import target "Qt6::QuickEffects_resources_18" for configuration "RelWithDebInfo"
set_property(TARGET Qt6::QuickEffects_resources_18 APPEND PROPERTY IMPORTED_CONFIGURATIONS RELWITHDEBINFO)

if(_qt_imported_objects)
    set_property(TARGET Qt6::QuickEffects_resources_18 PROPERTY IMPORTED_OBJECTS_RELWITHDEBINFO "${_qt_imported_objects}")
endif()
if(_qt_imported_clr)
    set_property(TARGET Qt6::QuickEffects_resources_18 PROPERTY IMPORTED_COMMON_LANGUAGE_RUNTIME_RELWITHDEBINFO "${_qt_imported_clr}")
endif()

# Import target "Qt6::QuickEffects_resources_18" for configuration "MinSizeRel"
set_property(TARGET Qt6::QuickEffects_resources_18 APPEND PROPERTY IMPORTED_CONFIGURATIONS MINSIZEREL)

if(_qt_imported_objects)
    set_property(TARGET Qt6::QuickEffects_resources_18 PROPERTY IMPORTED_OBJECTS_MINSIZEREL "${_qt_imported_objects}")
endif()
if(_qt_imported_clr)
    set_property(TARGET Qt6::QuickEffects_resources_18 PROPERTY IMPORTED_COMMON_LANGUAGE_RUNTIME_MINSIZEREL "${_qt_imported_clr}")
endif()

# Default configuration
if(_qt_imported_objects_default)
    set_property(TARGET Qt6::QuickEffects_resources_18 PROPERTY IMPORTED_OBJECTS "${_qt_imported_objects_default}")
endif()
if(_qt_imported_clr_default)
    set_property(TARGET Qt6::QuickEffects_resources_18 PROPERTY IMPORTED_COMMON_LANGUAGE_RUNTIME "${_qt_imported_clr_default}")
endif()
__qt_internal_promote_target_to_global_checked(Qt6::QuickEffects_resources_19)
get_target_property(_qt_imported_objects Qt6::QuickEffects_resources_19 IMPORTED_OBJECTS_RELEASE)
get_target_property(_qt_imported_clr Qt6::QuickEffects_resources_19 IMPORTED_COMMON_LANGUAGE_RUNTIME_RELEASE)
get_target_property(_qt_imported_objects_default Qt6::QuickEffects_resources_19 IMPORTED_OBJECTS_${QT_DEFAULT_IMPORT_CONFIGURATION})
get_target_property(_qt_imported_clr_default Qt6::QuickEffects_resources_19 IMPORTED_COMMON_LANGUAGE_RUNTIME_${QT_DEFAULT_IMPORT_CONFIGURATION})

# Import target "Qt6::QuickEffects_resources_19" for configuration "RelWithDebInfo"
set_property(TARGET Qt6::QuickEffects_resources_19 APPEND PROPERTY IMPORTED_CONFIGURATIONS RELWITHDEBINFO)

if(_qt_imported_objects)
    set_property(TARGET Qt6::QuickEffects_resources_19 PROPERTY IMPORTED_OBJECTS_RELWITHDEBINFO "${_qt_imported_objects}")
endif()
if(_qt_imported_clr)
    set_property(TARGET Qt6::QuickEffects_resources_19 PROPERTY IMPORTED_COMMON_LANGUAGE_RUNTIME_RELWITHDEBINFO "${_qt_imported_clr}")
endif()

# Import target "Qt6::QuickEffects_resources_19" for configuration "MinSizeRel"
set_property(TARGET Qt6::QuickEffects_resources_19 APPEND PROPERTY IMPORTED_CONFIGURATIONS MINSIZEREL)

if(_qt_imported_objects)
    set_property(TARGET Qt6::QuickEffects_resources_19 PROPERTY IMPORTED_OBJECTS_MINSIZEREL "${_qt_imported_objects}")
endif()
if(_qt_imported_clr)
    set_property(TARGET Qt6::QuickEffects_resources_19 PROPERTY IMPORTED_COMMON_LANGUAGE_RUNTIME_MINSIZEREL "${_qt_imported_clr}")
endif()

# Default configuration
if(_qt_imported_objects_default)
    set_property(TARGET Qt6::QuickEffects_resources_19 PROPERTY IMPORTED_OBJECTS "${_qt_imported_objects_default}")
endif()
if(_qt_imported_clr_default)
    set_property(TARGET Qt6::QuickEffects_resources_19 PROPERTY IMPORTED_COMMON_LANGUAGE_RUNTIME "${_qt_imported_clr_default}")
endif()
__qt_internal_promote_target_to_global_checked(Qt6::QuickEffects_resources_20)
get_target_property(_qt_imported_objects Qt6::QuickEffects_resources_20 IMPORTED_OBJECTS_RELEASE)
get_target_property(_qt_imported_clr Qt6::QuickEffects_resources_20 IMPORTED_COMMON_LANGUAGE_RUNTIME_RELEASE)
get_target_property(_qt_imported_objects_default Qt6::QuickEffects_resources_20 IMPORTED_OBJECTS_${QT_DEFAULT_IMPORT_CONFIGURATION})
get_target_property(_qt_imported_clr_default Qt6::QuickEffects_resources_20 IMPORTED_COMMON_LANGUAGE_RUNTIME_${QT_DEFAULT_IMPORT_CONFIGURATION})

# Import target "Qt6::QuickEffects_resources_20" for configuration "RelWithDebInfo"
set_property(TARGET Qt6::QuickEffects_resources_20 APPEND PROPERTY IMPORTED_CONFIGURATIONS RELWITHDEBINFO)

if(_qt_imported_objects)
    set_property(TARGET Qt6::QuickEffects_resources_20 PROPERTY IMPORTED_OBJECTS_RELWITHDEBINFO "${_qt_imported_objects}")
endif()
if(_qt_imported_clr)
    set_property(TARGET Qt6::QuickEffects_resources_20 PROPERTY IMPORTED_COMMON_LANGUAGE_RUNTIME_RELWITHDEBINFO "${_qt_imported_clr}")
endif()

# Import target "Qt6::QuickEffects_resources_20" for configuration "MinSizeRel"
set_property(TARGET Qt6::QuickEffects_resources_20 APPEND PROPERTY IMPORTED_CONFIGURATIONS MINSIZEREL)

if(_qt_imported_objects)
    set_property(TARGET Qt6::QuickEffects_resources_20 PROPERTY IMPORTED_OBJECTS_MINSIZEREL "${_qt_imported_objects}")
endif()
if(_qt_imported_clr)
    set_property(TARGET Qt6::QuickEffects_resources_20 PROPERTY IMPORTED_COMMON_LANGUAGE_RUNTIME_MINSIZEREL "${_qt_imported_clr}")
endif()

# Default configuration
if(_qt_imported_objects_default)
    set_property(TARGET Qt6::QuickEffects_resources_20 PROPERTY IMPORTED_OBJECTS "${_qt_imported_objects_default}")
endif()
if(_qt_imported_clr_default)
    set_property(TARGET Qt6::QuickEffects_resources_20 PROPERTY IMPORTED_COMMON_LANGUAGE_RUNTIME "${_qt_imported_clr_default}")
endif()
__qt_internal_promote_target_to_global_checked(Qt6::QuickEffects_resources_21)
get_target_property(_qt_imported_objects Qt6::QuickEffects_resources_21 IMPORTED_OBJECTS_RELEASE)
get_target_property(_qt_imported_clr Qt6::QuickEffects_resources_21 IMPORTED_COMMON_LANGUAGE_RUNTIME_RELEASE)
get_target_property(_qt_imported_objects_default Qt6::QuickEffects_resources_21 IMPORTED_OBJECTS_${QT_DEFAULT_IMPORT_CONFIGURATION})
get_target_property(_qt_imported_clr_default Qt6::QuickEffects_resources_21 IMPORTED_COMMON_LANGUAGE_RUNTIME_${QT_DEFAULT_IMPORT_CONFIGURATION})

# Import target "Qt6::QuickEffects_resources_21" for configuration "RelWithDebInfo"
set_property(TARGET Qt6::QuickEffects_resources_21 APPEND PROPERTY IMPORTED_CONFIGURATIONS RELWITHDEBINFO)

if(_qt_imported_objects)
    set_property(TARGET Qt6::QuickEffects_resources_21 PROPERTY IMPORTED_OBJECTS_RELWITHDEBINFO "${_qt_imported_objects}")
endif()
if(_qt_imported_clr)
    set_property(TARGET Qt6::QuickEffects_resources_21 PROPERTY IMPORTED_COMMON_LANGUAGE_RUNTIME_RELWITHDEBINFO "${_qt_imported_clr}")
endif()

# Import target "Qt6::QuickEffects_resources_21" for configuration "MinSizeRel"
set_property(TARGET Qt6::QuickEffects_resources_21 APPEND PROPERTY IMPORTED_CONFIGURATIONS MINSIZEREL)

if(_qt_imported_objects)
    set_property(TARGET Qt6::QuickEffects_resources_21 PROPERTY IMPORTED_OBJECTS_MINSIZEREL "${_qt_imported_objects}")
endif()
if(_qt_imported_clr)
    set_property(TARGET Qt6::QuickEffects_resources_21 PROPERTY IMPORTED_COMMON_LANGUAGE_RUNTIME_MINSIZEREL "${_qt_imported_clr}")
endif()

# Default configuration
if(_qt_imported_objects_default)
    set_property(TARGET Qt6::QuickEffects_resources_21 PROPERTY IMPORTED_OBJECTS "${_qt_imported_objects_default}")
endif()
if(_qt_imported_clr_default)
    set_property(TARGET Qt6::QuickEffects_resources_21 PROPERTY IMPORTED_COMMON_LANGUAGE_RUNTIME "${_qt_imported_clr_default}")
endif()
__qt_internal_promote_target_to_global_checked(Qt6::QuickEffects_resources_22)
get_target_property(_qt_imported_objects Qt6::QuickEffects_resources_22 IMPORTED_OBJECTS_RELEASE)
get_target_property(_qt_imported_clr Qt6::QuickEffects_resources_22 IMPORTED_COMMON_LANGUAGE_RUNTIME_RELEASE)
get_target_property(_qt_imported_objects_default Qt6::QuickEffects_resources_22 IMPORTED_OBJECTS_${QT_DEFAULT_IMPORT_CONFIGURATION})
get_target_property(_qt_imported_clr_default Qt6::QuickEffects_resources_22 IMPORTED_COMMON_LANGUAGE_RUNTIME_${QT_DEFAULT_IMPORT_CONFIGURATION})

# Import target "Qt6::QuickEffects_resources_22" for configuration "RelWithDebInfo"
set_property(TARGET Qt6::QuickEffects_resources_22 APPEND PROPERTY IMPORTED_CONFIGURATIONS RELWITHDEBINFO)

if(_qt_imported_objects)
    set_property(TARGET Qt6::QuickEffects_resources_22 PROPERTY IMPORTED_OBJECTS_RELWITHDEBINFO "${_qt_imported_objects}")
endif()
if(_qt_imported_clr)
    set_property(TARGET Qt6::QuickEffects_resources_22 PROPERTY IMPORTED_COMMON_LANGUAGE_RUNTIME_RELWITHDEBINFO "${_qt_imported_clr}")
endif()

# Import target "Qt6::QuickEffects_resources_22" for configuration "MinSizeRel"
set_property(TARGET Qt6::QuickEffects_resources_22 APPEND PROPERTY IMPORTED_CONFIGURATIONS MINSIZEREL)

if(_qt_imported_objects)
    set_property(TARGET Qt6::QuickEffects_resources_22 PROPERTY IMPORTED_OBJECTS_MINSIZEREL "${_qt_imported_objects}")
endif()
if(_qt_imported_clr)
    set_property(TARGET Qt6::QuickEffects_resources_22 PROPERTY IMPORTED_COMMON_LANGUAGE_RUNTIME_MINSIZEREL "${_qt_imported_clr}")
endif()

# Default configuration
if(_qt_imported_objects_default)
    set_property(TARGET Qt6::QuickEffects_resources_22 PROPERTY IMPORTED_OBJECTS "${_qt_imported_objects_default}")
endif()
if(_qt_imported_clr_default)
    set_property(TARGET Qt6::QuickEffects_resources_22 PROPERTY IMPORTED_COMMON_LANGUAGE_RUNTIME "${_qt_imported_clr_default}")
endif()
__qt_internal_promote_target_to_global_checked(Qt6::QuickEffects_resources_23)
get_target_property(_qt_imported_objects Qt6::QuickEffects_resources_23 IMPORTED_OBJECTS_RELEASE)
get_target_property(_qt_imported_clr Qt6::QuickEffects_resources_23 IMPORTED_COMMON_LANGUAGE_RUNTIME_RELEASE)
get_target_property(_qt_imported_objects_default Qt6::QuickEffects_resources_23 IMPORTED_OBJECTS_${QT_DEFAULT_IMPORT_CONFIGURATION})
get_target_property(_qt_imported_clr_default Qt6::QuickEffects_resources_23 IMPORTED_COMMON_LANGUAGE_RUNTIME_${QT_DEFAULT_IMPORT_CONFIGURATION})

# Import target "Qt6::QuickEffects_resources_23" for configuration "RelWithDebInfo"
set_property(TARGET Qt6::QuickEffects_resources_23 APPEND PROPERTY IMPORTED_CONFIGURATIONS RELWITHDEBINFO)

if(_qt_imported_objects)
    set_property(TARGET Qt6::QuickEffects_resources_23 PROPERTY IMPORTED_OBJECTS_RELWITHDEBINFO "${_qt_imported_objects}")
endif()
if(_qt_imported_clr)
    set_property(TARGET Qt6::QuickEffects_resources_23 PROPERTY IMPORTED_COMMON_LANGUAGE_RUNTIME_RELWITHDEBINFO "${_qt_imported_clr}")
endif()

# Import target "Qt6::QuickEffects_resources_23" for configuration "MinSizeRel"
set_property(TARGET Qt6::QuickEffects_resources_23 APPEND PROPERTY IMPORTED_CONFIGURATIONS MINSIZEREL)

if(_qt_imported_objects)
    set_property(TARGET Qt6::QuickEffects_resources_23 PROPERTY IMPORTED_OBJECTS_MINSIZEREL "${_qt_imported_objects}")
endif()
if(_qt_imported_clr)
    set_property(TARGET Qt6::QuickEffects_resources_23 PROPERTY IMPORTED_COMMON_LANGUAGE_RUNTIME_MINSIZEREL "${_qt_imported_clr}")
endif()

# Default configuration
if(_qt_imported_objects_default)
    set_property(TARGET Qt6::QuickEffects_resources_23 PROPERTY IMPORTED_OBJECTS "${_qt_imported_objects_default}")
endif()
if(_qt_imported_clr_default)
    set_property(TARGET Qt6::QuickEffects_resources_23 PROPERTY IMPORTED_COMMON_LANGUAGE_RUNTIME "${_qt_imported_clr_default}")
endif()
__qt_internal_promote_target_to_global_checked(Qt6::QuickEffects_resources_24)
get_target_property(_qt_imported_objects Qt6::QuickEffects_resources_24 IMPORTED_OBJECTS_RELEASE)
get_target_property(_qt_imported_clr Qt6::QuickEffects_resources_24 IMPORTED_COMMON_LANGUAGE_RUNTIME_RELEASE)
get_target_property(_qt_imported_objects_default Qt6::QuickEffects_resources_24 IMPORTED_OBJECTS_${QT_DEFAULT_IMPORT_CONFIGURATION})
get_target_property(_qt_imported_clr_default Qt6::QuickEffects_resources_24 IMPORTED_COMMON_LANGUAGE_RUNTIME_${QT_DEFAULT_IMPORT_CONFIGURATION})

# Import target "Qt6::QuickEffects_resources_24" for configuration "RelWithDebInfo"
set_property(TARGET Qt6::QuickEffects_resources_24 APPEND PROPERTY IMPORTED_CONFIGURATIONS RELWITHDEBINFO)

if(_qt_imported_objects)
    set_property(TARGET Qt6::QuickEffects_resources_24 PROPERTY IMPORTED_OBJECTS_RELWITHDEBINFO "${_qt_imported_objects}")
endif()
if(_qt_imported_clr)
    set_property(TARGET Qt6::QuickEffects_resources_24 PROPERTY IMPORTED_COMMON_LANGUAGE_RUNTIME_RELWITHDEBINFO "${_qt_imported_clr}")
endif()

# Import target "Qt6::QuickEffects_resources_24" for configuration "MinSizeRel"
set_property(TARGET Qt6::QuickEffects_resources_24 APPEND PROPERTY IMPORTED_CONFIGURATIONS MINSIZEREL)

if(_qt_imported_objects)
    set_property(TARGET Qt6::QuickEffects_resources_24 PROPERTY IMPORTED_OBJECTS_MINSIZEREL "${_qt_imported_objects}")
endif()
if(_qt_imported_clr)
    set_property(TARGET Qt6::QuickEffects_resources_24 PROPERTY IMPORTED_COMMON_LANGUAGE_RUNTIME_MINSIZEREL "${_qt_imported_clr}")
endif()

# Default configuration
if(_qt_imported_objects_default)
    set_property(TARGET Qt6::QuickEffects_resources_24 PROPERTY IMPORTED_OBJECTS "${_qt_imported_objects_default}")
endif()
if(_qt_imported_clr_default)
    set_property(TARGET Qt6::QuickEffects_resources_24 PROPERTY IMPORTED_COMMON_LANGUAGE_RUNTIME "${_qt_imported_clr_default}")
endif()
__qt_internal_promote_target_to_global_checked(Qt6::QuickEffects_resources_25)
get_target_property(_qt_imported_objects Qt6::QuickEffects_resources_25 IMPORTED_OBJECTS_RELEASE)
get_target_property(_qt_imported_clr Qt6::QuickEffects_resources_25 IMPORTED_COMMON_LANGUAGE_RUNTIME_RELEASE)
get_target_property(_qt_imported_objects_default Qt6::QuickEffects_resources_25 IMPORTED_OBJECTS_${QT_DEFAULT_IMPORT_CONFIGURATION})
get_target_property(_qt_imported_clr_default Qt6::QuickEffects_resources_25 IMPORTED_COMMON_LANGUAGE_RUNTIME_${QT_DEFAULT_IMPORT_CONFIGURATION})

# Import target "Qt6::QuickEffects_resources_25" for configuration "RelWithDebInfo"
set_property(TARGET Qt6::QuickEffects_resources_25 APPEND PROPERTY IMPORTED_CONFIGURATIONS RELWITHDEBINFO)

if(_qt_imported_objects)
    set_property(TARGET Qt6::QuickEffects_resources_25 PROPERTY IMPORTED_OBJECTS_RELWITHDEBINFO "${_qt_imported_objects}")
endif()
if(_qt_imported_clr)
    set_property(TARGET Qt6::QuickEffects_resources_25 PROPERTY IMPORTED_COMMON_LANGUAGE_RUNTIME_RELWITHDEBINFO "${_qt_imported_clr}")
endif()

# Import target "Qt6::QuickEffects_resources_25" for configuration "MinSizeRel"
set_property(TARGET Qt6::QuickEffects_resources_25 APPEND PROPERTY IMPORTED_CONFIGURATIONS MINSIZEREL)

if(_qt_imported_objects)
    set_property(TARGET Qt6::QuickEffects_resources_25 PROPERTY IMPORTED_OBJECTS_MINSIZEREL "${_qt_imported_objects}")
endif()
if(_qt_imported_clr)
    set_property(TARGET Qt6::QuickEffects_resources_25 PROPERTY IMPORTED_COMMON_LANGUAGE_RUNTIME_MINSIZEREL "${_qt_imported_clr}")
endif()

# Default configuration
if(_qt_imported_objects_default)
    set_property(TARGET Qt6::QuickEffects_resources_25 PROPERTY IMPORTED_OBJECTS "${_qt_imported_objects_default}")
endif()
if(_qt_imported_clr_default)
    set_property(TARGET Qt6::QuickEffects_resources_25 PROPERTY IMPORTED_COMMON_LANGUAGE_RUNTIME "${_qt_imported_clr_default}")
endif()
__qt_internal_promote_target_to_global_checked(Qt6::QuickEffects_resources_26)
get_target_property(_qt_imported_objects Qt6::QuickEffects_resources_26 IMPORTED_OBJECTS_RELEASE)
get_target_property(_qt_imported_clr Qt6::QuickEffects_resources_26 IMPORTED_COMMON_LANGUAGE_RUNTIME_RELEASE)
get_target_property(_qt_imported_objects_default Qt6::QuickEffects_resources_26 IMPORTED_OBJECTS_${QT_DEFAULT_IMPORT_CONFIGURATION})
get_target_property(_qt_imported_clr_default Qt6::QuickEffects_resources_26 IMPORTED_COMMON_LANGUAGE_RUNTIME_${QT_DEFAULT_IMPORT_CONFIGURATION})

# Import target "Qt6::QuickEffects_resources_26" for configuration "RelWithDebInfo"
set_property(TARGET Qt6::QuickEffects_resources_26 APPEND PROPERTY IMPORTED_CONFIGURATIONS RELWITHDEBINFO)

if(_qt_imported_objects)
    set_property(TARGET Qt6::QuickEffects_resources_26 PROPERTY IMPORTED_OBJECTS_RELWITHDEBINFO "${_qt_imported_objects}")
endif()
if(_qt_imported_clr)
    set_property(TARGET Qt6::QuickEffects_resources_26 PROPERTY IMPORTED_COMMON_LANGUAGE_RUNTIME_RELWITHDEBINFO "${_qt_imported_clr}")
endif()

# Import target "Qt6::QuickEffects_resources_26" for configuration "MinSizeRel"
set_property(TARGET Qt6::QuickEffects_resources_26 APPEND PROPERTY IMPORTED_CONFIGURATIONS MINSIZEREL)

if(_qt_imported_objects)
    set_property(TARGET Qt6::QuickEffects_resources_26 PROPERTY IMPORTED_OBJECTS_MINSIZEREL "${_qt_imported_objects}")
endif()
if(_qt_imported_clr)
    set_property(TARGET Qt6::QuickEffects_resources_26 PROPERTY IMPORTED_COMMON_LANGUAGE_RUNTIME_MINSIZEREL "${_qt_imported_clr}")
endif()

# Default configuration
if(_qt_imported_objects_default)
    set_property(TARGET Qt6::QuickEffects_resources_26 PROPERTY IMPORTED_OBJECTS "${_qt_imported_objects_default}")
endif()
if(_qt_imported_clr_default)
    set_property(TARGET Qt6::QuickEffects_resources_26 PROPERTY IMPORTED_COMMON_LANGUAGE_RUNTIME "${_qt_imported_clr_default}")
endif()
__qt_internal_promote_target_to_global_checked(Qt6::QuickEffects_resources_27)
get_target_property(_qt_imported_objects Qt6::QuickEffects_resources_27 IMPORTED_OBJECTS_RELEASE)
get_target_property(_qt_imported_clr Qt6::QuickEffects_resources_27 IMPORTED_COMMON_LANGUAGE_RUNTIME_RELEASE)
get_target_property(_qt_imported_objects_default Qt6::QuickEffects_resources_27 IMPORTED_OBJECTS_${QT_DEFAULT_IMPORT_CONFIGURATION})
get_target_property(_qt_imported_clr_default Qt6::QuickEffects_resources_27 IMPORTED_COMMON_LANGUAGE_RUNTIME_${QT_DEFAULT_IMPORT_CONFIGURATION})

# Import target "Qt6::QuickEffects_resources_27" for configuration "RelWithDebInfo"
set_property(TARGET Qt6::QuickEffects_resources_27 APPEND PROPERTY IMPORTED_CONFIGURATIONS RELWITHDEBINFO)

if(_qt_imported_objects)
    set_property(TARGET Qt6::QuickEffects_resources_27 PROPERTY IMPORTED_OBJECTS_RELWITHDEBINFO "${_qt_imported_objects}")
endif()
if(_qt_imported_clr)
    set_property(TARGET Qt6::QuickEffects_resources_27 PROPERTY IMPORTED_COMMON_LANGUAGE_RUNTIME_RELWITHDEBINFO "${_qt_imported_clr}")
endif()

# Import target "Qt6::QuickEffects_resources_27" for configuration "MinSizeRel"
set_property(TARGET Qt6::QuickEffects_resources_27 APPEND PROPERTY IMPORTED_CONFIGURATIONS MINSIZEREL)

if(_qt_imported_objects)
    set_property(TARGET Qt6::QuickEffects_resources_27 PROPERTY IMPORTED_OBJECTS_MINSIZEREL "${_qt_imported_objects}")
endif()
if(_qt_imported_clr)
    set_property(TARGET Qt6::QuickEffects_resources_27 PROPERTY IMPORTED_COMMON_LANGUAGE_RUNTIME_MINSIZEREL "${_qt_imported_clr}")
endif()

# Default configuration
if(_qt_imported_objects_default)
    set_property(TARGET Qt6::QuickEffects_resources_27 PROPERTY IMPORTED_OBJECTS "${_qt_imported_objects_default}")
endif()
if(_qt_imported_clr_default)
    set_property(TARGET Qt6::QuickEffects_resources_27 PROPERTY IMPORTED_COMMON_LANGUAGE_RUNTIME "${_qt_imported_clr_default}")
endif()
__qt_internal_promote_target_to_global_checked(Qt6::QuickEffects_resources_28)
get_target_property(_qt_imported_objects Qt6::QuickEffects_resources_28 IMPORTED_OBJECTS_RELEASE)
get_target_property(_qt_imported_clr Qt6::QuickEffects_resources_28 IMPORTED_COMMON_LANGUAGE_RUNTIME_RELEASE)
get_target_property(_qt_imported_objects_default Qt6::QuickEffects_resources_28 IMPORTED_OBJECTS_${QT_DEFAULT_IMPORT_CONFIGURATION})
get_target_property(_qt_imported_clr_default Qt6::QuickEffects_resources_28 IMPORTED_COMMON_LANGUAGE_RUNTIME_${QT_DEFAULT_IMPORT_CONFIGURATION})

# Import target "Qt6::QuickEffects_resources_28" for configuration "RelWithDebInfo"
set_property(TARGET Qt6::QuickEffects_resources_28 APPEND PROPERTY IMPORTED_CONFIGURATIONS RELWITHDEBINFO)

if(_qt_imported_objects)
    set_property(TARGET Qt6::QuickEffects_resources_28 PROPERTY IMPORTED_OBJECTS_RELWITHDEBINFO "${_qt_imported_objects}")
endif()
if(_qt_imported_clr)
    set_property(TARGET Qt6::QuickEffects_resources_28 PROPERTY IMPORTED_COMMON_LANGUAGE_RUNTIME_RELWITHDEBINFO "${_qt_imported_clr}")
endif()

# Import target "Qt6::QuickEffects_resources_28" for configuration "MinSizeRel"
set_property(TARGET Qt6::QuickEffects_resources_28 APPEND PROPERTY IMPORTED_CONFIGURATIONS MINSIZEREL)

if(_qt_imported_objects)
    set_property(TARGET Qt6::QuickEffects_resources_28 PROPERTY IMPORTED_OBJECTS_MINSIZEREL "${_qt_imported_objects}")
endif()
if(_qt_imported_clr)
    set_property(TARGET Qt6::QuickEffects_resources_28 PROPERTY IMPORTED_COMMON_LANGUAGE_RUNTIME_MINSIZEREL "${_qt_imported_clr}")
endif()

# Default configuration
if(_qt_imported_objects_default)
    set_property(TARGET Qt6::QuickEffects_resources_28 PROPERTY IMPORTED_OBJECTS "${_qt_imported_objects_default}")
endif()
if(_qt_imported_clr_default)
    set_property(TARGET Qt6::QuickEffects_resources_28 PROPERTY IMPORTED_COMMON_LANGUAGE_RUNTIME "${_qt_imported_clr_default}")
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