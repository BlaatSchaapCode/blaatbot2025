MODULE       := geblaat_logger_console
PROJ_DIR     := ../../..
PCDEV_ROOT   := $(PROJ_DIR)/pcdev
OUT_DIR      := $(PROJ_DIR)/out
SRC_DIR      := $(PROJ_DIR)/src

LIBS +=  nlohmann_json  

#CXXFLAGS += -DENABLE_LOG_DEBUG

BUILD_LIBRARY=D

CXX_INCLUDES += $(SRC_DIR)
CXX_INCLUDES += $(SRC_DIR)/logging
CXX_INCLUDES += $(SRC_DIR)/utils

CXX_SRC += $(SRC_DIR)/logging/Logger.cpp
CXX_SRC += $(SRC_DIR)/logging/ConsoleLogger.cpp

include $(PCDEV_ROOT)/build/make/all.mk

$(info target os is $(TARGET_OS) )

LDFLAGS += -lgeblaat_pluginloader -L$(SO_DIR)
