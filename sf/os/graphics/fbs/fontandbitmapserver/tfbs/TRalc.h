// Copyright (c) 1997-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __TRALC_H__
#define __TRALC_H__

#include <fbs.h>
#include "test/TGraphicsHarness.h"

class CTRalc : public CTGraphicsBase
	{
public:
	CTRalc(CTestStep* aStep);
	~CTRalc();
protected:
//from 	CTGraphicsStep
	virtual void RunTestCaseL(TInt aCurTestCase);
	void ConstructL();

	void ExpandCleanupStackL();

	void TestRALCEntryClass();
	void TestRALCCacheClass();
	void TestOOMCondition();

	void MeasurePerformanceWith2();
	void MeasurePerformanceWith2Long();
	void MeasurePerformanceWith5();
	void MeasurePerformanceWith23Mix();
private:
	TInt iHandle;
	RFbsSession* iFbs;
	TInt iResourceCount;
	TInt iAllocs;
	TBool iShutdownFbs;
	TFileName iTestBitmapName;
	};

class CTRalcStep : public CTGraphicsStep
	{
public:
	CTRalcStep();
protected:	
	//from CTGraphicsStep
	virtual CTGraphicsBase* CreateTestL();
	virtual void TestSetupL();
	virtual void TestClose();
private:
	TInt iStartProcessHandleCount;
	TInt iStartThreadHandleCount;
	TInt iEndProcessHandleCount;
	TInt iEndThreadHandleCount;
			
	RThread iThread;
	};

_LIT(KTRalcStep,"TRalc");


#endif
