MODULE       := geblaat_botmodule_test
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

CXX_SRC += $(SRC_DIR)/utils/time.cpp
CXX_SRC += $(SRC_DIR)/utils/logger.cpp
CXX_SRC += $(SRC_DIR)/utils/threadName.cpp
CXX_SRC += $(SRC_DIR)/utils/classname.cpp

include $(PCDEV_ROOT)/build/make/all.mk

#LDFLAGS += -lgeblaat_pluginloader -L$(SO_DIR)
LDFLAGS += -lgeblaat_client_bot -L$(SO_DIR)
