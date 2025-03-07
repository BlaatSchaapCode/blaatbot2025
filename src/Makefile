TARGET      = blaatbot2025
PCDEV_ROOT   = ../pcdev
OUT_DIR      = ../out

# adding tls to LIBS ain't working?
#LIBS += tls
LDFLAGS += -ltls

CXX_SRC += ./network/network.cpp
CXX_SRC += ./network/connection.cpp
CXX_SRC += ./network/tlsConnection.cpp
CXX_SRC += ./network/tcpConnection.cpp
CXX_SRC += ./utils/compat.cpp
CXX_SRC += ./utils/logger.cpp
CXX_SRC += ./main.cpp


include $(PCDEV_ROOT)/build/make/all.mk
