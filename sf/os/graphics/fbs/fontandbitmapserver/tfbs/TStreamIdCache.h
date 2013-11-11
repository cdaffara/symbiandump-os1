// Copyright (c) 2004-2010 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//

#ifndef __TSTREAMIDCACHE_H__
#define __TSTREAMIDCACHE_H__

#include <fbs.h>
#include "test/TGraphicsHarness.h"

class CTStreamIdCache : public CTGraphicsBase
	{
public:
	CTStreamIdCache(CTestStep* aStep);
	~CTStreamIdCache();
protected:
//from 	CTGraphicsStep
	virtual void RunTestCaseL(TInt aCurTestCase);
	void ConstructL();
	
private:	
	void ExpandCleanupStackL();

	void TestStreamIdCacheEntry();	
	void TestProcessingBitmapStream();
	void TestComparingStreams();
	void TestReplacingFile();
	void TestInvalidArgument();
	void TestOOMCondition();
	void TestPerformance();
	void TestLoadAtOffset();
	void DoTestingL();
	TInt FileSizeL(const TDesC& aFileName);
	void TestSessionClose();
private:
	RFbsSession* iFbs;
	RFs iFs;
	TBool iShutdownFbs;
	TFileName iTestBitmapName;
	TInt iSessionHandle;
	};

class CTStreamIdCacheStep : public CTGraphicsStep
	{
public:
	CTStreamIdCacheStep();
protected:	
	//from CTGraphicsStep
	virtual CTGraphicsBase* CreateTestL();
	void TestSetupL();
	void TestClose();
	
private:
	TInt iStartProcessHandleCount;
	TInt iStartThreadHandleCount;
	TInt iEndProcessHandleCount;
	TInt iEndThreadHandleCount;
			
	RThread iThread;
	};

_LIT(KTStreamIdCacheStep,"TStreamIdCache");


#endif
