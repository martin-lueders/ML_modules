# this flag selects the rack version to compile against.
# 
# possible values are v032 and v040

FLAGS += -D v_dev

SOURCES = $(wildcard src/*.cpp freeverb/*.cpp)

include ../../plugin.mk


dist: all
	mkdir -p dist/ML_modules
	cp LICENSE* dist/ML_modules/
	cp plugin.* dist/ML_modules/
	cp -R res dist/ML_modules/
