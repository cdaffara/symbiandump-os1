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

#ifndef __TFBSDEFECT_H__
#define __TFBSDEFECT_H__

#include <fbs.h>
#include "test/TGraphicsHarness.h"

class CTFbsDefect : public CTGraphicsBase
	{
public:
	CTFbsDefect(CTestStep* aStep);
protected:
//from 	CTGraphicsStep
	virtual void RunTestCaseL(TInt aCurTestCase);
private:
	void CreateBitmapTestL();
	void TestDisplayModeL();
	void TestFbservChunkLockingL();
	void TestKErrEof();
	void CreateBitmapOOMTestL();
	};
	

class CTFbsDefectStep : public CTGraphicsStep
	{
public:
	CTFbsDefectStep();
protected:	
	//from CTGraphicsStep
	virtual CTGraphicsBase* CreateTestL();
	};

_LIT(KTFbsDefectStep,"TFbsDefect");


#endif
