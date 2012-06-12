
INCLUDEPATH += $(OIVHOME)/include

LIBS += \
    -L$(OIVHOME)/$(OIVARCH)/lib \
    -lDirectViz \
    -lHardCopy \
    -lInventorAlgorithms \
    -lInventorBase \
    -lInventorGL \
    -lInventorGui \
    -lInventorQt4 \
    -lInventor \
    -lLDM \
    -lMeshViz \
    -lScaleViz \
    -lVolumeViz

exists ( $(OIVHOME)/$(OIVARCH)/lib/libInventorImage.so ) {
    LIBS += -lInventorImage # This lib is only contained in OIV8.1 and later.
}
exists ( $(OIVHOME)/$(OIVARCH)/lib/libcomplex.so ) {
    LIBS += -lcomplex # This lib is only contained in OIV8.1 and later.
}
exists ( $(OIVHOME)/$(OIVARCH)/lib/libRemoteViz.so ) {
    LIBS += -lRemoteViz # This lib is only contained in OIV8.5 and later.
}
