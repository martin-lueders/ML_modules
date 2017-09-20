
SOURCES = $(wildcard src/*.cpp)

include ../../plugin.mk


dist: all
	mkdir -p dist/ML-Modules
	cp LICENSE* dist/ML-Modules/
	cp plugin.* dist/ML-Modules/
	cp -R res dist/ML-Modules/
