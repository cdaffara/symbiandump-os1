// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#if !defined(__TRWINDOWS_H__)
#define __TRWINDOWS_H__
#include "TGraphicsHarness.h"

class CTRWindows : public CTGraphicsBase
	{
public:
	CTRWindows(CTestStep* aStep);
protected:
//from 	CTGraphicsStep
	virtual void RunTestCaseL(TInt aCurTestCase);
private:
	void TestCreatingWindowsL(TSize aSize);
	};

class CTRWindowsStep : public CTGraphicsStep
	{
public:
	CTRWindowsStep();
protected:	
	//from CTGraphicsStep
	virtual CTGraphicsBase* CreateTestL();
	};

_LIT(KTRWindowsStep,"TRWindows");

#endif
