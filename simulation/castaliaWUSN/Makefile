
.PHONY: all clean cleanall makefiles makefiles-so makefiles-lib makefiles-exe checkmakefiles

all: checkmakefiles
	@cd src && $(MAKE)

clean: checkmakefiles
	@cd src && $(MAKE) clean

cleanall: checkmakefiles
	@cd src && $(MAKE) MODE=release clean
	@cd src && $(MAKE) MODE=debug clean
	@rm -f src/Makefile

MAKEMAKE_OPTIONS := -f --deep -o CastaliaBin -O out -I. -DAUTOIMPORT_OMNETPP_NAMESPACE

makefiles: makefiles-exe

makefiles-so:
	@cd src && opp_makemake --make-so $(MAKEMAKE_OPTIONS)

makefiles-lib:
	@cd src && opp_makemake --make-lib $(MAKEMAKE_OPTIONS)

makefiles-exe:
	@cd src && opp_makemake $(MAKEMAKE_OPTIONS)

checkmakefiles:
	@if [ ! -f src/Makefile ]; then \
	echo; \
	echo '========================================================================'; \
	echo 'src/Makefile does not exist. Please use "make makefiles" to generate it!'; \
	echo '========================================================================'; \
	echo; \
	exit 1; \
	fi
