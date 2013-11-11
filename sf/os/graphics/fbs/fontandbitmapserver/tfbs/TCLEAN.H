// Copyright (c) 1996-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __TCLEAN_H__
#define __TCLEAN_H__

#include <fbs.h>
#include "test/TGraphicsHarness.h"

enum TTestType
	{
	EFont1,
	EFont2,
	EBitmap1,
	EPile1,
	ESession1,
	ESession2,
	ETypefaceStore1,
	EBadMessage
	};

class CTClean : public CTGraphicsBase
	{
public:
	CTClean(CTestStep* aStep);
	~CTClean();
	void DeleteScanLineBuffer();
protected:
//from 	CTGraphicsStep
	virtual void RunTestCaseL(TInt aCurTestCase);
	void ConstructL();
private:
	void LaunchThread(TTestType aType);
	void DeadThread();
private:
	RFbsSession* iFbs;
	CFbsTypefaceStore* iStore;
	CFbsBitmap *iBitmap;
	CTrapCleanup* iTc;
	TBool iShutdownFbs;
	};

class CTCleanStep : public CTGraphicsStep
	{
public:
	CTCleanStep();
protected:	
	//from CTGraphicsStep
	virtual CTGraphicsBase* CreateTestL();
	};

_LIT(KTCleanStep,"TClean");



#endif
