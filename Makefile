
SOURCES = $(wildcard src/*.cpp)

include ../../plugin.mk


dist: all
	mkdir -p dist/ML-Modules
	cp LICENSE* dist/ML_Modules/
	cp plugin.* dist/ML_Modules/
	cp -R res dist/ML_Modules/
