TARGET       := geblaat_protocol_irc
PROJ_DIR     := ../../..
PCDEV_ROOT   := $(PROJ_DIR)/pcdev
OUT_DIR      := $(PROJ_DIR)/out
SRC_DIR      := $(PROJ_DIR)/src

LIBS +=  nlohmann_json  icu-uc

#CXXFLAGS += -DENABLE_LOG_DEBUG

BUILD_LIBRARY=D

CXX_INCLUDES += $(SRC_DIR)
CXX_INCLUDES += $(SRC_DIR)/connection
CXX_INCLUDES += $(SRC_DIR)/protocol
CXX_INCLUDES += $(SRC_DIR)/utils
CXX_INCLUDES += ../ext/base64/include/

CXX_SRC += $(SRC_DIR)/protocol/Protocol.cpp
CXX_SRC += $(SRC_DIR)/protocol/IRC.cpp
CXX_SRC += $(SRC_DIR)/connection/Connection.cpp
CXX_SRC += $(SRC_DIR)/PluginLoadable.cpp
#CXX_SRC += $(SRC_DIR)/PluginLoader.cpp

CXX_SRC += $(SRC_DIR)/utils/time.cpp
CXX_SRC += $(SRC_DIR)/utils/timer.cpp
CXX_SRC += $(SRC_DIR)/utils/logger.cpp
CXX_SRC += $(SRC_DIR)/utils/threadName.cpp
CXX_SRC += $(SRC_DIR)/utils/splitString.cpp


include $(PCDEV_ROOT)/build/make/all.mk

$(info target os is $(TARGET_OS) )


LDFLAGS += -lgeblaat_pluginloader -L$(SO_DIR)
