// Copyright (c) 1996-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file
 @test
 @internalComponent - Internal Symbian test code
*/

#ifndef __TPROCPRI_H__
#define __TPROCPRI_H__

#include "AUTO.H"
#include "TGraphicsHarness.h"
#include "../tlib/testbase.h"
#include <e32std.h>
#include <w32std.h>

#define OTHER_PROC_GROUP_NAME _L("OtherProcGroupWindow")
#define PROC_PRI_SEMAPHORE_NAME1 _L("abcerProcSem1")
#define PROC_PRI_SEMAPHORE_NAME2 _L("OtherProcSem2")

class CTProcPri : public CTWsGraphicsBase
	{
private:
	enum
		{
		ECreatedSem1=0x0001,
		ECreatedSem2=0x0002,
		};
public:
	CTProcPri(CTestStep* aStep);
	~CTProcPri();
	void ConstructL();
	void TestPriChangeL();
protected:
//from 	CTGraphicsStep
	virtual void RunTestCaseL(TInt aCurTestCase);
private:
	TSize iWinSize;
	RSemaphore iSem1;
	RSemaphore iSem2;
	TUint iFlags;
	CProcess* iProcess;
	};


class CTProcPriStep : public CTGraphicsStep
	{
public:
	CTProcPriStep();
protected:	
	//from CTGraphicsStep
	virtual CTGraphicsBase* CreateTestL();
	};

_LIT(KTProcPriStep,"TProcPri");


#endif
