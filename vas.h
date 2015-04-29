#pragma once
#include "types.h"

typedef enum {
	VAS_DT_INVALID,
	VAS_DT_VOIDPTR,
	VAS_DT_STR,
	VAS_DT_INT,
	VAS_DT_LL,
	VAS_DT_FLOAT,
	VAS_DT_DOUBLE,
	VAS_DT_LDOUBLE
} VAS_DT;

typedef union {
	char *	str;
	int		i;
	long long ll;
	float	fl;
	double	dbl;
	long double ldbl;
	void *		ptr;
} VAS_VAL;

typedef struct {
	BOOL	invalid;
	int		len;
	VAS_VAL	val;
} VAS_VALUE;

typedef struct {
	VAS_DT		dt;
	int			len;
	VAS_VAL		value;
} VAS_KEY;

typedef struct {
	VAS_KEY *	key;
	VAS_VALUE	data;
	int			data_len;
} VAS_C;

typedef struct {
	long long int		n;
	VAS_C **	collides;
} VAS_E;

typedef struct {
	char *		va;
	long long int n;
} VAS;

VAS *		vas_alloc (long long int N);
BOOL		vas_init (VAS * vas, long long int n);
void		vas_free (VAS * vas);
BOOL		vas_insert (VAS * vas, VAS_KEY * key, VAS_VALUE data);
BOOL		vas_remove (VAS * vas, VAS_KEY * key);
BOOL		vas_replace (VAS * vas, VAS_KEY * key, VAS_VALUE data);
VAS_VALUE	vas_get (VAS * vas, VAS_KEY * key);

BOOL		vas_is_allocated (long long int va);
VOID *		vas_get_virtual (VAS * vas, int i);
