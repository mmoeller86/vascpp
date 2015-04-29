#pragma once

#include "types.h"

int mincore(void *addr, size_t length, unsigned char *vec);

VOID *		vmem_reserve (long long int n);
BOOL		vmem_commit (VOID * va);
BOOL		vmem_page_commited (VOID * va);
void		vmem_free (VOID * va, int n);
int		vmem_get_pagesize ();
