// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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


#if !defined(__TFONTSELECT_H__)
#define __TFONTSELECT_H__

#include <test/testexecutestepbase.h>
#include "TGraphicsHarness.h"


class CTFontSelect : public CTGraphicsBase
	{
public:
	CTFontSelect(CTestStep* aStep);
	~CTFontSelect();
	void ConstructL();
protected:
//from 	CTGraphicsStep
	virtual void RunTestCaseL(TInt aCurTestCase);
private:
	void DoFontSelectTestsL();
private:
	CFbsFont* iFont1;
	CFbsFont* iFont2;
	CFbsFont* iFont3;
	CFbsBitGc* iGc;
	CFbsScreenDevice* iDevice;
	};

class CTFontSelectStep : public CTGraphicsStep
	{
public:
	CTFontSelectStep();
protected:	
	//from CTGraphicsStep
	virtual CTGraphicsBase* CreateTestL();
	virtual void TestSetupL();
	virtual void TestClose();
	};

_LIT(KTFontSelectStep,"TFontSelect");

#endif	// __TFONTSELECT_H__
