MOC_DIR = .moc
OBJECTS_DIR = .obj
QMAKE_LFLAGS += `pkg-config opencv --cflags --libs`
QMAKE_LIBS += `pkg-config opencv --cflags --libs`

TEMPLATE   = app
  CONFIG  += qt warn_on release thread
 SOURCES   = hough-lines.cpp
  TARGET   = hough-lines
  CONFIG  -= app_bundle

INCLUDEPATH += ../mm/
INCLUDEPATH += /usr/include/opencv2

LIBS += -lopencv_core \
-lopencv_highgui \
-lopencv_imgproc \
-lopencv_features2d \
-lopencv_calib3d
