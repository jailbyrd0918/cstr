#include "cstr.h"

#include <stdio.h>


int main(int argc, char **argv)
{
	string_t str = cstr_new("Hello, world!");

	printf("%s\n", str);

	cstr_destroy(str);
	return 0;
}


