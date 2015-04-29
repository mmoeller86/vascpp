#include <sys/mman.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "posix.h"

VOID *
vmem_reserve (long long int n)
{
	VOID * va;

	va = mmap (0, n * 16, PROT_NONE, MAP_ANON | MAP_PRIVATE, -1, 0);
	return va;
}

BOOL
vmem_commit (VOID * va)
{
	long long int va0;

	va0 = (long long int)va;
	va0 -= (va0 % vmem_get_pagesize ());
	if (mprotect ((void *)va0, vmem_get_pagesize (), PROT_READ | PROT_WRITE) != 0) {
		fprintf (stderr, "mmap failed...\n");
		return FALSE;
	}

	return TRUE;
}

int
vmem_get_pagesize ()
{
	return sysconf (_SC_PAGE_SIZE);
}

BOOL
vmem_page_commited (VOID * va)
{
	unsigned char incore;
	long long int va0;
	int ret;

	// Cygwin doesn't has mincore ???
	va0 = (long long int)va;
	va0 -= (va0 % vmem_get_pagesize ());
	ret = mlock ((void *)va0, vmem_get_pagesize ());
	if (ret == 0) {
		munlock ((void *)va0, vmem_get_pagesize ());
		return TRUE;
	}

	return FALSE;
}

void
vmem_free (VOID * va, int n)
{
	munmap (va, n);
	return;
}
