
message("gridShow path is >>>>>>>>> $(GSPATH) <<<<<<<<<")

TEMPLATE     = app
TARGET       = gridShow

DEPENDPATH  += .
INCLUDEPATH += $$DEPENDPATH

OBJECTS_DIR  = .obj
MOC_DIR      = .moc
RCC_DIR      = .rcc
UI_DIR       = .uidir
DESTDIR      = $(GSPATH)/bin

DEFINES += CODING
#DEFINES += UNFINISHED

DEFINES += USE_OIV860_IN_ADES
include (./OIV-lib-include.mk)

CONFIG += warn_on

HEADERS += \ 
    GSMainWindow.h \
    GSPoint.h \
    GSPublicDefine.h \
    GSTree.h \
    GSPublicFunctions.h \
    GSTreeItem.h \
    GSView.h \
    GSSelection.h \
    GS3DGrid.h \
    GS3DObject.h \
    GS3DGridSlice.h \
    GSGridReader.h \
    GSDialogCreateSubGridSlice.h

SOURCES += \
    gridShowMain.cpp \
    GSMainWindow.cpp \
    GSPoint.cpp \
    GSTree.cpp \
    GSPublicFunctions.cpp \
    GSTreeItem.cpp \
    GSView.cpp \
    GSSelection.cpp \
    GS3DGrid.cpp \
    GS3DObject.cpp \
    GS3DGridSlice.cpp \
    GSGridReader.cpp \
    GSDialogCreateSubGridSlice.cpp

FORMS += \
    GSMainWindow.ui \
    GSDialogCreateSubGridSlice.ui
