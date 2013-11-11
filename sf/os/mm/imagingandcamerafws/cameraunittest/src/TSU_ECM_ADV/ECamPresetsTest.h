
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

#ifndef ECamPresetsTest_H
#define ECamPresetsTest_H

#include <testframework.h>
#include "ECamObserverTest.h"

class RECamPresetsTest : public RTestStep
	{
public:
	static RECamPresetsTest* NewL(TBool aAllocTest);
protected:
	// from RTestStep;
	TVerdict DoTestStepL();
	TVerdict DoPresetsTestStepL();
private:
	RECamPresetsTest(TBool aAllocTest);
	
	};

class RECamPresetsNotificationTest : public RECamObserverTest
	{
public:
	static RECamPresetsNotificationTest* NewL(TBool aAllocTest);

protected:
	// from RTestStep;
	TVerdict DoTestStepL();
	void ConstructL();
	
private:
	RECamPresetsNotificationTest(TBool aAllocTest);
	RArray<TUid> iOriginalSet;
	RArray<TUid> iResultSet;
	TUid iInputEventUid;
	};
	
class RECamPresetsBaselineTest : public RECamObserverTest
	{
public:
	static RECamPresetsBaselineTest* NewL(TBool aAllocTest);

protected:
	// from RTestStep;
	TVerdict DoTestStepL();
	void ConstructL();
	
private:
	RECamPresetsBaselineTest(TBool aAllocTest);
	RArray<TUid> iOriginalSet;
	RArray<TUid> iResultSet;
	TUid iInputEventUid;
	};

	
class RECamPresetsNegTest : public RECamObserverTest
	{
public:
	static RECamPresetsNegTest* NewL(TBool aAllocTest);
protected:
	// from RTestStep;
	TVerdict DoTestStepL();
private:
	RECamPresetsNegTest(TBool aAllocTest);
	TUid iInputEventUid;
	};
#endif // ECamPresetsTest_H

