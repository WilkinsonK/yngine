BUILDDIR=./build
EXECUTABLE=$(BUILDDIR)/engine/Engine

.PHONY: all clean compile run
all: compile

clean: $(BUILDDIR)
	@rm -rf $(BUILDDIR)

compile: $(BUILDDIR)/Makefile
	@cd $(BUILDDIR) && $(MAKE)

run: $(EXECUTABLE)
	@$(EXECUTABLE)

$(BUILDDIR)/Makefile: CMakeLists.txt engine/CMakeLists.txt engine/*/CMakeLists.txt
	@cmake -B $(BUILDDIR)
