UIFCTEST	=	$(EXEODIR)$(DIRSEP)uifctest$(EXEFILE)

all: lib mtlib test

test: mtlib ciolib-mt xpdevlib-mt $(EXEODIR) $(UIFCTEST)

lib: $(OBJODIR) $(LIBODIR) $(UIFCLIB)
mtlib: $(MTOBJODIR) $(LIBODIR) $(UIFCLIB-MT)
