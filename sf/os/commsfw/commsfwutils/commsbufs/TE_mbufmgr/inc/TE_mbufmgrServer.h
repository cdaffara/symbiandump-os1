/**
 * Copyright (c) 2008 Symbian Ltd.  All rights reserved.
 *
 */
#if (!defined __TE_mbufmgr_SERVER_H__)
#define __TE_mbufmgr_SERVER_H__

#include <test/TestExecuteServerBase.h>
#include <es_mbman.h> 
//const TUint KMBufMinHeapSize        = 4 * 1024;

class CTE_mbufmgrServer : public CTestServer
	{
public:
	static CTE_mbufmgrServer* NewL();
	virtual CTestStep* CreateTestStep(const TDesC& aStepName);
	RFs& Fs(){return iFs;};

private:
	RFs iFs;
	};
#endif
