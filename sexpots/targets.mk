SEXPOTS		=	$(EXEODIR)$(DIRSEP)sexpots$(EXEFILE)

all: xpdev-mt $(MTOBJODIR) $(EXEODIR) $(SEXPOTS)

$(SEXPOTS):	$(XPDEV-MT_LIB)
