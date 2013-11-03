HEADERS += \
    server.h \
    threadedtcpserver.h \
    threadedtcpsocket.h \
    gate_src/truegate.h \
    gate_src/orgate.h \
    gate_src/gate.h \
    gate_src/falsegate.h \
    gate_src/andgate.h

SOURCES += \
    server.cpp \
    main.cpp \
    threadedtcpserver.cpp \
    threadedtcpsocket.cpp \
    gate_src/truegate.cpp \
    gate_src/orgate.cpp \
    gate_src/gate.cpp \
    gate_src/falsegate.cpp \
    gate_src/andgate.cpp

QT +=network

OTHER_FILES +=
