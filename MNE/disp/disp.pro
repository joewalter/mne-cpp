#--------------------------------------------------------------------------------------------------------------
#
# @file     disp.pro
# @author   Christoph Dinh <chdinh@nmr.mgh.harvard.edu>;
#           Matti Hämäläinen <msh@nmr.mgh.harvard.edu>
# @version  1.0
# @date     July, 2012
#
# @section  LICENSE
#
# Copyright (C) 2012, Christoph Dinh and Matti Hamalainen. All rights reserved.
#
# Redistribution and use in source and binary forms, with or without modification, are permitted provided that
# the following conditions are met:
#     * Redistributions of source code must retain the above copyright notice, this list of conditions and the
#       following disclaimer.
#     * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and
#       the following disclaimer in the documentation and/or other materials provided with the distribution.
#     * Neither the name of the Massachusetts General Hospital nor the names of its contributors may be used
#       to endorse or promote products derived from this software without specific prior written permission.
# 
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
# WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
# PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL MASSACHUSETTS GENERAL HOSPITAL BE LIABLE FOR ANY DIRECT,
# INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
# PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
# HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
# NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.
#
#
# @brief    ToDo Documentation...
#
#--------------------------------------------------------------------------------------------------------------

TEMPLATE = lib

QT       += 3d
QT       += core
QT       -= gui

DEFINES += DISP_LIBRARY

CONFIG(debug, debug|release) {
    TARGET = dispd
    unix:DESTDIR = $$PWD/../../lib/unix/debug
    win32:DESTDIR = $$PWD/../../lib/win32/debug
    unix: LIBS += -L$$PWD/../../lib/unix/debug/ -lfiffd
    win32:LIBS += -L$$PWD/../../lib/win32/debug/ -lfiffd
    unix: LIBS += -L$$PWD/../../lib/unix/debug/ -lmned
    win32:LIBS += -L$$PWD/../../lib/win32/debug/ -lmned
    win32:QMAKE_POST_LINK += xcopy /y "..\\..\\..\\mne-cpp\\lib\\win32\\debug\\dispd.dll" "..\\..\\..\\mne-cpp\\bin\\win32\\debug\\"
}
else {
    TARGET = disp
    unix:DESTDIR = $$PWD/../../lib/unix/release
    win32:DESTDIR = $$PWD/../../lib/win32/release
    unix: LIBS += -L$$PWD/../../lib/unix/release/ -lfiff
    win32:LIBS += -L$$PWD/../../lib/win32/release/ -lfiff
    unix: LIBS += -L$$PWD/../../lib/unix/release/ -lmne
    win32:LIBS += -L$$PWD/../../lib/win32/release/ -lmne
    win32:QMAKE_POST_LINK += xcopy /y "..\\..\\..\\mne-cpp\\lib\\win32\\release\\disp.dll" "..\\..\\..\\mne-cpp\\bin\\win32\\release\\"
}


SOURCES += \
    geometryview.cpp

HEADERS += disp_global.h \
    geometryview.h


symbian {
    MMP_RULES += EXPORTUNFROZEN
    TARGET.UID3 = 0xE05B73F1
    TARGET.CAPABILITY = 
    TARGET.EPOCALLOWDLLDATA = 1
    addFiles.sources = mne.dll
    addFiles.path = !:/sys/bin
    DEPLOYMENT += addFiles
}

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}
