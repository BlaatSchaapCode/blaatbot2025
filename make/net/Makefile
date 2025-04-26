TARGET       := blaatnet
PCDEV_ROOT   := ../../pcdev
OUT_DIR      := ../../out
SRC_DIR      := ../../src

LIBS +=  icu-uc nlohmann_json libtls

CXXFLAGS += -DENABLE_LOG_DEBUG

BUILD_LIBRARY=D

CXX_INCLUDES += $(SRC_DIR)/network
CXX_INCLUDES += $(SRC_DIR)/protocol
CXX_INCLUDES += $(SRC_DIR)/utils
#CXX_INCLUDES += ../ext/base64/include/

#CXX_SRC += $(SRC_DIR)/clients/Client.cpp
CXX_SRC += $(SRC_DIR)/network/network.cpp
CXX_SRC += $(SRC_DIR)/network/Connection.cpp
CXX_SRC += $(SRC_DIR)/network/TlsConnection.cpp
CXX_SRC += $(SRC_DIR)/network/TcpConnection.cpp
#CXX_SRC += $(SRC_DIR)/protocol/Protocol.cpp
#CXX_SRC += $(SRC_DIR)/protocol/IRC.cpp
#CXX_SRC += $(SRC_DIR)/utils/compat.cpp
#CXX_SRC += $(SRC_DIR)/utils/logger.cpp
#CXX_SRC += $(SRC_DIR)/utils/splitString.cpp
#CXX_SRC += $(SRC_DIR)/utils/threadName.cpp
#CXX_SRC += $(SRC_DIR)/utils/timer.cpp
#CXX_SRC += $(SRC_DIR)/main.cpp


include $(PCDEV_ROOT)/build/make/all.mk
