TARGET      = blaatbot2025
PCDEV_ROOT   = ../pcdev
OUT_DIR      = ../out

# adding tls to LIBS ain't working?
#LIBS += tls
LDFLAGS += -ltls

CXX_INCLUDES += network
CXX_INCLUDES += protocol
CXX_INCLUDES += utils
CXX_INCLUDES += ../ext/base64/include/

CXX_SRC += ./clients/Client.cpp
CXX_SRC += ./network/network.cpp
CXX_SRC += ./network/Connection.cpp
CXX_SRC += ./network/TlsConnection.cpp
CXX_SRC += ./network/TcpConnection.cpp
CXX_SRC += ./protocol/Protocol.cpp
CXX_SRC += ./protocol/IRC.cpp
CXX_SRC += ./utils/compat.cpp
CXX_SRC += ./utils/logger.cpp
CXX_SRC += ./utils/splitString.cpp
CXX_SRC += ./utils/threadName.cpp
CXX_SRC += ./main.cpp


include $(PCDEV_ROOT)/build/make/all.mk
