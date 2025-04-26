TARGET       := blaatnet_tcp
PROJ_DIR     := ../../..
PCDEV_ROOT   := $(PROJ_DIR)/pcdev
OUT_DIR      := $(PROJ_DIR)/out
SRC_DIR      := $(PROJ_DIR)/src

LIBS +=  nlohmann_json

CXXFLAGS += -DENABLE_LOG_DEBUG

BUILD_LIBRARY=D

CXX_INCLUDES += $(SRC_DIR)/network
CXX_INCLUDES += $(SRC_DIR)/protocol
CXX_INCLUDES += $(SRC_DIR)/utils
CXX_INCLUDES += ../ext/base64/include/

#CXX_SRC += $(SRC_DIR)/network/network.cpp
CXX_SRC += $(SRC_DIR)/network/Connection.cpp
#CXX_SRC += $(SRC_DIR)/network/TlsConnection.cpp
CXX_SRC += $(SRC_DIR)/network/TcpConnection.cpp

CXX_SRC += $(SRC_DIR)/utils/logger.cpp
CXX_SRC += $(SRC_DIR)/utils/threadName.cpp


include $(PCDEV_ROOT)/build/make/all.mk
