/* js_msg_area.c */

/* Synchronet JavaScript "Message Area" Object */

/* $Id$ */

/****************************************************************************
 * @format.tab-size 4		(Plain Text/Source Code File Header)			*
 * @format.use-tabs true	(see http://www.synchro.net/ptsc_hdr.html)		*
 *																			*
 * Copyright 2011 Rob Swindell - http://www.synchro.net/copyright.html		*
 *																			*
 * This program is free software; you can redistribute it and/or			*
 * modify it under the terms of the GNU General Public License				*
 * as published by the Free Software Foundation; either version 2			*
 * of the License, or (at your option) any later version.					*
 * See the GNU General Public License for more details: gpl.txt or			*
 * http://www.fsf.org/copyleft/gpl.html										*
 *																			*
 * Anonymous FTP access to the most recent released source is available at	*
 * ftp://vert.synchro.net, ftp://cvs.synchro.net and ftp://ftp.synchro.net	*
 *																			*
 * Anonymous CVS access to the development source and modification history	*
 * is available at cvs.synchro.net:/cvsroot/sbbs, example:					*
 * cvs -d :pserver:anonymous@cvs.synchro.net:/cvsroot/sbbs login			*
 *     (just hit return, no password is necessary)							*
 * cvs -d :pserver:anonymous@cvs.synchro.net:/cvsroot/sbbs checkout src		*
 *																			*
 * For Synchronet coding style and modification guidelines, see				*
 * http://www.synchro.net/source.html										*
 *																			*
 * You are encouraged to submit any modifications (preferably in Unix diff	*
 * format) via e-mail to mods@synchro.net									*
 *																			*
 * Note: If this box doesn't appear square, then you need to fix your tabs.	*
 ****************************************************************************/

#include "sbbs.h"

#ifdef JAVASCRIPT

#ifdef BUILD_JSDOCS

static char* msg_area_prop_desc[] = {
	  "message area settings (bitfield) - see <tt>MM_*</tt> in <tt>sbbsdefs.js</tt> for details"
	,NULL
};

static char* msg_grp_prop_desc[] = {
	 "index into grp_list array (or -1 if not in array) <i>(introduced in v3.12)</i>"
	,"unique number for this message group"
	,"group name"
	,"group description"
	,"group access requirements"
	,NULL
};

static char* msg_sub_prop_desc[] = {

	 "index into sub_list array (or -1 if not in array) <i>(introduced in v3.12)</i>"
	,"group's index into grp_list array <i>(introduced in v3.12)</i>"
	,"unique number for this sub-board"
	,"group number"
	,"group name <i>(introduced in v3.12)</i>"
	,"sub-board internal code"
	,"sub-board name"
	,"sub-board description"
	,"sub-board QWK name"
	,"newsgroup name (as configured or dymamically generated)"
	,"sub-board access requirements"
	,"sub-board reading requirements"
	,"sub-board posting requirements"
	,"sub-board operator requirements"
	,"sub-board moderated-user requirements (if non-blank)"
	,"sub-board data storage location"
	,"FidoNet origin line"
	,"QWK Network tagline"
	,"toggle options (bitfield)"
	,"index into message scan configuration/pointer file"
	,"QWK conference number"
	,"configured maximum number of message CRCs to store (for dupe checking)"
	,"configured maximum number of messages before purging"
	,"configured maximum age (in days) of messages before expiration"
	/* Insert here */
	,"user has sufficient access to read messages"
	,"user has sufficient access to post messages"
	,"user has operator access to this message area"
	,"user's posts are moderated"
	,"user's current new message scan pointer (highest-read message number)"
	,"user's message scan configuration (bitfield) see <tt>SCAN_CFG_*</tt> in <tt>sbbsdefs.js</tt> for valid bits"
	,"user's last-read message number"
	,NULL
};
#endif

BOOL DLLCALL js_CreateMsgAreaProperties(JSContext* cx, scfg_t* cfg, JSObject* subobj, uint subnum)
{
	char		str[128];
	int			c;
	JSString*	js_str;
	jsval		val;
	sub_t*		sub;

	if(subnum==INVALID_SUB || subnum>=cfg->total_subs)
		return(FALSE);

	sub=cfg->sub[subnum];

	if(!JS_DefineProperty(cx, subobj, "number", INT_TO_JSVAL(subnum)
		,NULL,NULL,JSPROP_ENUMERATE|JSPROP_READONLY))
		return(FALSE);

	if(!JS_DefineProperty(cx, subobj, "grp_number", INT_TO_JSVAL(sub->grp)
		,NULL,NULL,JSPROP_ENUMERATE|JSPROP_READONLY))
		return(FALSE);

	if((js_str=JS_NewStringCopyZ(cx, cfg->grp[sub->grp]->sname))==NULL)
		return(FALSE);
	if(!JS_DefineProperty(cx, subobj, "grp_name", STRING_TO_JSVAL(js_str)
		,NULL,NULL,JSPROP_ENUMERATE|JSPROP_READONLY))
		return(FALSE);

	if((js_str=JS_NewStringCopyZ(cx, sub->code))==NULL)
		return(FALSE);
	if(!JS_DefineProperty(cx, subobj, "code", STRING_TO_JSVAL(js_str)
		,NULL,NULL,JSPROP_ENUMERATE|JSPROP_READONLY))
		return(FALSE);

	if((js_str=JS_NewStringCopyZ(cx, sub->sname))==NULL)
		return(FALSE);
	if(!JS_DefineProperty(cx, subobj, "name", STRING_TO_JSVAL(js_str)
		,NULL,NULL,JSPROP_ENUMERATE|JSPROP_READONLY))
		return(FALSE);

	if((js_str=JS_NewStringCopyZ(cx, sub->lname))==NULL)
		return(FALSE);
	if(!JS_DefineProperty(cx, subobj, "description", STRING_TO_JSVAL(js_str)
		,NULL,NULL,JSPROP_ENUMERATE|JSPROP_READONLY))
		return(FALSE);

	if((js_str=JS_NewStringCopyZ(cx, sub->qwkname))==NULL)
		return(FALSE);
	if(!JS_DefineProperty(cx, subobj, "qwk_name", STRING_TO_JSVAL(js_str)
		,NULL,NULL,JSPROP_ENUMERATE|JSPROP_READONLY))
		return(FALSE);

	if(sub->newsgroup[0])
		SAFECOPY(str,sub->newsgroup);
	else {
		sprintf(str,"%s.%s",cfg->grp[sub->grp]->sname,sub->sname);
		for(c=0;str[c];c++)
			if(str[c]==' ')
				str[c]='_';
	}
	if((js_str=JS_NewStringCopyZ(cx, str))==NULL)
		return(FALSE);
	if(!JS_DefineProperty(cx, subobj, "newsgroup", STRING_TO_JSVAL(js_str)
		,NULL,NULL,JSPROP_ENUMERATE|JSPROP_READONLY))
		return(FALSE);

	if((js_str=JS_NewStringCopyZ(cx, sub->arstr))==NULL)
		return(FALSE);
	if(!JS_DefineProperty(cx, subobj, "ars", STRING_TO_JSVAL(js_str)
		,NULL,NULL,JSPROP_ENUMERATE|JSPROP_READONLY))
		return(FALSE);

	if((js_str=JS_NewStringCopyZ(cx, sub->read_arstr))==NULL)
		return(FALSE);
	if(!JS_DefineProperty(cx, subobj, "read_ars", STRING_TO_JSVAL(js_str)
		,NULL,NULL,JSPROP_ENUMERATE|JSPROP_READONLY))
		return(FALSE);

	if((js_str=JS_NewStringCopyZ(cx, sub->post_arstr))==NULL)
		return(FALSE);
	if(!JS_DefineProperty(cx, subobj, "post_ars", STRING_TO_JSVAL(js_str)
		,NULL,NULL,JSPROP_ENUMERATE|JSPROP_READONLY))
		return(FALSE);

	if((js_str=JS_NewStringCopyZ(cx, sub->op_arstr))==NULL)
		return(FALSE);
	if(!JS_DefineProperty(cx, subobj, "operator_ars", STRING_TO_JSVAL(js_str)
		,NULL,NULL,JSPROP_ENUMERATE|JSPROP_READONLY))
		return(FALSE);

	if((js_str=JS_NewStringCopyZ(cx, sub->mod_arstr))==NULL)
		return(FALSE);
	if(!JS_DefineProperty(cx, subobj, "moderated_ars", STRING_TO_JSVAL(js_str)
		,NULL,NULL,JSPROP_ENUMERATE|JSPROP_READONLY))
		return(FALSE);

	if((js_str=JS_NewStringCopyZ(cx, sub->data_dir))==NULL)
		return(FALSE);
	if(!JS_DefineProperty(cx, subobj, "data_dir", STRING_TO_JSVAL(js_str)
		,NULL,NULL,JSPROP_ENUMERATE|JSPROP_READONLY))
		return(FALSE);

	if((js_str=JS_NewStringCopyZ(cx, sub->origline))==NULL)
		return(FALSE);
	if(!JS_DefineProperty(cx, subobj, "fidonet_origin", STRING_TO_JSVAL(js_str)
		,NULL,NULL,JSPROP_ENUMERATE|JSPROP_READONLY))
		return(FALSE);

	if((js_str=JS_NewStringCopyZ(cx, sub->tagline))==NULL)
		return(FALSE);
	if(!JS_DefineProperty(cx, subobj, "qwknet_tagline", STRING_TO_JSVAL(js_str)
		,NULL,NULL,JSPROP_ENUMERATE|JSPROP_READONLY))
		return(FALSE);

	val=UINT_TO_JSVAL(sub->misc);
	if(!JS_DefineProperty(cx, subobj, "settings", val
		,NULL,NULL,JSPROP_ENUMERATE|JSPROP_READONLY))
		return(FALSE);

	if(!JS_DefineProperty(cx, subobj, "ptridx", INT_TO_JSVAL(sub->ptridx)
		,NULL,NULL,JSPROP_ENUMERATE|JSPROP_READONLY))
		return(FALSE);

	if(!JS_DefineProperty(cx, subobj, "qwk_conf", INT_TO_JSVAL(sub->qwkconf)
		,NULL,NULL,JSPROP_ENUMERATE|JSPROP_READONLY))
		return(FALSE);

	if(!JS_DefineProperty(cx, subobj, "max_crcs", INT_TO_JSVAL(sub->maxcrcs)
		,NULL,NULL,JSPROP_ENUMERATE|JSPROP_READONLY))
		return(FALSE);

	if(!JS_DefineProperty(cx, subobj, "max_msgs", INT_TO_JSVAL(sub->maxmsgs)
		,NULL,NULL,JSPROP_ENUMERATE|JSPROP_READONLY))
		return(FALSE);

	if(!JS_DefineProperty(cx, subobj, "max_age", INT_TO_JSVAL(sub->maxage)
		,NULL,NULL,JSPROP_ENUMERATE|JSPROP_READONLY))
		return(FALSE);

#ifdef BUILD_JSDOCS
	js_CreateArrayOfStrings(cx, subobj, "_property_desc_list", msg_sub_prop_desc, JSPROP_READONLY);
#endif

	return(TRUE);
}

/*******************************************/
/* Re-writable Sub-board Object Properites */
/*******************************************/
enum {
	 SUB_PROP_SCAN_PTR
	,SUB_PROP_SCAN_CFG
	,SUB_PROP_LAST_READ
};


static JSBool js_sub_get(JSContext *cx, JSObject *obj, jsid id, jsval *vp)
{
	jsval idval;
    jsint       tiny;
	subscan_t*	scan;

	if((scan=(subscan_t*)JS_GetPrivate(cx,obj))==NULL)
		return(JS_TRUE);

    JS_IdToValue(cx, id, &idval);
    tiny = JSVAL_TO_INT(idval);

	switch(tiny) {
		case SUB_PROP_SCAN_PTR:
			*vp=UINT_TO_JSVAL(scan->ptr);
			break;
		case SUB_PROP_SCAN_CFG:
			*vp=UINT_TO_JSVAL(scan->cfg);
			break;
		case SUB_PROP_LAST_READ:
			*vp=UINT_TO_JSVAL(scan->last);
			break;
	}

	return(JS_TRUE);
}

static JSBool js_sub_set(JSContext *cx, JSObject *obj, jsid id, JSBool strict, jsval *vp)
{
	jsval idval;
	int32		val=0;
    jsint       tiny;
	subscan_t*	scan;

	if((scan=(subscan_t*)JS_GetPrivate(cx,obj))==NULL)
		return(JS_TRUE);

    JS_IdToValue(cx, id, &idval);
    tiny = JSVAL_TO_INT(idval);

	switch(tiny) {
		case SUB_PROP_SCAN_PTR:
			if(!JS_ValueToInt32(cx, *vp, (int32*)&scan->ptr))
				return JS_FALSE;
			break;
		case SUB_PROP_SCAN_CFG:
			if(!JS_ValueToInt32(cx, *vp, &val))
				return JS_FALSE;
			scan->cfg=(ushort)val;
			break;
		case SUB_PROP_LAST_READ:
			if(!JS_ValueToInt32(cx, *vp, (int32*)&scan->last))
				return JS_FALSE;
			break;
	}

	return(JS_TRUE);
}

static struct JSPropertySpec js_sub_properties[] = {
/*		 name				,tinyid		,flags	*/

	{	"scan_ptr"	,SUB_PROP_SCAN_PTR	,JSPROP_ENUMERATE|JSPROP_SHARED },
	{	"scan_cfg"	,SUB_PROP_SCAN_CFG	,JSPROP_ENUMERATE|JSPROP_SHARED },
	{	"last_read"	,SUB_PROP_LAST_READ	,JSPROP_ENUMERATE|JSPROP_SHARED },
	{0}
};


static JSClass js_sub_class = {
     "MsgSub"				/* name			*/
    ,JSCLASS_HAS_PRIVATE	/* flags		*/
	,JS_PropertyStub		/* addProperty	*/
	,JS_PropertyStub		/* delProperty	*/
	,js_sub_get				/* getProperty	*/
	,js_sub_set				/* setProperty	*/
	,JS_EnumerateStub		/* enumerate	*/
	,JS_ResolveStub			/* resolve		*/
	,JS_ConvertStub			/* convert		*/
	,JS_FinalizeStub		/* finalize		*/
};

JSObject* DLLCALL js_CreateMsgAreaObject(JSContext* cx, JSObject* parent, scfg_t* cfg
										  ,user_t* user, client_t* client, subscan_t* subscan)
{
	JSObject*	areaobj;
	JSObject*	allgrps;
	JSObject*	allsubs;
	JSObject*	grpobj_proto;
	JSObject*	grpobj;
	JSObject*	subobj_proto;
	JSObject*	subobj;
	JSObject*	grp_list;
	JSObject*	sub_list;
	JSString*	js_str;
	jsval		val;
	jsuint		grp_index;
	jsuint		sub_index;
	uint		l,d;

	/* Return existing object if it's already been created */
	if(JS_GetProperty(cx,parent,"msg_area",&val) && val!=JSVAL_VOID)
		areaobj = JSVAL_TO_OBJECT(val);
	else
		areaobj = JS_DefineObject(cx, parent, "msg_area", NULL
									, NULL, JSPROP_ENUMERATE|JSPROP_READONLY);

	if(areaobj==NULL)
		return(NULL);

#ifdef BUILD_JSDOCS
	js_DescribeSyncObject(cx,areaobj,"Message Areas",310);
#endif

	/* msg_area.properties */
	if(!JS_NewNumberValue(cx,cfg->msg_misc,&val))
		return(NULL);
	if(!JS_SetProperty(cx, areaobj, "settings", &val)) 
		return(NULL);

	/* msg_area.grp[] */
	if((allgrps=JS_NewObject(cx, NULL, NULL, areaobj))==NULL)
		return(NULL);

	val=OBJECT_TO_JSVAL(allgrps);
	if(!JS_SetProperty(cx, areaobj, "grp", &val))
		return(NULL);

	/* msg_area.sub[] */
	if((allsubs=JS_NewObject(cx, NULL, NULL, areaobj))==NULL)
		return(NULL);

	val=OBJECT_TO_JSVAL(allsubs);
	if(!JS_SetProperty(cx, areaobj, "sub", &val))
		return(NULL);

	/* msg_area.grp_list[] */
	if((grp_list=JS_NewArrayObject(cx, 0, NULL))==NULL) 
		return(NULL);

	val=OBJECT_TO_JSVAL(grp_list);
	if(!JS_SetProperty(cx, areaobj, "grp_list", &val)) 
		return(NULL);

	if((grpobj_proto=JS_NewObject(cx, NULL, NULL, areaobj))==NULL)
		return(NULL);
	if((subobj_proto=JS_NewObject(cx, NULL, NULL, areaobj))==NULL)
		return(NULL);
	for(l=0;l<cfg->total_grps;l++) {

		if((grpobj=JS_NewObject(cx, NULL, grpobj_proto, NULL))==NULL)
			return(NULL);

		val=OBJECT_TO_JSVAL(grpobj);
		grp_index=-1;
		if(user==NULL || chk_ar(cfg,cfg->grp[l]->ar,user,client)) {

			if(!JS_GetArrayLength(cx, grp_list, &grp_index))
				return(NULL);

			if(!JS_SetElement(cx, grp_list, grp_index, &val))
				return(NULL);
		}

		/* Add as property (associative array element) */
		if(!JS_DefineProperty(cx, allgrps, cfg->grp[l]->sname, val
			,NULL,NULL,JSPROP_READONLY|JSPROP_ENUMERATE))
			return(NULL);

		val=INT_TO_JSVAL(grp_index);
		if(!JS_SetProperty(cx, grpobj, "index", &val))
			return(NULL);

		val=INT_TO_JSVAL(l);
		if(!JS_SetProperty(cx, grpobj, "number", &val))
			return(NULL);

		if((js_str=JS_NewStringCopyZ(cx, cfg->grp[l]->sname))==NULL)
			return(NULL);
		val=STRING_TO_JSVAL(js_str);
		if(!JS_SetProperty(cx, grpobj, "name", &val))
			return(NULL);

		if((js_str=JS_NewStringCopyZ(cx, cfg->grp[l]->lname))==NULL)
			return(NULL);
		val=STRING_TO_JSVAL(js_str);
		if(!JS_SetProperty(cx, grpobj, "description", &val))
			return(NULL);

		if((js_str=JS_NewStringCopyZ(cx, cfg->grp[l]->arstr))==NULL)
			return(NULL);
		if(!JS_DefineProperty(cx, grpobj, "ars", STRING_TO_JSVAL(js_str)
			,NULL,NULL,JSPROP_ENUMERATE|JSPROP_READONLY))
			return(NULL);

#ifdef BUILD_JSDOCS
		js_DescribeSyncObject(cx,grpobj,"Message Groups (current user has access to)",310);
#endif

		/* sub_list[] */
		if((sub_list=JS_NewArrayObject(cx, 0, NULL))==NULL) 
			return(NULL);

		val=OBJECT_TO_JSVAL(sub_list);
		if(!JS_SetProperty(cx, grpobj, "sub_list", &val)) 
			return(NULL);

		for(d=0;d<cfg->total_subs;d++) {
			if(cfg->sub[d]->grp!=l)
				continue;

			if((subobj=JS_NewObject(cx, &js_sub_class, subobj_proto, NULL))==NULL)
				return(NULL);
/** Crash ^^^ Here in JSexec/ircd upon recycle/reload of script:

 	mozjs185-1.0.dll!62e4a968() 	
 	[Frames below may be incorrect and/or missing, no symbols loaded for mozjs185-1.0.dll]	
 	mozjs185-1.0.dll!62eda4b2() 	
 	mozjs185-1.0.dll!62e9cd4e() 	
 	mozjs185-1.0.dll!62ea3cf0() 	
 	mozjs185-1.0.dll!62e4e39e() 	
 	mozjs185-1.0.dll!62edd884() 	
 	mozjs185-1.0.dll!62e8010f() 	
 	mozjs185-1.0.dll!62e5b0c9() 	
 	mozjs185-1.0.dll!62e4b1ee() 	
>	sbbs.dll!js_CreateMsgAreaObject(JSContext * cx=0x07b33ce8, JSObject * parent=0x0a37f028, scfg_t * cfg=0x004a2b20, user_t * user=0x00000000, client_t * client=0x00000000, subscan_t * subscan=0x00000000)  Line 459 + 0x17 bytes	C
 	sbbs.dll!js_CreateUserObjects(JSContext * cx=0x07b33ce8, JSObject * parent=0x0a37f028, scfg_t * cfg=0x004a2b20, user_t * user=0x00000000, client_t * client=0x00000000, char * html_index_file=0x00000000, subscan_t * subscan=0x00000000)  Line 1431 + 0x1d bytes	C
 	sbbs.dll!js_CreateCommonObjects(JSContext * js_cx=0x07b33ce8, scfg_t * cfg=0x004a2b20, scfg_t * node_cfg=0x004a2b20, jsSyncMethodSpec * methods=0x00000000, __int64 uptime=0, char * host_name=0x101bdaa6, char * socklib_desc=0x101bdaa6, js_branch_t * branch=0x019c8f30, js_startup_t * js_startup=0x0012f7cc, client_t * client=0x00000000, unsigned int client_socket=4294967295, js_server_props_t * props=0x00000000)  Line 3858 + 0x1b bytes	C
 	sbbs.dll!js_load(JSContext * cx=0x02e36300, unsigned int argc=3, unsigned __int64 * arglist=0x01d700d0)  Line 282 + 0x44 bytes	C
 	mozjs185-1.0.dll!62e91dfd() 	
 	jsexec.exe!__lock_fhandle(int fh=1240564)  Line 467	C
 	7ffdf000()	
 	ffff0007()	
 	mozjs185-1.0.dll!62fe9c60() 	

*/

			if(subscan!=NULL)
				JS_SetPrivate(cx,subobj,&subscan[d]);

			val=OBJECT_TO_JSVAL(subobj);
			sub_index=-1;
			if(user==NULL || can_user_access_sub(cfg,d,user,client)) {

				if(!JS_GetArrayLength(cx, sub_list, &sub_index))
					return(NULL);							

				if(!JS_SetElement(cx, sub_list, sub_index, &val))
					return(NULL);
			}

			/* Add as property (associative array element) */
			if(!JS_DefineProperty(cx, allsubs, cfg->sub[d]->code, val
				,NULL,NULL,JSPROP_READONLY|JSPROP_ENUMERATE))
				return(NULL);

			val=INT_TO_JSVAL(sub_index);
			if(!JS_SetProperty(cx, subobj, "index", &val))
				return(NULL);

			val=INT_TO_JSVAL(grp_index);
			if(!JS_SetProperty(cx, subobj, "grp_index", &val))
				return(NULL);

			if(!js_CreateMsgAreaProperties(cx, cfg, subobj, d))
				return(NULL);
		
			if(user==NULL)
				val=BOOLEAN_TO_JSVAL(JS_TRUE);
			else
				val=BOOLEAN_TO_JSVAL(can_user_read_sub(cfg,d,user,client));
			if(!JS_SetProperty(cx, subobj, "can_read", &val))
				return(NULL);

			if(user==NULL)
				val=BOOLEAN_TO_JSVAL(JS_TRUE);
			else
				val=BOOLEAN_TO_JSVAL(can_user_post(cfg,d,user,client,/* reason: */NULL));
			if(!JS_SetProperty(cx, subobj, "can_post", &val))
				return(NULL);

			if(user==NULL)
				val=BOOLEAN_TO_JSVAL(JS_TRUE);
			else
				val=BOOLEAN_TO_JSVAL(is_user_subop(cfg,d,user,client));
			if(!JS_SetProperty(cx, subobj, "is_operator", &val))
				return(NULL);

			if(cfg->sub[d]->mod_ar[0]!=0 && user!=NULL 
				&& chk_ar(cfg,cfg->sub[d]->mod_ar,user,client))
				val=BOOLEAN_TO_JSVAL(JS_TRUE);
			else
				val=BOOLEAN_TO_JSVAL(JS_FALSE);
			if(!JS_SetProperty(cx, subobj, "is_moderated", &val))
				return(NULL);

			if(!JS_DefineProperties(cx, subobj, js_sub_properties))
				return(NULL);

#ifdef BUILD_JSDOCS
			js_DescribeSyncObject(cx,subobj,"Message Sub-boards (current user has access to)</h2>"
				"(all properties are <small>READ ONLY</small> except for "
				"<i>scan_ptr</i>, <i>scan_cfg</i>, and <i>last_read</i>)"
				,310);
#endif

		}

#ifdef BUILD_JSDOCS
		js_CreateArrayOfStrings(cx, grpobj, "_property_desc_list", msg_grp_prop_desc, JSPROP_READONLY);
#endif
	}

#ifdef BUILD_JSDOCS
	js_CreateArrayOfStrings(cx, areaobj, "_property_desc_list", msg_area_prop_desc, JSPROP_READONLY);

	js_DescribeSyncObject(cx,allgrps,"Associative array of all groups (use name as index)",312);
	JS_DefineProperty(cx,allgrps,"_dont_document",JSVAL_TRUE,NULL,NULL,JSPROP_READONLY);
#endif

#ifdef BUILD_JSDOCS
	js_DescribeSyncObject(cx,allsubs,"Associative array of all sub-boards (use internal code as index)",311);
	JS_DefineProperty(cx,allsubs,"_dont_document",JSVAL_TRUE,NULL,NULL,JSPROP_READONLY);
#endif

	return(areaobj);
}

#endif	/* JAVSCRIPT */
