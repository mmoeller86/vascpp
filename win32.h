#pragma once

#include "types.h"
VOID *		vmem_reserve (int n);
BOOL		vmem_commit (VOID * va);
BOOL		vmem_page_commited (VOID * va);
void		vmem_free (VOID * va, int n);
int		vmem_get_pagesize ();
