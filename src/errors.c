#include <lua.h>
#include "globals.h"

/* Error handling */

void set_error(struct lua_State *l, char *error_msg)
{
	fatal_error_msg = strdup(error_msg);
	fprintf(stderr, "GOJOERROR: %s\n",error_msg);
	fatal_error = 1;
}
