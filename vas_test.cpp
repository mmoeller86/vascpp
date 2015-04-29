#include <stdio.h>
#include "vas.h"

int main (int c, char ** args)
{
	VAS vas;
	long long int val;
	VAS_VALUE value;
	VAS_KEY key;
	
	setbuf (stdout, NULL);
	if (vas_init (&vas, 1024 * 1024 * 16) == FALSE) {
		fprintf (stderr, "%s: vas_init failed...\n", args [0]);
		return 1;
	}

	value.invalid = FALSE;
	value.val.ll = 1000;
	key.dt = VAS_DT_LL;
	key.value.ll = 1000;
	if (vas_insert (&vas, &key, value) == FALSE) {
		fprintf (stderr, "%s: vas_insert failed...\n", args [0]);
		vas_free (&vas);
		return 2;
	}
	
	value = vas_get (&vas, &key);
	printf ("val = %d\n", value.val.ll);
	
	value.invalid = FALSE;
	value.val.ll = 2000;
	vas_replace (&vas, &key, value);
	
	value = vas_get (&vas, &key);
	printf ("val = %d\n", value.val.ll);

	vas_free (&vas);
	return 0;
}