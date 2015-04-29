#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#ifdef _MSC_VER
#include "win32.h"
#else
#include "posix.h"
#endif
#include "vas.h"

BOOL vas_key_cmp (VAS_KEY *, VAS_KEY *);

VAS_C *
vas_c_alloc ()
{
	return (VAS_C *)calloc (1, sizeof (VAS_C));
}

void
vas_c_free (VAS_C * c)
{
	free (c);
}

void
vas_c_set_key (VAS_C * c, VAS_KEY * key)
{
	c->key = key;
	return;
}

void
vas_c_set_data (VAS_C * c, VAS_VALUE data)
{
	c->data = data;
	return;
}

BOOL
vas_is_allocated (VOID * va)
{
	return vmem_page_commited (va);
}

VOID *
vas_get_virtual (VAS * vas, int i)
{
	return &vas->va [i * 16];
}

BOOL
vas_alloc (VOID * va)
{
	return vmem_commit (va);
}

VAS_E *
vas_e_alloc (VAS * vas, int i)
{
	if (vas_is_allocated (vas_get_virtual (vas, i)) == 0) {
		if (vas_alloc (vas_get_virtual (vas, i)) == FALSE)
			return NULL;
	}
	
	return (VAS_E *)&vas->va [i * 16];
}

void
vas_e_free (VAS * vas, int i)
{
	VAS_E * e;
	
	e = (VAS_E *)vas_get_virtual (vas, i);
	if (e->n > 0) {
		int i;
		
		for (i = 0; i < e->n; i++) {
			free (e->collides [i]->key);
			free (e->collides [i]);
		}
	
		free (e->collides);
	}
}

VAS_KEY *
vas_k_dup (VAS_KEY * key)
{
	VAS_KEY * k;
	
	k = (VAS_KEY *)calloc (1, sizeof (VAS_KEY));
	if (k == NULL)
		return NULL;

	memcpy (k, key, sizeof (VAS_KEY));
	return k;
}

BOOL
vas_e_append (VAS * vas, VAS_E * e, VAS_KEY * key, VAS_VALUE data)
{
	VAS_C * c;
	int i;
	
	if (vas_is_allocated (e) == FALSE) {
		if (vas_alloc (e) == FALSE) {
			fprintf (stderr, "failed to allocate page...\n");
			return FALSE;
		}
	}

	for (i = 0; i < e->n; i++)
		if (vas_key_cmp (e->collides [i]->key, key))
			return TRUE;
			
	e->n++;
	e->collides = (VAS_C **)realloc (e->collides, sizeof (VAS_C *) * e->n);
	if (e->collides == NULL) {
		fprintf (stderr, "failed to allocate e->collides...\n");
		return FALSE;
	}
		
	c = vas_c_alloc ();
	if (c == NULL) {
		fprintf (stderr, "vas_c_alloc failed...\n");
		return FALSE;
	}

	c->key = vas_k_dup (key);
	c->data = data;
	c->data.invalid = FALSE;
	e->collides [e->n -1] = c;
	return TRUE;
}

BOOL
vas_key_cmp (VAS_KEY * k0, VAS_KEY * k1)
{
	if (k0->dt != k1->dt)
		return FALSE;
		
	switch (k0->dt) {
		case VAS_DT_INT:
			return k0->value.i == k1->value.i;
		case VAS_DT_LL:
			return k0->value.ll == k1->value.ll;
		case VAS_DT_FLOAT:
			return k0->value.fl == k1->value.fl;
		case VAS_DT_DOUBLE:
			return k0->value.dbl == k1->value.dbl;
		case VAS_DT_LDOUBLE:
			return k0->value.ldbl == k1->value.ldbl;
		case VAS_DT_STR:
			return strcmp (k0->value.str, k1->value.str) == 0;
	}
	
	return FALSE;
}

BOOL
vas_e_remove (VAS * vas, VAS_E * e, VAS_KEY * key)
{
	VAS_C * c;
	int i;
	
	if (vas_is_allocated (e) == FALSE)
		return FALSE;

	for (i = 0; i < e->n; i++) {
		if (vas_key_cmp (e->collides [i]->key, key)) {
			memmove (&e->collides [i], e->collides [i +1], (e->n -i -1) * sizeof (VAS_C *));
			e->n--;
			e->collides = (VAS_C **)realloc (e->collides, e->n * sizeof (VAS_C *));
			return TRUE;
		}
	}
	
	return FALSE;
}

VAS_VALUE
vas_e_get (VAS * vas, VAS_E * e, VAS_KEY * key)
{
	VAS_C * c;
	VAS_VALUE va_invalid;
	int i;

	va_invalid.invalid = TRUE;
	va_invalid.val.str = NULL;
	if (vas_is_allocated (e) == FALSE)
		return va_invalid;

	for (i = 0; i < e->n; i++) {
		if (vas_key_cmp (e->collides [i]->key, key)) {
			return e->collides [i]->data;
		}
	}
	
	return va_invalid;
}

BOOL
vas_e_replace (VAS * vas, VAS_E * e, VAS_KEY * key, VAS_VALUE data)
{
	VAS_C * c;
	int i;

	if (vas_is_allocated (e) == FALSE)
		return FALSE;

	for (i = 0; i < e->n; i++) {
		if (vas_key_cmp (e->collides [i]->key, key)) {
			e->collides [i]->data = data;
			return TRUE;
		}
	}
	
	return FALSE;
}

BOOL
vas_init (VAS * vas, long long int n)
{
	VOID * va;
	
	va = vmem_reserve (n);
	if (va == NULL)
		return FALSE;
		
	vas->va = (CHAR *)va;
	vas->n = n;
	return TRUE;
}

VAS *
vas_alloc (long long int N)
{
	VAS * vas;
	
	vas = (VAS *)calloc (1, sizeof (VAS));
	vas_init (vas, N);
	return vas;
}

void
vas_free (VAS * vas)
{
	int i;
	
	for (i = 0; i < vas->n; i++) {
		if (vas_is_allocated (vas_get_virtual (vas, i)) == TRUE)
			vas_e_free (vas, i);
	}
	
	vmem_free (vas->va, vas->n * 16);
}

long long int
vas_hash_ll (VAS * vas, long long i)
{
	return ((i % vas->n) * 16);
}

long long int
vas_hash_str (VAS * vas, char * str)
{
	long long int hash;
	
	hash = 0;
	while (*str) {
		hash <<= 1;
		hash += *str++;
		hash ^= 0xABDE4567;
	}
	
	return vas_hash_ll (vas, hash);
}

long long int
vas_hash_voidptr (VAS * vas, CHAR * ptr, int len)
{
	long long int hash;
	int i;
	
	hash = 0;
	for (i = 0; i < len; i++) {
		hash <<= 1;
		hash += *ptr++;
		hash ^= 0xABDE4567;
	}
	
	return hash;
}

long long int
vas_hash (VAS * vas, VAS_KEY * key)
{
	switch (key->dt) {
		case VAS_DT_STR:
			return vas_hash_str (vas, (char *)key->value.str);
		case VAS_DT_VOIDPTR:
			return vas_hash_voidptr (vas, (CHAR *)key->value.ptr, key->len);
		case VAS_DT_LL:
			return vas_hash_ll (vas, (long long int)key->value.ll);
		case VAS_DT_INT:
			return vas_hash_ll (vas, (long long int)key->value.i);
		case VAS_DT_FLOAT:
			return vas_hash_ll (vas, (long long int)key->value.fl);
		case VAS_DT_DOUBLE:
			return vas_hash_ll (vas, (long long int)key->value.dbl);
		case VAS_DT_LDOUBLE:
			return vas_hash_ll (vas, (long long int)key->value.ldbl);
	}
	
	return 0;
}

BOOL
vas_insert (VAS * vas, VAS_KEY * key, VAS_VALUE data)
{
	long long int va;
	VAS_E * e;
	
	va = vas_hash (vas, key);
	e = (VAS_E *)(va + (long long int)vas->va);
	return vas_e_append (vas, e, key, data);
}

BOOL
vas_remove (VAS * vas, VAS_KEY * key)
{
	long long int va;
	VAS_E * e;
	
	va = vas_hash (vas, key);
	e = (VAS_E *)(va + (long long int)vas->va);
	return vas_e_remove (vas, e, key);
}

VAS_VALUE
vas_get (VAS * vas, VAS_KEY * key)
{
	long long int va;
	VAS_E * e;
	
	va = vas_hash (vas, key);
	e = (VAS_E *)(va + (long long int)vas->va);
	return vas_e_get (vas, e, key);
}

BOOL
vas_replace (VAS * vas, VAS_KEY * key, VAS_VALUE data)
{
	long long int va;
	VAS_E * e;
	
	va = vas_hash (vas, key);
	e = (VAS_E *)(va + (long long int)vas->va);
	return vas_e_replace (vas, e, key, data);
}
