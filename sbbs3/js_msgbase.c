/* js_msgbase.c */

/* Synchronet JavaScript "MsgBase" Object */

/* $Id$ */

/****************************************************************************
 * @format.tab-size 4		(Plain Text/Source Code File Header)			*
 * @format.use-tabs true	(see http://www.synchro.net/ptsc_hdr.html)		*
 *																			*
 * Copyright 2001 Rob Swindell - http://www.synchro.net/copyright.html		*
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

static scfg_t* scfg=NULL;

typedef struct
{
	smb_t	smb;
	BOOL	debug;

} private_t;



/* MsgBase Constructor (open message base) */

static JSBool
js_msgbase_constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	char*		code;
	JSString*	js_str;
	private_t*	p;

	*rval = JSVAL_VOID;

	if((p=(private_t*)malloc(sizeof(private_t)))==NULL) 
		return(JS_FALSE);

	memset(&(p->smb),0,sizeof(smb_t));
	js_str = JS_ValueToString(cx, argv[0]);
	code = JS_GetStringBytes(js_str);
	if(stricmp(code,"mail")==0) {
		p->smb.subnum=INVALID_SUB;
		snprintf(p->smb.file,sizeof(p->smb.file),"%s%s",scfg->data_dir,code);
	} else {
		for(p->smb.subnum=0;p->smb.subnum<scfg->total_subs;p->smb.subnum++)
			if(!stricmp(scfg->sub[p->smb.subnum]->code,code))
				break;
		if(p->smb.subnum>=scfg->total_subs)	/* unknown code */
			return(JS_TRUE);
		snprintf(p->smb.file,sizeof(p->smb.file),"%s%s"
			,scfg->sub[p->smb.subnum]->data_dir,code);
	}

	if(argc>1)
		p->smb.retry_time=JSVAL_TO_INT(argv[0]);
	else
		p->smb.retry_time=scfg->smb_retry_time;

	if(smb_open(&(p->smb))!=0) 
		return(JS_FALSE);

	p->debug=JS_FALSE;

	if(!JS_SetPrivate(cx, obj, p)) 
		return(JS_FALSE);

	return(JS_TRUE);
}

/* Destructor */

static void js_finalize_msgbase(JSContext *cx, JSObject *obj)
{
	private_t* p;
	
	if((p=(private_t*)JS_GetPrivate(cx,obj))==NULL)
		return;

	if(SMB_IS_OPEN(&(p->smb)))
		smb_close(&(p->smb));

	free(p);

	JS_SetPrivate(cx, obj, NULL);
}

/* Methods */

static JSBool
js_close(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	private_t* p;
	
	if((p=(private_t*)JS_GetPrivate(cx,obj))==NULL)
		return(JS_FALSE);

	smb_close(&(p->smb));

	*rval = JSVAL_VOID;
	return(JS_TRUE);
}

static JSClass js_msghdr_class = {
     "MsgHeader"				/* name			*/
    ,JSCLASS_HAS_PRIVATE	/* flags		*/
	,JS_PropertyStub		/* addProperty	*/
	,JS_PropertyStub		/* delProperty	*/
	,JS_PropertyStub		/* getProperty	*/
	,JS_PropertyStub		/* setProperty	*/
	,JS_EnumerateStub		/* enumerate	*/
	,JS_ResolveStub			/* resolve		*/
	,JS_ConvertStub			/* convert		*/
	,js_finalize_msgbase	/* finalize		*/
};

static BOOL parse_header_object(JSContext* cx, JSObject* hdr, uint subnum, smbmsg_t* msg)
{
	char*		cp;
	ushort		nettype;
	JSString*	js_str;
	jsval		val;

	/* Required Header Fields */
	if(JS_GetProperty(cx, hdr, "subject", &val) && val!=JSVAL_VOID) {
		if((js_str=JS_ValueToString(cx,val))==NULL)
			return(FALSE);
		if((cp=JS_GetStringBytes(js_str))==NULL)
			return(FALSE);
	} else
		cp="";
	smb_hfield(msg, SUBJECT, (ushort)strlen(cp), cp);
	msg->idx.subj=crc16(cp);

	if(JS_GetProperty(cx, hdr, "to", &val) && val!=JSVAL_VOID) {
		if((js_str=JS_ValueToString(cx,val))==NULL)
			return(FALSE);
		if((cp=JS_GetStringBytes(js_str))==NULL)
			return(FALSE);
	} else {
		if(subnum==INVALID_SUB)	/* e-mail */
			return(FALSE);		/* "to" property required */
		cp="All";
	}
	smb_hfield(msg, RECIPIENT, (ushort)strlen(cp), cp);
	if(subnum!=INVALID_SUB)
		msg->idx.to=crc16(cp);

	if(JS_GetProperty(cx, hdr, "from", &val) && val!=JSVAL_VOID) {
		if((js_str=JS_ValueToString(cx,val))==NULL)
			return(FALSE);
		if((cp=JS_GetStringBytes(js_str))==NULL)
			return(FALSE);
	} else
		return(FALSE);	/* "from" property required */
	smb_hfield(msg, SENDER, (ushort)strlen(cp), cp);
	if(subnum!=INVALID_SUB)
		msg->idx.from=crc16(cp);

	/* Optional Header Fields */
	if(JS_GetProperty(cx, hdr, "from_ext", &val) && val!=JSVAL_VOID) {
		if((js_str=JS_ValueToString(cx,val))==NULL)
			return(FALSE);
		if((cp=JS_GetStringBytes(js_str))==NULL)
			return(FALSE);
		smb_hfield(msg, SENDEREXT, (ushort)strlen(cp), cp);
		if(subnum==INVALID_SUB)
			msg->idx.from=atoi(cp);
	}

	if(JS_GetProperty(cx, hdr, "from_net_type", &val) && val!=JSVAL_VOID) {
		nettype=(ushort)JSVAL_TO_INT(val);
		smb_hfield(msg, SENDERNETTYPE, sizeof(nettype), &nettype);
		if(subnum==INVALID_SUB && nettype!=NET_NONE)
			msg->idx.from=0;
	}

	if(JS_GetProperty(cx, hdr, "from_net_addr", &val) && val!=JSVAL_VOID) {
		if((js_str=JS_ValueToString(cx,val))==NULL)
			return(FALSE);
		if((cp=JS_GetStringBytes(js_str))==NULL)
			return(FALSE);
		smb_hfield(msg, SENDERNETADDR, (ushort)strlen(cp), cp);
	}

	if(JS_GetProperty(cx, hdr, "to_ext", &val) && val!=JSVAL_VOID) {
		if((js_str=JS_ValueToString(cx,val))==NULL)
			return(FALSE);
		if((cp=JS_GetStringBytes(js_str))==NULL)
			return(FALSE);
		smb_hfield(msg, RECIPIENTEXT, (ushort)strlen(cp), cp);
		if(subnum==INVALID_SUB)
			msg->idx.to=atoi(cp);
	}

	if(JS_GetProperty(cx, hdr, "to_net_type", &val) && val!=JSVAL_VOID) {
		nettype=(ushort)JSVAL_TO_INT(val);
		smb_hfield(msg, RECIPIENTNETTYPE, sizeof(nettype), &nettype);
		if(subnum==INVALID_SUB && nettype!=NET_NONE)
			msg->idx.to=0;
	}

	if(JS_GetProperty(cx, hdr, "to_net_addr", &val) && val!=JSVAL_VOID) {
		if((js_str=JS_ValueToString(cx,val))==NULL)
			return(FALSE);
		if((cp=JS_GetStringBytes(js_str))==NULL)
			return(FALSE);
		smb_hfield(msg, RECIPIENTNETADDR, (ushort)strlen(cp), cp);
	}

	if(JS_GetProperty(cx, hdr, "id", &val) && val!=JSVAL_VOID) {
		if((js_str=JS_ValueToString(cx,val))==NULL)
			return(FALSE);
		if((cp=JS_GetStringBytes(js_str))==NULL)
			return(FALSE);
		smb_hfield(msg, RFC822MSGID, (ushort)strlen(cp), cp);
	}

	if(JS_GetProperty(cx, hdr, "reply_id", &val) && val!=JSVAL_VOID) {
		if((js_str=JS_ValueToString(cx,val))==NULL)
			return(FALSE);
		if((cp=JS_GetStringBytes(js_str))==NULL)
			return(FALSE);
		smb_hfield(msg, RFC822REPLYID, (ushort)strlen(cp), cp);
	}

	if(JS_GetProperty(cx, hdr, "date", &val) && val!=JSVAL_VOID) {
		if((js_str=JS_ValueToString(cx,val))==NULL)
			return(FALSE);
		if((cp=JS_GetStringBytes(js_str))==NULL)
			return(FALSE);
		msg->hdr.when_written=rfc822date(cp);
	}
	
	/* Numeric Header Fields */
	if(JS_GetProperty(cx, hdr, "attr", &val) && val!=JSVAL_VOID) 
		msg->hdr.attr=(ushort)JSVAL_TO_INT(val);
	if(JS_GetProperty(cx, hdr, "auxattr", &val) && val!=JSVAL_VOID) 
		msg->hdr.auxattr=JSVAL_TO_INT(val);
	if(JS_GetProperty(cx, hdr, "netattr", &val) && val!=JSVAL_VOID) 
		msg->hdr.netattr=JSVAL_TO_INT(val);
	if(JS_GetProperty(cx, hdr, "when_written_time", &val) && val!=JSVAL_VOID) 
		msg->hdr.when_written.time=JSVAL_TO_INT(val);
	if(JS_GetProperty(cx, hdr, "when_written_zone", &val) && val!=JSVAL_VOID) 
		msg->hdr.when_written.zone=(short)JSVAL_TO_INT(val);
	if(JS_GetProperty(cx, hdr, "when_imported_time", &val) && val!=JSVAL_VOID) 
		msg->hdr.when_imported.time=JSVAL_TO_INT(val);
	if(JS_GetProperty(cx, hdr, "when_imported_zone", &val) && val!=JSVAL_VOID) 
		msg->hdr.when_imported.zone=(short)JSVAL_TO_INT(val);

	return(TRUE);
}

static JSBool
js_get_msg_header(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	char		date[128];
	char		msg_id[128];
	char		reply_id[128];
	char*		val;
	ulong		l;
	smbmsg_t	msg;
	JSObject*	hdrobj;
	private_t*	p;

	*rval = JSVAL_NULL;
	
	if((p=(private_t*)JS_GetPrivate(cx,obj))==NULL)
		return(JS_FALSE);

	if(!SMB_IS_OPEN(&(p->smb)))
		return(JS_TRUE);

	memset(&msg,0,sizeof(msg));

	if(JSVAL_TO_BOOLEAN(argv[0])==JS_TRUE)	/* Get by offset */
		msg.offset=JSVAL_TO_INT(argv[1]);
	else									/* Get by number */
		msg.hdr.number=JSVAL_TO_INT(argv[1]);

	if(smb_getmsgidx(&(p->smb), &msg)!=0)
		return(JS_TRUE);

	if(smb_lockmsghdr(&(p->smb),&msg)!=0)
		return(JS_TRUE);

	if(smb_getmsghdr(&(p->smb), &msg)!=0) {
		smb_unlockmsghdr(&(p->smb),&msg); 
		return(JS_TRUE);
	}

	smb_unlockmsghdr(&(p->smb),&msg); 

	if((hdrobj=JS_NewObject(cx,&js_msghdr_class,NULL,obj))==NULL) {
		smb_freemsgmem(&msg);
		return(JS_TRUE);
	}

	JS_DefineProperty(cx, hdrobj, "number", INT_TO_JSVAL(msg.hdr.number)
		,NULL,NULL,JSPROP_READONLY|JSPROP_ENUMERATE);
	JS_DefineProperty(cx, hdrobj, "to",STRING_TO_JSVAL(JS_NewStringCopyZ(cx,msg.to))
		,NULL,NULL,JSPROP_ENUMERATE);
	JS_DefineProperty(cx, hdrobj, "from",STRING_TO_JSVAL(JS_NewStringCopyZ(cx,msg.from))
		,NULL,NULL,JSPROP_ENUMERATE);
	JS_DefineProperty(cx, hdrobj, "subject",STRING_TO_JSVAL(JS_NewStringCopyZ(cx,msg.subj))
		,NULL,NULL,JSPROP_ENUMERATE);
	JS_DefineProperty(cx, hdrobj, "to_ext",STRING_TO_JSVAL(JS_NewStringCopyZ(cx,msg.to_ext))
		,NULL,NULL,JSPROP_ENUMERATE);
	JS_DefineProperty(cx, hdrobj, "from_ext",STRING_TO_JSVAL(JS_NewStringCopyZ(cx,msg.from_ext))
		,NULL,NULL,JSPROP_ENUMERATE);
	JS_DefineProperty(cx, hdrobj, "replyto",STRING_TO_JSVAL(JS_NewStringCopyZ(cx,msg.replyto))
		,NULL,NULL,JSPROP_ENUMERATE);
	JS_DefineProperty(cx, hdrobj, "replyto_ext",STRING_TO_JSVAL(JS_NewStringCopyZ(cx,msg.replyto_ext))
		,NULL,NULL,JSPROP_ENUMERATE);
	JS_DefineProperty(cx, hdrobj, "to_agent",INT_TO_JSVAL(msg.to_agent)
		,NULL,NULL,JSPROP_ENUMERATE);
	JS_DefineProperty(cx, hdrobj, "from_agent",INT_TO_JSVAL(msg.from_agent)
		,NULL,NULL,JSPROP_ENUMERATE);
	JS_DefineProperty(cx, hdrobj, "replyto_agent",INT_TO_JSVAL(msg.replyto_agent)
		,NULL,NULL,JSPROP_ENUMERATE);

	JS_DefineProperty(cx, hdrobj, "to_net_type",INT_TO_JSVAL(msg.to_net.type)
		,NULL,NULL,JSPROP_ENUMERATE);
	JS_DefineProperty(cx, hdrobj, "to_net_addr"
		,STRING_TO_JSVAL(JS_NewStringCopyZ(cx,net_addr(&msg.to_net)))
		,NULL,NULL,JSPROP_ENUMERATE);

	JS_DefineProperty(cx, hdrobj, "from_net_type",INT_TO_JSVAL(msg.from_net.type)
		,NULL,NULL,JSPROP_ENUMERATE);
	JS_DefineProperty(cx, hdrobj, "from_net_addr"
		,STRING_TO_JSVAL(JS_NewStringCopyZ(cx,net_addr(&msg.from_net)))
		,NULL,NULL,JSPROP_ENUMERATE);

	JS_DefineProperty(cx, hdrobj, "replyto_net_type",INT_TO_JSVAL(msg.replyto_net.type)
		,NULL,NULL,JSPROP_ENUMERATE);
	JS_DefineProperty(cx, hdrobj, "replyto_net_addr"
		,STRING_TO_JSVAL(JS_NewStringCopyZ(cx,net_addr(&msg.replyto_net)))
		,NULL,NULL,JSPROP_ENUMERATE);

	JS_DefineProperty(cx, hdrobj, "forwarded",INT_TO_JSVAL(msg.forwarded)
		,NULL,NULL,JSPROP_ENUMERATE);
	JS_DefineProperty(cx, hdrobj, "expiration_time",INT_TO_JSVAL(msg.expiration.time)
		,NULL,NULL,JSPROP_ENUMERATE);
	JS_DefineProperty(cx, hdrobj, "expiration_zone",INT_TO_JSVAL(msg.expiration.zone)
		,NULL,NULL,JSPROP_ENUMERATE);

	JS_DefineProperty(cx, hdrobj, "type", INT_TO_JSVAL(msg.hdr.type)
		,NULL,NULL,JSPROP_ENUMERATE);
	JS_DefineProperty(cx, hdrobj, "version", INT_TO_JSVAL(msg.hdr.version)
		,NULL,NULL,JSPROP_ENUMERATE);
	JS_DefineProperty(cx, hdrobj, "attr", INT_TO_JSVAL(msg.hdr.attr)
		,NULL,NULL,JSPROP_ENUMERATE);
	JS_DefineProperty(cx, hdrobj, "auxattr", INT_TO_JSVAL(msg.hdr.auxattr)
		,NULL,NULL,JSPROP_ENUMERATE);
	JS_DefineProperty(cx, hdrobj, "netattr", INT_TO_JSVAL(msg.hdr.netattr)
		,NULL,NULL,JSPROP_ENUMERATE);

	JS_DefineProperty(cx, hdrobj, "when_written_time", INT_TO_JSVAL(msg.hdr.when_written.time)
		,NULL,NULL,JSPROP_ENUMERATE);
	JS_DefineProperty(cx, hdrobj, "when_written_zone", INT_TO_JSVAL(msg.hdr.when_written.zone)
		,NULL,NULL,JSPROP_ENUMERATE);
	JS_DefineProperty(cx, hdrobj, "when_imported_time", INT_TO_JSVAL(msg.hdr.when_imported.time)
		,NULL,NULL,JSPROP_ENUMERATE);
	JS_DefineProperty(cx, hdrobj, "when_imported_zone", INT_TO_JSVAL(msg.hdr.when_imported.zone)
		,NULL,NULL,JSPROP_ENUMERATE);

	JS_DefineProperty(cx, hdrobj, "thread_orig", INT_TO_JSVAL(msg.hdr.thread_orig)
		,NULL,NULL,JSPROP_ENUMERATE);
	JS_DefineProperty(cx, hdrobj, "thread_next", INT_TO_JSVAL(msg.hdr.thread_next)
		,NULL,NULL,JSPROP_ENUMERATE);
	JS_DefineProperty(cx, hdrobj, "thread_first", INT_TO_JSVAL(msg.hdr.thread_first)
		,NULL,NULL,JSPROP_ENUMERATE);

	JS_DefineProperty(cx, hdrobj, "delivery_attempts", INT_TO_JSVAL(msg.hdr.delivery_attempts)
		,NULL,NULL,JSPROP_ENUMERATE);

	l=smb_getmsgdatlen(&msg);
	JS_DefineProperty(cx, hdrobj, "data_length", INT_TO_JSVAL(l)
		,NULL,NULL,JSPROP_ENUMERATE);

	JS_DefineProperty(cx, hdrobj, "date"
		,STRING_TO_JSVAL(JS_NewStringCopyZ(cx,msgdate(msg.hdr.when_written,date)))
		,NULL,NULL,JSPROP_ENUMERATE);

	/* Reply-ID (References) */
	if(msg.reply_id!=NULL)
		val=msg.reply_id;
	else {
		reply_id[0]=0;
		if(p->smb.subnum!=INVALID_SUB && msg.hdr.thread_orig)
			sprintf(reply_id,"<%lu.%s@%s>"
				,msg.hdr.thread_orig,scfg->sub[p->smb.subnum]->code,scfg->sys_inetaddr);
		val=reply_id;
	}
	JS_DefineProperty(cx, hdrobj, "reply_id", STRING_TO_JSVAL(JS_NewStringCopyZ(cx,val))
		,NULL,NULL,JSPROP_READONLY|JSPROP_ENUMERATE);

	/* Message-ID */
	if(msg.id!=NULL)
		val=msg.id;
	else {
		if(p->smb.subnum==INVALID_SUB)
			sprintf(msg_id,"<%08lX.%lu@%s>"
				,msg.hdr.when_written.time
				,msg.idx.number,scfg->sys_inetaddr);
		else
			sprintf(msg_id,"<%08lX.%lu.%s@%s>"
				,msg.hdr.when_written.time
				,msg.idx.number,scfg->sub[p->smb.subnum]->code,scfg->sys_inetaddr);
		val=msg_id;
	}
	JS_DefineProperty(cx, hdrobj, "id", STRING_TO_JSVAL(JS_NewStringCopyZ(cx,val))
		,NULL,NULL,JSPROP_READONLY|JSPROP_ENUMERATE);

	smb_freemsgmem(&msg);

	*rval = OBJECT_TO_JSVAL(hdrobj);

	return(JS_TRUE);
}

static JSBool
js_put_msg_header(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	smbmsg_t	msg;
	JSObject*	hdr;
	private_t*	p;

	*rval = BOOLEAN_TO_JSVAL(JS_FALSE);

	if((p=(private_t*)JS_GetPrivate(cx,obj))==NULL)
		return(JS_FALSE);

	if(!JSVAL_IS_OBJECT(argv[2]))
		return(JS_TRUE);
	hdr = JSVAL_TO_OBJECT(argv[2]);

	if(!SMB_IS_OPEN(&(p->smb)))
		return(JS_TRUE);

	memset(&msg,0,sizeof(msg));

	if(JSVAL_TO_BOOLEAN(argv[0])==JS_TRUE)	/* Get by offset */
		msg.offset=JSVAL_TO_INT(argv[1]);
	else									/* Get by number */
		msg.hdr.number=JSVAL_TO_INT(argv[1]);

	if(smb_getmsgidx(&(p->smb), &msg)!=0)
		return(JS_TRUE);

	if(smb_lockmsghdr(&(p->smb),&msg)!=0)
		return(JS_TRUE);

	do {
		if(smb_getmsghdr(&(p->smb), &msg)!=0)
			break;

		if(!parse_header_object(cx, hdr, p->smb.subnum, &msg))
			break;

		if(smb_putmsghdr(&(p->smb), &msg)!=0)
			break;

		*rval = BOOLEAN_TO_JSVAL(JS_TRUE);
	} while(0);

	smb_unlockmsghdr(&(p->smb),&msg); 
	smb_freemsgmem(&msg);

	return(JS_TRUE);
}

static char* get_msg_text(smb_t* smb, smbmsg_t* msg, BOOL strip_ctrl_a, BOOL rfc822, ulong mode)
{
	char*		buf;

	if(smb_getmsgidx(smb, msg)!=0)
		return(NULL);

	if(smb_lockmsghdr(smb,msg)!=0)
		return(NULL);

	if(smb_getmsghdr(smb, msg)!=0) {
		smb_unlockmsghdr(smb, msg); 
		return(NULL);
	}

	if((buf=smb_getmsgtxt(smb, msg, mode))==NULL) {
		smb_unlockmsghdr(smb,msg); 
		smb_freemsgmem(msg);
		return(NULL);
	}

	smb_unlockmsghdr(smb, msg); 
	smb_freemsgmem(msg);

	if(strip_ctrl_a) {
		char* newbuf;
		if((newbuf=malloc(strlen(buf)+1))!=NULL) {
			int i,j;
			for(i=j=0;buf[i];i++) {
				if(buf[i]==CTRL_A && buf[i+1]!=0)
					i++;
				else newbuf[j++]=buf[i]; 
			}
			newbuf[j]=0;
			strcpy(buf,newbuf);
			free(newbuf);
		}
	}

	if(rfc822) {	/* must escape lines starting with dot ('.') */
		char* newbuf;
		if((newbuf=malloc((strlen(buf)*2)+1))!=NULL) {
			int i,j;
			for(i=j=0;buf[i];i++) {
				if((i==0 || buf[i-1]=='\n') && buf[i]=='.')
					newbuf[j++]='.';
				newbuf[j++]=buf[i]; 
			}
			newbuf[j]=0;
			free(buf);
			buf = newbuf;
		}
	}

	return(buf);
}

static JSBool
js_get_msg_body(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	char*		buf;
	smbmsg_t	msg;
	JSBool		strip_ctrl_a=JS_FALSE;
	JSBool		tails=JS_TRUE;
	JSBool		rfc822=JS_FALSE;
	private_t*	p;

	*rval = JSVAL_NULL;
	
	if((p=(private_t*)JS_GetPrivate(cx,obj))==NULL)
		return(JS_FALSE);

	if(!SMB_IS_OPEN(&(p->smb)))
		return(JS_TRUE);

	memset(&msg,0,sizeof(msg));

	if(JSVAL_TO_BOOLEAN(argv[0])==JS_TRUE)	/* Get by offset */
		msg.offset=JSVAL_TO_INT(argv[1]);
	else									/* Get by number */
		msg.hdr.number=JSVAL_TO_INT(argv[1]);

	if(argc>2)
		strip_ctrl_a=JSVAL_TO_BOOLEAN(argv[2]);

	if(argc>3)
		rfc822=JSVAL_TO_BOOLEAN(argv[3]);

	if(argc>4)
		tails=JSVAL_TO_BOOLEAN(argv[4]);

	buf = get_msg_text(&(p->smb), &msg, strip_ctrl_a, rfc822, tails ? GETMSGTXT_TAILS : 0);
	if(buf==NULL)
		return(JS_TRUE);

	*rval = STRING_TO_JSVAL(JS_NewStringCopyZ(cx,buf));

	smb_freemsgtxt(buf);

	return(JS_TRUE);
}

static JSBool
js_get_msg_tail(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	char*		buf;
	smbmsg_t	msg;
	JSBool		strip_ctrl_a=JS_FALSE;
	JSBool		rfc822=JS_FALSE;
	private_t*	p;

	*rval = JSVAL_NULL;
	
	if((p=(private_t*)JS_GetPrivate(cx,obj))==NULL)
		return(JS_FALSE);

	if(!SMB_IS_OPEN(&(p->smb)))
		return(JS_TRUE);

	memset(&msg,0,sizeof(msg));

	if(JSVAL_TO_BOOLEAN(argv[0])==JS_TRUE)	/* Get by offset */
		msg.offset=JSVAL_TO_INT(argv[1]);
	else									/* Get by number */
		msg.hdr.number=JSVAL_TO_INT(argv[1]);

	if(argc>2)
		strip_ctrl_a=JSVAL_TO_BOOLEAN(argv[2]);

	if(argc>3)
		rfc822=JSVAL_TO_BOOLEAN(argv[3]);

	buf = get_msg_text(&(p->smb), &msg, strip_ctrl_a, rfc822, GETMSGTXT_TAILS|GETMSGTXT_NO_BODY);
	if(buf==NULL)
		return(JS_TRUE);

	*rval = STRING_TO_JSVAL(JS_NewStringCopyZ(cx,buf));

	smb_freemsgtxt(buf);

	return(JS_TRUE);
}

static JSBool
js_save_msg(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	char*		body;
	JSString*	js_str;
	JSObject*	hdr;
	smbmsg_t	msg;
	private_t*	p;

	*rval = BOOLEAN_TO_JSVAL(JS_FALSE);

	if(argc<2)
		return(JS_TRUE);
	
	if((p=(private_t*)JS_GetPrivate(cx,obj))==NULL)
		return(JS_FALSE);

	memset(&msg,0,sizeof(msg));

	if(!JSVAL_IS_OBJECT(argv[0]))
		return(JS_TRUE);
	hdr = JSVAL_TO_OBJECT(argv[0]);

	if(!JSVAL_IS_STRING(argv[1]))
		return(JS_TRUE);
	if((js_str=JS_ValueToString(cx,argv[1]))==NULL)
		return(JS_FALSE);
	if((body=JS_GetStringBytes(js_str))==NULL)
		return(JS_FALSE);

	if(!parse_header_object(cx, hdr, p->smb.subnum, &msg))
		return(JS_TRUE);

	if(msg.hdr.when_written.time==0) {
		msg.hdr.when_written.time=time(NULL);
		msg.hdr.when_written.zone=scfg->sys_timezone;
	}

	truncsp(body);
	if(savemsg(scfg, &(p->smb), &msg, body)==0)
		*rval = BOOLEAN_TO_JSVAL(JS_TRUE);

	return(JS_TRUE);
}


/* MsgBase Object Properites */
enum {
	 SMB_PROP_LAST_ERROR
	,SMB_PROP_FILE		
	,SMB_PROP_DEBUG		
	,SMB_PROP_RETRY_TIME
	,SMB_PROP_FIRST_MSG		// first message number
	,SMB_PROP_LAST_MSG		// last message number
	,SMB_PROP_TOTAL_MSGS 	// total messages
	,SMB_PROP_MAX_CRCS		// Maximum number of CRCs to keep in history
    ,SMB_PROP_MAX_MSGS      // Maximum number of message to keep in sub
    ,SMB_PROP_MAX_AGE       // Maximum age of message to keep in sub (in days)
	,SMB_PROP_ATTR			// Attributes for this message base (SMB_HYPER,etc)
	,SMB_PROP_SUBNUM		// sub-board number
};

static JSBool js_msgbase_set(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
    jsint       tiny;
	private_t*	p;

	if((p=(private_t*)JS_GetPrivate(cx,obj))==NULL)
		return(JS_FALSE);

    tiny = JSVAL_TO_INT(id);

	switch(tiny) {
		case SMB_PROP_RETRY_TIME:
			p->smb.retry_time = JSVAL_TO_BOOLEAN(*vp);
			break;
		case SMB_PROP_DEBUG:
			p->debug = JSVAL_TO_BOOLEAN(*vp);
			break;
	}

	return(JS_TRUE);
}

static JSBool js_msgbase_get(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
    jsint       tiny;
	idxrec_t	idx;
	private_t*	p;

	if((p=(private_t*)JS_GetPrivate(cx,obj))==NULL)
		return(JS_FALSE);

    tiny = JSVAL_TO_INT(id);

	switch(tiny) {
		case SMB_PROP_FILE:
			*vp = STRING_TO_JSVAL(JS_NewStringCopyZ(cx, p->smb.file));
			break;
		case SMB_PROP_LAST_ERROR:
			*vp = STRING_TO_JSVAL(JS_NewStringCopyZ(cx, p->smb.last_error));
			break;
		case SMB_PROP_RETRY_TIME:
			*vp = INT_TO_JSVAL(p->smb.retry_time);
			break;
		case SMB_PROP_DEBUG:
			*vp = INT_TO_JSVAL(p->debug);
			break;
		case SMB_PROP_FIRST_MSG:
			memset(&idx,0,sizeof(idx));
			smb_getfirstidx(&(p->smb),&idx);
			*vp = INT_TO_JSVAL(idx.number);
			break;
		case SMB_PROP_LAST_MSG:
			smb_getstatus(&(p->smb));
			*vp = INT_TO_JSVAL(p->smb.status.last_msg);
			break;
		case SMB_PROP_TOTAL_MSGS:
			smb_getstatus(&(p->smb));
			*vp = INT_TO_JSVAL(p->smb.status.total_msgs);
			break;
		case SMB_PROP_MAX_CRCS:
			*vp = INT_TO_JSVAL(p->smb.status.max_crcs);
			break;
		case SMB_PROP_MAX_MSGS:
			*vp = INT_TO_JSVAL(p->smb.status.max_msgs);
			break;
		case SMB_PROP_MAX_AGE:
			*vp = INT_TO_JSVAL(p->smb.status.max_age);
			break;
		case SMB_PROP_ATTR:
			*vp = INT_TO_JSVAL(p->smb.status.attr);
			break;
		case SMB_PROP_SUBNUM:
			*vp = INT_TO_JSVAL(p->smb.subnum);
			break;

	}

	return(JS_TRUE);
}

#define SMB_PROP_FLAGS JSPROP_ENUMERATE|JSPROP_READONLY

static struct JSPropertySpec js_msgbase_properties[] = {
/*		 name				,tinyid					,flags,				getter,	setter	*/

	{	"last_error"		,SMB_PROP_LAST_ERROR	,SMB_PROP_FLAGS,	NULL,NULL},
	{	"file"				,SMB_PROP_FILE			,SMB_PROP_FLAGS,	NULL,NULL},
	{	"debug"				,SMB_PROP_DEBUG			,JSPROP_ENUMERATE,	NULL,NULL},
	{	"retry_time"		,SMB_PROP_RETRY_TIME	,JSPROP_ENUMERATE,	NULL,NULL},
	{	"first_msg"			,SMB_PROP_FIRST_MSG		,SMB_PROP_FLAGS,	NULL,NULL},
	{	"last_msg"			,SMB_PROP_LAST_MSG		,SMB_PROP_FLAGS,	NULL,NULL},
	{	"total_msgs"		,SMB_PROP_TOTAL_MSGS	,SMB_PROP_FLAGS,	NULL,NULL},
	{	"max_crcs"			,SMB_PROP_MAX_CRCS		,SMB_PROP_FLAGS,	NULL,NULL},
	{	"max_msgs"			,SMB_PROP_MAX_MSGS  	,SMB_PROP_FLAGS,	NULL,NULL},
	{	"max_age"			,SMB_PROP_MAX_AGE   	,SMB_PROP_FLAGS,	NULL,NULL},
	{	"attributes"		,SMB_PROP_ATTR			,SMB_PROP_FLAGS,	NULL,NULL},
	{	"subnum"			,SMB_PROP_SUBNUM		,SMB_PROP_FLAGS,	NULL,NULL},
	{0}
};

static JSClass js_msgbase_class = {
     "MsgBase"				/* name			*/
    ,JSCLASS_HAS_PRIVATE	/* flags		*/
	,JS_PropertyStub		/* addProperty	*/
	,JS_PropertyStub		/* delProperty	*/
	,js_msgbase_get			/* getProperty	*/
	,js_msgbase_set			/* setProperty	*/
	,JS_EnumerateStub		/* enumerate	*/
	,JS_ResolveStub			/* resolve		*/
	,JS_ConvertStub			/* convert		*/
	,js_finalize_msgbase	/* finalize		*/
};

static JSFunctionSpec js_msgbase_functions[] = {
	{"close",			js_close,			0},		/* close msgbase */
	{"get_msg_header",	js_get_msg_header,	2},		/* get_msg_header(by_offset, number) */
	{"put_msg_header",	js_put_msg_header,	2},		/* put_msg_header(by_offset, number, hdrObj) */
	{"get_msg_body",	js_get_msg_body,	2},		/* get_msg_body(by_offset, number, [strip_ctrl_a]) */
	{"get_msg_tail",	js_get_msg_tail,	2},		/* get_msg_body(by_offset, number, [strip_ctrl_a]) */
	{"save_msg",		js_save_msg,		2},		/* save_msg(code, hdr, body) */
	{0}
};


JSObject* DLLCALL js_CreateMsgBaseClass(JSContext* cx, JSObject* parent, scfg_t* cfg)
{
	JSObject*	obj;

	scfg = cfg;
	obj = JS_InitClass(cx, parent, NULL
		,&js_msgbase_class
		,js_msgbase_constructor
		,1	/* number of constructor args */
		,js_msgbase_properties
		,js_msgbase_functions
		,NULL,NULL);

	return(obj);
}


#endif	/* JAVSCRIPT */
