TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp \
    watershedwrapper.cpp

include(deployment.pri)
qtcAddDeployment()

INCLUDEPATH += /usr/local/include/opencv \
    /usr/local/include
LIBS += -L/usr/local/lib \
    -lopencv_calib3d \
    -lopencv_core  \
    -lopencv_features2d \
    -lopencv_flann \
    -lopencv_highgui  \
    -lopencv_imgcodecs  \
    -lopencv_imgproc \
    -lopencv_ml \
    -lopencv_objdetect \
    -lopencv_photo \
    -lopencv_shape \
    -lopencv_stitching \
    -lopencv_superres \
    -lopencv_ts \
    -lopencv_video \
    -lopencv_videoio \
    -lopencv_videostab \


HEADERS += \
    watershedwrapper.h

