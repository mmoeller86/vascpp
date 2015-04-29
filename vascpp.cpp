#include "vascpp.h"

VASCpp::VASCpp (int N)
{
	vas = vas_alloc (N);
	return;
}

VASCpp::~VASCpp ()
{
	vas_free (vas);
	return;
}

VAS_VALUE
VASCpp::get (VAS_KEY * key)
{
	return vas_get (vas, key);
}

bool
VASCpp::insert (VAS_KEY * key, VAS_VALUE data)
{
	return vas_insert (vas, key, data);
}

bool
VASCpp::remove (VAS_KEY * key)
{
	return vas_remove (vas, key);
}

bool
VASCpp::replace (VAS_KEY * key, VAS_VALUE data)
{
	return vas_replace (vas, key, data);
}
