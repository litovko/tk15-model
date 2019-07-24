QT += quick network printsupport
QT += qml 3dcore 3drender 3dinput 3dlogic 3dextras 3danimation
CONFIG += c++11 console

DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

system(ver.bat)
BUILDVER = $$system("git.exe rev-list HEAD --count")
DEFINES += GIT_REV=$$BUILDVER
message($$BUILDVER)
#build_nr.commands = ver.bat
#build_nr.depends = FORCE
#QMAKE_EXTRA_TARGETS += build_nr
#PRE_TARGETDEPS += build_nr

HEADERS  += build.h


SOURCES += \
    dataset.cpp \
        main.cpp \
    model.cpp \
    third/qcustomplot/qcustomplot.cpp \
    third/qmlplot.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
DESTDIR = D:\dest.rig.5.12.msvc

HEADERS += \
    dataset.h \
    model.h \
    third/qcustomplot/qcustomplot.h \
    third/qmlplot.h
