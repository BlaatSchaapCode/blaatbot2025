MODULE       := blaatbot2025
PCDEV_ROOT   := ../../pcdev
OUT_DIR      := ../../out
SRC_DIR      := ../../src

LIBS +=  nlohmann_json

#CXXFLAGS += -DENABLE_LOG_DEBUG

CXX_INCLUDES += $(SRC_DIR)
CXX_INCLUDES += $(SRC_DIR)/connection
CXX_INCLUDES += $(SRC_DIR)/protocol
CXX_INCLUDES += $(SRC_DIR)/utils

CXX_SRC += $(SRC_DIR)/connection/Connection.cpp
CXX_SRC += $(SRC_DIR)/protocol/Protocol.cpp
CXX_SRC += $(SRC_DIR)/utils/logger.cpp
CXX_SRC += $(SRC_DIR)/utils/threadName.cpp
CXX_SRC += $(SRC_DIR)/utils/classname.cpp
CXX_SRC += $(SRC_DIR)/utils/time.cpp
CXX_SRC += $(SRC_DIR)/utils/timer.cpp
CXX_SRC += $(SRC_DIR)/main.cpp

include $(PCDEV_ROOT)/build/make/all.mk

LDFLAGS += -lgeblaat_pluginloader -L$(SO_DIR)

ifeq ($(TARGET_OS),freebsd)
	LDFLAGS += -lpthread
endif

ifeq ($(TARGET_OS),mingw)
	LDFLAGS += -ldbghelp
endif
