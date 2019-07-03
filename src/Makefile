REAL_MAKEFILE=Makefile.real
PARA_BUILD=-j $(shell ./countNbProcessors.sh)

all: para
nonpara:
	$(MAKE) -f $(REAL_MAKEFILE) $(PARA_BUILD)
intel:
	$(MAKE) -f $(REAL_MAKEFILE) INTEL=1
para:
	$(MAKE) -f $(REAL_MAKEFILE) $(PARA_BUILD) PARA=1
rl:
	$(MAKE) -f $(REAL_MAKEFILE) RL=1
clean:
	$(MAKE) -f $(REAL_MAKEFILE) clean
	rm -f `find . -name "*.o"` mogo

noMpi:
	$(MAKE) -f $(REAL_MAKEFILE) $(PARA_BUILD) PARA=1 noMPI=1
