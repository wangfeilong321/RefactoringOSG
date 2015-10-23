#include "PointerAccess.h"
#ifndef _WIN32 

static jmp_buf hndbuf;

static void handler(int ignored)
{
	(void)ignored;
	longjmp(hndbuf, 1);
}

BOOL IsBadWritePtr(const void *lp, size_t ucb)
{
	struct sigaction act;
	struct sigaction oact;
	char *base = (char*)const_cast<void*>(lp);
	char *over = base + ucb;
	volatile char *test;

	act.sa_handler = handler;
	sigemptyset(&act.sa_mask);
	act.sa_flags |= SA_RESTART;

	if (sigaction(SIGSEGV, &act, &oact) < 0) {
		return FALSE; /* Debatable action */
	}

	switch (setjmp(hndbuf)) {
	case 0:
		for (test = base; test < over; test++) {
			*test = *test;
		}

		(void)sigaction(SIGSEGV, &oact, NULL);
		return FALSE;
	default:
		break;
	}

	(void)sigaction(SIGSEGV, &oact, NULL);
	return TRUE;
}

void PtrTestHandler(int nSig)
{
	if (g_bPtrTestInstalled)
		longjmp(g_PtrTestJmpBuf, 1);
}

BOOL IsBadReadPtr(void* lp, size_t cb)
{
	size_t i;
	BYTE b1;
	BOOL bRet = TRUE;
	void(/*__cdecl*/* pfnPrevHandler)(int);
	g_bPtrTestInstalled = TRUE;
	if (setjmp(g_PtrTestJmpBuf))
	{
		bRet = FALSE;
		goto Ret;
	}
	pfnPrevHandler = signal(SIGSEGV, PtrTestHandler);

	for (i = 0; i < cb; i++)
		b1 = ((BYTE*)lp)[i];
Ret:
	g_bPtrTestInstalled = FALSE;
	signal(SIGSEGV, pfnPrevHandler);

	return bRet;
}

#endif //!_WIN32