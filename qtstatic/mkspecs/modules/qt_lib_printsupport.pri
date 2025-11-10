QT.printsupport.VERSION = 6.10.0
QT.printsupport.name = QtPrintSupport
QT.printsupport.module = Qt6PrintSupport
QT.printsupport.libs = $$QT_MODULE_LIB_BASE
QT.printsupport.ldflags = 
QT.printsupport.includes = $$QT_MODULE_INCLUDE_BASE $$QT_MODULE_INCLUDE_BASE/QtPrintSupport
QT.printsupport.frameworks = 
QT.printsupport.bins = $$QT_MODULE_BIN_BASE
QT.printsupport.plugin_types = printsupport
QT.printsupport.depends =  core gui widgets
QT.printsupport.uses = 
QT.printsupport.module_config = v2 staticlib
QT.printsupport.DEFINES = QT_PRINTSUPPORT_LIB
QT.printsupport.enabled_features = printer printpreviewwidget printdialog printpreviewdialog
QT.printsupport.disabled_features = cups cupsjobwidget
QT_CONFIG += printer printpreviewwidget printdialog printpreviewdialog
QT_MODULES += printsupport

