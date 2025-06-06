TARGET       := geblaat_connection_gnutls
PROJ_DIR     := ../../..
PCDEV_ROOT   := $(PROJ_DIR)/pcdev
OUT_DIR      := $(PROJ_DIR)/out
SRC_DIR      := $(PROJ_DIR)/src

LIBS +=  nlohmann_json gnutls

# CXXFLAGS += -DENABLE_LOG_DEBUG
CXXFLAGS += -DUSING_TCPCONNECTION

BUILD_LIBRARY=D

CXX_INCLUDES += $(SRC_DIR)
CXX_INCLUDES += $(SRC_DIR)/connection
CXX_INCLUDES += $(SRC_DIR)/protocol
CXX_INCLUDES += $(SRC_DIR)/utils
CXX_INCLUDES += ../ext/base64/include/

CXX_SRC += $(SRC_DIR)/connection/Connection.cpp
CXX_SRC += $(SRC_DIR)/connection/TcpConnection.cpp
CXX_SRC += $(SRC_DIR)/connection/TlsConnection.cpp
CXX_SRC += $(SRC_DIR)/connection/GnuTlsConnection.cpp
CXX_SRC += $(SRC_DIR)/PluginLoadable.cpp

CXX_SRC += $(SRC_DIR)/utils/time.cpp
CXX_SRC += $(SRC_DIR)/utils/logger.cpp
CXX_SRC += $(SRC_DIR)/utils/threadName.cpp

include $(PCDEV_ROOT)/build/make/all.mk





