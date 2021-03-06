# Makefile

#########################################################################
# Makefile for Synchronet BBS Menu Editor (menuedit)
# For use with Borland C++ Builder 5+ or Borland C++ 5.5 for Win32      #
# @format.tab-size 4													#
#########################################################################

# $Id$

# Macros
#DEBUG	=	1				# Comment out for release (non-debug) version
#USE_FLTK=	1
USE_UIFC32= 1
CC		=	bcc32
LD		=	ilink32
SLASH	=	\\
OFILE	=	obj
EXEFILE	=	.exe
EXEODIR	=	bcc.win32	# Executable output directory
XPDEV	=   	..\..\xpdev
UIFC	=	..\..\uifc
LIB	=	..\..\..\lib
INCLUDE	=	..\..\..\include
CFLAGS	=	-M -I..;$(UIFC);$(XPDEV) 
DELETE	=	-@echo y | del 

.path.c = .;..;$(UIFC);$(XPDEV)
.path.cpp = $(UIFC)

# Enable auto-dependency checking
.autodepend
.cacheautodepend	

# Optional compile flags (disable banner, warnings and such)
CFLAGS	=	$(CFLAGS) -q -d -H -X- -w-csu -w-pch -w-ccc -w-rch -w-par -w-pro -w-8004
#-w-aus

# Debug or release build?
!ifdef DEBUG
CFLAGS	=	$(CFLAGS) -v -Od -D_DEBUG 
LFLAGS	=	$(LFLAGS) -v
EXEODIR	=	$(EXEODIR).debug
!else
EXEODIR	=	$(EXEODIR).release
!endif
LIBODIR =	$(EXEODIR)

# Cross platform/compiler definitions
!include targets.mk	# defines all targets
!include objects.mk	# defines $(OBJS)

!ifdef USE_UIFC32
OBJS = $(OBJS) $(EXEODIR)\uifc32.$(OFILE)
CFLAGS  =   $(CFLAGS) -DUSE_UIFC32
!else
OBJS = $(OBJS) $(EXEODIR)\uifc.$(OFILE)
!endif

!ifdef USE_FLTK
CFLAGS 	= $(CFLAGS) -DUSE_FLTK -I$(INCLUDE)\fltk
OBJS 	= $(OBJS) $(EXEODIR)\uifcfltk.$(OFILE) $(LIB)/fltk/win32/fltk.lib
!endif

# Implicit C Compile Rule for SCFG
.c.$(OFILE):
	@$(CC) $(CFLAGS) -n$(EXEODIR) -c $<

.cpp.$(OFILE):
	@$(CC) $(CFLAGS) -n$(EXEODIR) -c $<

# Create output directory if it doesn't exist
$(EXEODIR):
	@if not exist $(EXEODIR) mkdir $(EXEODIR)

# MenuEdit Build Rule
$(MENUEDIT): $(OBJS)
	@echo Linking $@
	@$(CC) $(CFLAGS) -e$@ $**

clean:
	@echo Deleting $(EXEODIR)
	$(QUIET)$(DELETE) $(EXEODIR)\*
