// Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef TMULTIPLESCREENS_H
#define TMULTIPLESCREENS_H

#include <bitdev.h>
#include "TGraphicsHarness.h"

//
//
//The main test class. Add your test methods here.
class CTMultipleScreens : public CTGraphicsBase
	{
public:
	CTMultipleScreens(CTestStep* aStep);
	~CTMultipleScreens();
	void TestL();
protected:
//from 	CTGraphicsStep
	virtual void RunTestCaseL(TInt aCurTestCase);
	void ConstructL();
private:
	
	void CreateScreenDeviceL();

	void DoRotateMoveTextL();
	TInt CreateScrDevAndContext(TDisplayMode aDisplayMode);
	TInt CreateScrDevAndContext(TInt aScreenNo, TDisplayMode aDisplayMode);
	void DeleteScreenDevice();
	void DeleteGraphicsContext();
	void CreateFontL();
	void DestroyFont();

private:
	CFbsScreenDevice* iScrDev;
	CFbsBitGc* iGc;
	};

class CTMultipleScreensStep : public CTGraphicsStep
	{
public:
	CTMultipleScreensStep();
protected:	
	//from CTGraphicsStep
	virtual CTGraphicsBase* CreateTestL();
	virtual void TestSetupL();
	virtual void TestClose();
	};

_LIT(KTMultipleScreensStep,"TMultipleScreens");

#endif //TMULTIPLESCREENS_H
