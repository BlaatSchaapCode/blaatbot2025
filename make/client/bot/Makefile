TARGET       := geblaat_client_bot
PROJ_DIR     := ../../..
PCDEV_ROOT   := $(PROJ_DIR)/pcdev
OUT_DIR      := $(PROJ_DIR)/out
SRC_DIR      := $(PROJ_DIR)/src

LIBS +=  nlohmann_json 

#CXXFLAGS += -DENABLE_LOG_DEBUG

BUILD_LIBRARY=D



CXX_INCLUDES += $(SRC_DIR)
CXX_INCLUDES += $(SRC_DIR)/clients
CXX_INCLUDES += $(SRC_DIR)/connection
CXX_INCLUDES += $(SRC_DIR)/protocol
CXX_INCLUDES += $(SRC_DIR)/utils
CXX_INCLUDES += ../ext/base64/include/

CXX_SRC += $(SRC_DIR)/clients/BotClient.cpp
CXX_SRC += $(SRC_DIR)/PluginLoadable.cpp
CXX_SRC += $(SRC_DIR)/PluginLoader.cpp

CXX_SRC += $(SRC_DIR)/botmodule/CAPI_BotModule.cpp


CXX_SRC += $(SRC_DIR)/connection/Connection.cpp
CXX_SRC += $(SRC_DIR)/clients/Client.cpp



CXX_SRC += $(SRC_DIR)/utils/time.cpp
CXX_SRC += $(SRC_DIR)/utils/logger.cpp
CXX_SRC += $(SRC_DIR)/utils/threadName.cpp
CXX_SRC += $(SRC_DIR)/utils/classname.cpp


include $(PCDEV_ROOT)/build/make/all.mk


