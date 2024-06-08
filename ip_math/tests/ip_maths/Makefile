COMPONENT_NAME = $(notdir $(CURDIR))

MAKEFILE_DIR = $(dir $(realpath $(firstword $(MAKEFILE_LIST))))
PROJECT_DIR = $(realpath $(MAKEFILE_DIR)../..)
SRC_DIR = $(PROJECT_DIR)/src
TEST_DIR = $(PROJECT_DIR)/tests

INCLUDE_DIRS = $(SRC_DIR)/$(COMPONENT_NAME)
SRC_DIRS = $(SRC_DIR)/$(COMPONENT_NAME)

CPPUTEST_HOME ?= $(PROJECT_DIR)/../tools/cpputest
INCLUDE_DIRS += $(CPPUTEST_HOME)/include

TEST_SRC_DIRS = $(TEST_DIR)/$(COMPONENT_NAME)

include $(CPPUTEST_HOME)/build/MakefileWorker.mk
