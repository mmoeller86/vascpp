#include "win32.h"

VOID *
vmem_reserve (int n)
{
	VOID * va;

	va = VirtualAlloc (0, n * 16, MEM_RESERVE, PAGE_READWRITE);
	return va;
}

BOOL
vmem_commit (VOID * va)
{
	if (VirtualAlloc (va, vmem_get_pagesize (), MEM_COMMIT, PAGE_READWRITE) == 0)
		return FALSE;
	
	return TRUE;
}

int
vmem_get_pagesize ()
{
	SYSTEM_INFO si;
	
	GetSystemInfo (&si);
	return si.dwPageSize;
}

BOOL
vmem_page_commited (VOID * va)
{
	MEMORY_BASIC_INFORMATION mbi;
	mbi.RegionSize = vmem_get_pagesize ();
	if (VirtualQuery (va, &mbi, sizeof (mbi)) == 0)
		return FALSE;
		
	return (mbi.State & MEM_COMMIT) == MEM_COMMIT;
}

void
vmem_free (VOID * va, int n)
{
	VirtualFree (va, n, MEM_DECOMMIT | MEM_RELEASE);
	return;
}
