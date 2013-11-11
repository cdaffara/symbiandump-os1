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

#if !defined(__TBASE_H__)
#define __TBASE_H__

#include "TGDI.H"
#include "TGraphicsHarness.h"

class CTGdi : public CTGraphicsBase
	{
public:
	CTGdi(CTestStep* aStep);
protected:
//from 	CTGraphicsStep
	virtual void RunTestCaseL(TInt aCurTestCase);
private:
	void DoTestL();
	void TestScreenL(TDisplayMode aDispMode);
	void TestBitmapL(TDisplayMode aDispMode);
	void DoTestAndDeleteL(CFbsDevice* aDevice,TBool aScreen);
private:
	TestGdi iTestGdi;
	};

class CTGdiStep : public CTGraphicsStep
	{
public:
	CTGdiStep();
protected:	
	//from CTGraphicsStep
	virtual CTGraphicsBase* CreateTestL();
	virtual void TestSetupL();
	virtual void TestClose();
	};

_LIT(KTGdiStep,"TGdi");

#endif
