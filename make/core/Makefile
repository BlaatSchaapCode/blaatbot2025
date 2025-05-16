TARGET       := blaatbot2025
PCDEV_ROOT   := ../../pcdev
OUT_DIR      := ../../out
SRC_DIR      := ../../src

LIBS +=  icu-uc nlohmann_json



#CXXFLAGS += -DENABLE_LOG_DEBUG

CXX_INCLUDES += $(SRC_DIR)
CXX_INCLUDES += $(SRC_DIR)/connection
CXX_INCLUDES += $(SRC_DIR)/protocol
CXX_INCLUDES += $(SRC_DIR)/utils
#CXX_INCLUDES += ../ext/base64/include/

CXX_SRC += $(SRC_DIR)/connection/Connection.cpp
CXX_SRC += $(SRC_DIR)/protocol/Protocol.cpp
CXX_SRC += $(SRC_DIR)/utils/compat.cpp
CXX_SRC += $(SRC_DIR)/utils/logger.cpp
CXX_SRC += $(SRC_DIR)/utils/splitString.cpp
CXX_SRC += $(SRC_DIR)/utils/threadName.cpp
CXX_SRC += $(SRC_DIR)/utils/classname.cpp
CXX_SRC += $(SRC_DIR)/utils/time.cpp
CXX_SRC += $(SRC_DIR)/utils/timer.cpp
CXX_SRC += $(SRC_DIR)/main.cpp



# Temporary added here to fix some linker errors
# We should split the PluginLoader off its own library
# Also, the plugin loader should be extendible such that
# CAPI_BotModule can live in the BotClient library
# without causing the linker issues requiring it
# to be linked into unrelated components
#CXX_SRC += $(SRC_DIR)/botmodule/CAPI_BotModule.cpp
#CXX_SRC += $(SRC_DIR)/clients/BotClient.cpp
#CXX_SRC += $(SRC_DIR)/clients/Client.cpp
#CXX_SRC += $(SRC_DIR)/PluginLoader.cpp



include $(PCDEV_ROOT)/build/make/all.mk


LDFLAGS += -lgeblaat_pluginloader -L$(SO_DIR)
