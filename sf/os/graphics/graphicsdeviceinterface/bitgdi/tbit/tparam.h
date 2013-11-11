// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#if !defined(__TPARAM_H__)
#define __TPARAM_H__

#include "TGraphicsHarness.h"

//Some dispalay modes are scaled in the test.
//Check the implementation of void CTParam::DoTestL(TDisplayMode aDispMode),
//SetScalingFactor() call.

class CTParam : public CTGraphicsBase
	{
public:
	CTParam(CTestStep* aStep);
protected:
//from 	CTGraphicsStep
	virtual void RunTestCaseL(TInt aCurTestCase);
private:
	void DoTestL(TDisplayMode aDispMode);
	void DoAnotherTestL();
private:
	CFbsScreenDevice* iDevice;
	CFbsBitGc* iContext;
	CFbsBitmap iBitmap;
	};

class CTParamStep : public CTGraphicsStep
	{
public:
	CTParamStep();
protected:	
	//from CTGraphicsStep
	virtual CTGraphicsBase* CreateTestL();
	virtual void TestSetupL();
	virtual void TestClose();
	};

_LIT(KTParamStep,"TParam");

#endif
