QT.testlib.VERSION = 6.10.0
QT.testlib.name = QtTest
QT.testlib.module = Qt6Test
QT.testlib.libs = $$QT_MODULE_LIB_BASE
QT.testlib.ldflags = 
QT.testlib.includes = $$QT_MODULE_INCLUDE_BASE $$QT_MODULE_INCLUDE_BASE/QtTest
QT.testlib.frameworks = 
QT.testlib.bins = $$QT_MODULE_BIN_BASE
QT.testlib.depends =  core
QT.testlib.uses = 
QT.testlib.module_config = v2 staticlib
QT.testlib.CONFIG = console testlib_defines
QT.testlib.DEFINES = QT_TESTLIB_LIB
QT.testlib.enabled_features = itemmodeltester
QT.testlib.disabled_features = testlib_selfcover valgrind batch_test_support
QT_CONFIG += itemmodeltester
QT_MODULES += testlib

