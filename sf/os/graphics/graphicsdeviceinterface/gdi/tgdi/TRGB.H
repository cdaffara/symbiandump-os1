// Copyright (c) 1998-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __TRGB_H__
#define __TRGB_H__

#include <gdi.h>
#include "TGraphicsHarness.h"


class CTRgb : public CTGraphicsBase
	{
public:
	CTRgb(CTestStep* aStep);

protected:
//from 	CTGraphicsStep
	virtual void RunTestCaseL(TInt aCurTestCase);
private:
	void TestGray2();
	void TestGray4();
	void TestGray16();
	void TestGray256();
	void TestColor16();
	void TestColor256();
	void TestColor4K();
	void TestColor64K();
	void TestColor16M();
	void TestColor16MU();
	void TestColor16MA();
	void TestColor256Util();
	void TestColor16MAP();
	void ValidatePMAndNPM(TInt aAlpha, TInt aValue, TInt aPreMulVal, TInt aNonPreMulValue);
	};

class CTRgbStep : public CTGraphicsStep
	{
public:
	CTRgbStep();
protected:	
	//from CTGraphicsStep
	virtual CTGraphicsBase* CreateTestL();
	};

_LIT(KTRgbStep,"TRgb");


#endif
