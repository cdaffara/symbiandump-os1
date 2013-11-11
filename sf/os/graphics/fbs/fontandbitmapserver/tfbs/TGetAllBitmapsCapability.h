// Copyright (c) 2006-2010 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __TGETALLBITMAPSCAPABILITY_H__
#define __TGETALLBITMAPSCAPABILITY_H__

#include "test/TGraphicsHarness.h"

// The main test class. Add your test methods here.
class CTGetAllBitmapsCapability : public CTGraphicsBase
	{
public:
	CTGetAllBitmapsCapability(CTestStep* aStep);
	~CTGetAllBitmapsCapability();
	
protected:
	// From CTGraphicsStep
	virtual void RunTestCaseL(TInt aCurTestCase);

private:
	void GetAllBitmapsTestL();
	};

class CTGetAllBitmapsCapabilityStep : public CTGraphicsStep
	{
public:
	CTGetAllBitmapsCapabilityStep();
protected:	
	//from CTGraphicsStep
	virtual CTGraphicsBase* CreateTestL();	
	};

_LIT(KTGetAllBitmapsCapabilityStep,"TGetAllBitmapsCapability");

#endif	//__TGETALLBITMAPSCAPABILITY_H__
