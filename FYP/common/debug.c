#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "debug.h"

void error(const char* msg)
{
	fprintf(stderr, "%s\n", msg);
	exit(0);
}

