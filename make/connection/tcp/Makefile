MODULE       := geblaat_connection_tcp
PROJ_DIR     := ../../..
PCDEV_ROOT   := $(PROJ_DIR)/pcdev
OUT_DIR      := $(PROJ_DIR)/out
SRC_DIR      := $(PROJ_DIR)/src

LIBS +=  nlohmann_json 

#CXXFLAGS += -DENABLE_LOG_DEBUG

BUILD_LIBRARY=D

CXX_INCLUDES += $(SRC_DIR)
CXX_INCLUDES += $(SRC_DIR)/connection
CXX_INCLUDES += $(SRC_DIR)/protocol
CXX_INCLUDES += $(SRC_DIR)/utils
CXX_INCLUDES += ../ext/base64/include/

CXX_SRC += $(SRC_DIR)/connection/Connection.cpp
CXX_SRC += $(SRC_DIR)/connection/TcpConnection.cpp
CXX_SRC += $(SRC_DIR)/PluginLoadable.cpp

CXX_SRC += $(SRC_DIR)/utils/time.cpp
CXX_SRC += $(SRC_DIR)/utils/logger.cpp
CXX_SRC += $(SRC_DIR)/utils/threadName.cpp


include $(PCDEV_ROOT)/build/make/all.mk

$(info target os is $(TARGET_OS) )

ifeq ($(TARGET_OS),haiku)
	LDFLAGS += -lnetwork
endif

ifeq ($(TARGET_OS),mingw)
	LDFLAGS += -lws2_32
endif

ifdef MSYSTEM
	LDFLAGS += -lws2_32
endif

ifeq ($(TARGET_OS),sunos)
	LDFLAGS += -lsocket -lnsl
endif
