#--------------------------------------------------------------------------
# Name         : content.mak
# Title        : Makefile to build content files
#
# Copyright    : Copyright (C) by Imagination Technologies Limited.
#
# Description  : Makefile to wrap content files for examples in the PowerVR SDK
#
# Platform     :
#
#--------------------------------------------------------------------------

#############################################################################
## Variables
#############################################################################
FILEWRAP 	= ..\..\..\..\Utilities\Filewrap\Windows_x86_32\Filewrap.exe
CONTENTDIR = Content

#############################################################################
## Instructions
#############################################################################

RESOURCES = \
	$(CONTENTDIR)/FragShader.cpp \
	$(CONTENTDIR)/VertShader.cpp \
	$(CONTENTDIR)/SceneFragShader.cpp \
	$(CONTENTDIR)/SceneVertShader.cpp \
	$(CONTENTDIR)/BackgroundFragShader.cpp \
	$(CONTENTDIR)/Mask.cpp \
	$(CONTENTDIR)/MaskTexture.cpp \
	$(CONTENTDIR)/Background.cpp \
	$(CONTENTDIR)/identity.cpp \
	$(CONTENTDIR)/cooler.cpp \
	$(CONTENTDIR)/warmer.cpp \
	$(CONTENTDIR)/bw.cpp \
	$(CONTENTDIR)/sepia.cpp \
	$(CONTENTDIR)/inverted.cpp \
	$(CONTENTDIR)/highcontrast.cpp \
	$(CONTENTDIR)/bluewhitegradient.cpp

all: resources
	
help:
	@echo Valid targets are:
	@echo resources, clean
	@echo FILEWRAP can be used to override the default path to the Filewrap utility.

clean:
	@for i in $(RESOURCES); do test -f $$i && rm -vf $$i || true; done

resources: $(RESOURCES)

$(CONTENTDIR):
	-mkdir "$@"

$(CONTENTDIR)/FragShader.cpp: $(CONTENTDIR) ./FragShader.fsh
	$(FILEWRAP)  -s  -o $@ ./FragShader.fsh

$(CONTENTDIR)/VertShader.cpp: $(CONTENTDIR) ./VertShader.vsh
	$(FILEWRAP)  -s  -o $@ ./VertShader.vsh

$(CONTENTDIR)/SceneFragShader.cpp: $(CONTENTDIR) ./SceneFragShader.fsh
	$(FILEWRAP)  -s  -o $@ ./SceneFragShader.fsh

$(CONTENTDIR)/SceneVertShader.cpp: $(CONTENTDIR) ./SceneVertShader.vsh
	$(FILEWRAP)  -s  -o $@ ./SceneVertShader.vsh

$(CONTENTDIR)/BackgroundFragShader.cpp: $(CONTENTDIR) ./BackgroundFragShader.fsh
	$(FILEWRAP)  -s  -o $@ ./BackgroundFragShader.fsh

$(CONTENTDIR)/Mask.cpp: $(CONTENTDIR)
	$(FILEWRAP)  -o $@ ./Mask.pod

$(CONTENTDIR)/MaskTexture.cpp: $(CONTENTDIR)
	$(FILEWRAP)  -o $@ ./MaskTexture.pvr

$(CONTENTDIR)/Background.cpp: $(CONTENTDIR)
	$(FILEWRAP)  -o $@ ./Background.pvr

$(CONTENTDIR)/identity.cpp: $(CONTENTDIR)
	$(FILEWRAP)  -o $@ ./identity.pvr

$(CONTENTDIR)/cooler.cpp: $(CONTENTDIR)
	$(FILEWRAP)  -o $@ ./cooler.pvr

$(CONTENTDIR)/warmer.cpp: $(CONTENTDIR)
	$(FILEWRAP)  -o $@ ./warmer.pvr

$(CONTENTDIR)/bw.cpp: $(CONTENTDIR)
	$(FILEWRAP)  -o $@ ./bw.pvr

$(CONTENTDIR)/sepia.cpp: $(CONTENTDIR)
	$(FILEWRAP)  -o $@ ./sepia.pvr

$(CONTENTDIR)/inverted.cpp: $(CONTENTDIR)
	$(FILEWRAP)  -o $@ ./inverted.pvr

$(CONTENTDIR)/highcontrast.cpp: $(CONTENTDIR)
	$(FILEWRAP)  -o $@ ./highcontrast.pvr

$(CONTENTDIR)/bluewhitegradient.cpp: $(CONTENTDIR)
	$(FILEWRAP)  -o $@ ./bluewhitegradient.pvr

############################################################################
# End of file (content.mak)
############################################################################
