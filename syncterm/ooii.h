#ifndef _OOII_H_
#define _OOII_H_

#include <genwrap.h>

#define MAX_OOII_MODE 3

BOOL handle_ooii_code(char *codeStr, int *ooii_mode, char *retbuf, size_t retsize);

#endif
