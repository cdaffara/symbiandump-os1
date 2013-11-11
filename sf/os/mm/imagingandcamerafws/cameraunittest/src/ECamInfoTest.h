
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

#ifndef ECAMINFOTEST_H
#define ECAMINFOTEST_H

#include <testframework.h>

class RECamInfoTest : public RTestStep
	{
public:
	static RECamInfoTest* NewL(TBool aAllocTest);
protected:
	// from RTestStep;
	TVerdict DoTestStepL();
private:
	RECamInfoTest(TBool aAllocTest);
	
	};

class RECamInnerInfoTest : public RTestStep
	{
public:
	static RECamInnerInfoTest* NewL(TBool aAllocTest);
protected:
	// from RTestStep;
	TVerdict DoTestStepL();
private:
	RECamInnerInfoTest(TBool aAllocTest);
	
	};

#endif // ECAMINFOTEST_H
