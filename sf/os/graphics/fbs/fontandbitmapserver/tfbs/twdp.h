// Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __TWDP_H__
#define __TWDP_H__

#include <fbs.h>
#include "test/TGraphicsHarness.h"

class CTWDP : public CTGraphicsBase
	{
public:
	CTWDP(CTestStep* aStep);
	~CTWDP();
protected:
	//from 	CTGraphicsStep
	virtual void RunTestCaseL(TInt aCurTestCase);
	virtual void ConstructL();

private:
	void TestFbservPaged();
	void TestFbservUnPaged();
	void TestBitmapDataPagedL();
	void TestBitmapDataAndSharedHeapPagedL();
	TBool DefaultDataPaged();
	TBool BitmapDataPagedL();
	
private:
	RFbsSession* iFbs;
	RChunk iBitmapChunk;
	RChunk iSharedHeapChunk;
	};


class CTWDPStep : public CTGraphicsStep
	{
public:
	CTWDPStep();
protected:	
	//from CTGraphicsStep
	virtual CTGraphicsBase* CreateTestL();
	};

_LIT(KTWDPStep,"TWDP");

#endif
