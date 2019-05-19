

SOURCES += $(wildcard src/*.cpp)
SOURCES += $(wildcard freeverb/*.cpp)

DISTRIBUTABLES += $(wildcard LICENSE*) res

RACK_DIR ?= ../..
include $(RACK_DIR)/plugin.mk
