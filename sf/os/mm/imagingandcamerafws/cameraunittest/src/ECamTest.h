
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

#ifndef ECAMTEST_H
#define ECAMTEST_H

#include <testframework.h>

class RECamTest : public RTestStep
	{
public:
	static RECamTest* NewL(TBool aAllocTest);
protected:
	// from RTestStep;
	TVerdict DoTestStepL();
private:
	RECamTest(TBool aAllocTest);
	
	};

class RECamInnerTest : public RTestStep
	{
public:
	static RECamInnerTest* NewL(TBool aAllocTest);
protected:
	// from RTestStep;
	TVerdict DoTestStepL();
private:
	RECamInnerTest(TBool aAllocTest);
	
	};

#endif // ECAMTEST_H
