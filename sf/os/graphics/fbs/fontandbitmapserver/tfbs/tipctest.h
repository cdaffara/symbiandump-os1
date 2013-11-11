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

#ifndef __TIPCTest_H__
#define __TIPCTest_H__

#include <fbs.h>
#include "test/TGraphicsHarness.h"

class RIPCTestFbservClient : public RSessionBase
	{
public:
	RIPCTestFbservClient();
	~RIPCTestFbservClient();
	TInt Connect();
	
	void SendInvalidSize(TInt aFunctionNumber);
	void SendInvalidDisplayMode(TInt aFunctionNumber);
	void SendInvalidNameLength(TInt aFunctionNumber);
private:
	};

class CTIPCTest : public CTGraphicsBase
	{
public:
	CTIPCTest(CTestStep* aStep);
	~CTIPCTest();
protected:
//from 	CTGraphicsStep
	virtual void RunTestCaseL(TInt aCurTestCase);
	
	void ConstructL();

private:
	RFbsSession* iFbs;
	CFontStore* iFs;
	RHeap* iHeap;
	TBool iShutdownFbs;
	RIPCTestFbservClient iClient;
	};	

class CTIPCTestStep : public CTGraphicsStep
	{
public:
	CTIPCTestStep();
protected:	
	//from CTGraphicsStep
	virtual CTGraphicsBase* CreateTestL();
	};




_LIT(KTIPCTestStep,"TIPCTest");

#endif
