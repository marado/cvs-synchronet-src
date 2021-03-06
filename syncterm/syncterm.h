/* Copyright (C), 2007 by Stephen Hurd */

#ifndef _SYNCTERM_H_
#define _SYNCTERM_H_

#include <dirwrap.h>

#include "bbslist.h"

#if defined(_WIN32)
	#include <malloc.h>	/* alloca() on Win32 */
#endif

enum {
	 SYNCTERM_PATH_INI
	,SYNCTERM_PATH_LIST
	,SYNCTERM_DEFAULT_TRANSFER_PATH
	,SYNCTERM_PATH_CACHE
};

/* Default modem device */
#if defined(__APPLE__) && defined(__MACH__)
/* Mac OS X */
#define DEFAULT_MODEM_DEV	"/dev/tty.modem"
#elif defined(_WIN32)
#define DEFAULT_MODEM_DEV	"COM1"
#else
/* FreeBSD */
#define DEFAULT_MODEM_DEV	"/dev/ttyd0"
#endif

/* "ALT/META" key name string */
#if defined(__APPLE__) && defined(__MACH__)
#define ALT_KEY_NAME	"COMMAND"
#define ALT_KEY_NAMEP	"Command"
#define ALT_KEY_NAME3CH	"CMD"
#else
#define ALT_KEY_NAME	"ALT"
#define ALT_KEY_NAMEP	"Alt"
#define ALT_KEY_NAME3CH	"ALT"
#endif

struct modem_settings {
	char	init_string[INI_MAX_VALUE_LEN];
	char	dial_string[INI_MAX_VALUE_LEN];
	char	device_name[INI_MAX_VALUE_LEN+1];
	ulong	com_rate;
};

struct syncterm_settings {
	int		confirm_close;
	int		startup_mode;
	int		output_mode;
	int		backlines;
	int		prompt_save;
	struct modem_settings mdm;
	char	TERM[INI_MAX_VALUE_LEN+1];
	char	list_path[MAX_PATH+1];
	int		scaling_factor;
};

extern char *inpath;
extern char *syncterm_version;
extern unsigned char *scrollback_buf;
extern unsigned int   scrollback_lines;
extern unsigned int  scrollback_mode;
extern unsigned int  scrollback_cols;
extern struct syncterm_settings settings;
void parse_url(char *url, struct bbslist *bbs, int dflt_conn_type, int force_defaults);
extern int default_font;
extern char *font_names[];
extern int safe_mode;
char *get_syncterm_filename(char *fn, int fnlen, int type, int shared);
void load_settings(struct syncterm_settings *set);
extern char *output_types[];
extern int output_map[];
extern char *output_descrs[];
extern char *output_enum[];
int ciolib_to_screen(int screen);
int screen_to_ciolib(int ciolib);

#endif
