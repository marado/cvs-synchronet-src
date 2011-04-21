/* Copyright (C), 2007 by Stephen Hurd */

#include <stdio.h>	/* NULL */
#include <unistd.h>	/* _exit() */
#include "st_crypt.h"
#include <xp_dl.h>

int crypt_loaded=0;

#ifdef WITHOUT_CRYPTLIB

int init_crypt()
{
	return(-1);
}

void exit_crypt()
{
}

#else

/*
 * cryptlib calls fork() to gather entropy.
 * It then calls exit().
 * This calls the atexit() handlers.
 * SDL_Exit is in there...
 * SDL doesn't exist in the forked child.
 * This causes the child to spin FOREVER
 * Eating your CPU.
 * So, we will break exit(3).
 */
void exit(int code)
{
	fclose(fopen("/tmp/Dumbass", "w"));
	_exit(code);
}

struct crypt_funcs cl;

int init_crypt(void)
{
	dll_handle	cryptlib;
	const char *libnames[]={ "cl", "cl33", "cl32", NULL };

	if(crypt_loaded)
		return(0);

#ifdef STATIC_CRYPTLIB
	cl.PopData=cryptPopData;
	cl.PushData=cryptPushData;
	cl.FlushData=cryptFlushData;
	cl.Init=cryptInit;
	cl.End=cryptEnd;
	cl.CreateSession=cryptCreateSession;
	cl.GetAttribute=cryptGetAttribute;
	cl.GetAttributeString=cryptGetAttributeString;
	cl.SetAttribute=cryptSetAttribute;
	cl.SetAttributeString=cryptSetAttributeString;
	cl.DestroySession=cryptDestroySession;
	cl.AddRandom=cryptAddRandom;
#else
	cryptlib=xp_dlopen(libnames,RTLD_LAZY, CRYPTLIB_VERSION/1000);
	if(cryptlib==NULL)
		return(-1);
	if((cl.PopData=xp_dlsym(cryptlib,cryptPopData))==NULL) {
		xp_dlclose(cryptlib);
		return(-1);
	}
	if((cl.PushData=xp_dlsym(cryptlib,cryptPushData))==NULL) {
		xp_dlclose(cryptlib);
		return(-1);
	}
	if((cl.FlushData=xp_dlsym(cryptlib,cryptFlushData))==NULL) {
		xp_dlclose(cryptlib);
		return(-1);
	}
	if((cl.Init=xp_dlsym(cryptlib,cryptInit))==NULL) {
		xp_dlclose(cryptlib);
		return(-1);
	}
	if((cl.End=xp_dlsym(cryptlib,cryptEnd))==NULL) {
		xp_dlclose(cryptlib);
		return(-1);
	}
	if((cl.CreateSession=xp_dlsym(cryptlib,cryptCreateSession))==NULL) {
		xp_dlclose(cryptlib);
		return(-1);
	}
	if((cl.GetAttribute=xp_dlsym(cryptlib,cryptGetAttribute))==NULL) {
		xp_dlclose(cryptlib);
		return(-1);
	}
	if((cl.GetAttributeString=xp_dlsym(cryptlib,cryptGetAttributeString))==NULL) {
		xp_dlclose(cryptlib);
		return(-1);
	}
	if((cl.SetAttribute=xp_dlsym(cryptlib,cryptSetAttribute))==NULL) {
		xp_dlclose(cryptlib);
		return(-1);
	}
	if((cl.SetAttributeString=xp_dlsym(cryptlib,cryptSetAttributeString))==NULL) {
		xp_dlclose(cryptlib);
		return(-1);
	}
	if((cl.DestroySession=xp_dlsym(cryptlib,cryptDestroySession))==NULL) {
		xp_dlclose(cryptlib);
		return(-1);
	}
	if((cl.AddRandom=xp_dlsym(cryptlib,cryptAddRandom))==NULL) {
		xp_dlclose(cryptlib);
		return(-1);
	}
#endif
	if(cryptStatusOK(cl.Init())) {
		if(cryptStatusOK(cl.AddRandom(NULL, CRYPT_RANDOM_SLOWPOLL))) {
			crypt_loaded=1;
			return(0);
		}
		cl.End();
	}
	return(-1);
}

void exit_crypt(void)
{
	if(crypt_loaded)
		cl.End();
}

#endif
