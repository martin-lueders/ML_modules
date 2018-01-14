# this flag selects the rack version to compile against.
# 

SLUG = ML_modules
VERSION = 0.5.4

FLAGS += -D v_dev

SOURCES = $(wildcard src/*.cpp freeverb/*.cpp)

include ../../plugin.mk


# Convenience target for packaging files into a ZIP file
.PHONY: dist
dist: all
	mkdir -p dist/$(SLUG)
	cp LICENSE* dist/$(SLUG)/
	cp $(TARGET) dist/$(SLUG)/
	cp -R res dist/$(SLUG)/
	cd dist && zip -5 -r $(SLUG)-$(VERSION)-$(ARCH).zip $(SLUG)
