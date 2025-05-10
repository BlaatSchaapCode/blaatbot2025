TARGET       := geblaat_botmodule_test
PROJ_DIR     := ../../..
PCDEV_ROOT   := $(PROJ_DIR)/pcdev
OUT_DIR      := $(PROJ_DIR)/out
SRC_DIR      := $(PROJ_DIR)/src

LIBS +=  nlohmann_json 

#CXXFLAGS += -DENABLE_LOG_DEBUG

BUILD_LIBRARY=D

CXX_INCLUDES += $(SRC_DIR)
CXX_INCLUDES += $(SRC_DIR)/clients
CXX_INCLUDES += $(SRC_DIR)/botmodule
CXX_INCLUDES += $(SRC_DIR)/utils
CXX_INCLUDES += ../ext/base64/include/

CXX_SRC += $(SRC_DIR)/PluginLoadable.cpp
CXX_SRC += $(SRC_DIR)/botmodule/TestBotModule.cpp


# Hmm I don't want this in here but???
#CXX_INCLUDES += $(SRC_DIR)/connection
#CXX_INCLUDES += $(SRC_DIR)/protocol
#CXX_SRC += $(SRC_DIR)/clients/Client.cpp
#CXX_SRC += $(SRC_DIR)/clients/BotClient.cpp
#CXX_SRC += $(SRC_DIR)/connection/Connection.cpp
#CXX_SRC += $(SRC_DIR)/PluginLoader.cpp


CXX_SRC += $(SRC_DIR)/utils/time.cpp
CXX_SRC += $(SRC_DIR)/utils/logger.cpp
CXX_SRC += $(SRC_DIR)/utils/threadName.cpp


include $(PCDEV_ROOT)/build/make/all.mk


# On Linux, we load out plugins into global symbol space. Windows does not
# appear to support this. The resulting dll fails to build due linker errors
# As a solution, we link the BotClient dll against the BotModule
ifeq ($(TARGET_OS),mingw)
	LDFLAGS += $(SO_DIR)/geblaat_client_bot.dll
endif
