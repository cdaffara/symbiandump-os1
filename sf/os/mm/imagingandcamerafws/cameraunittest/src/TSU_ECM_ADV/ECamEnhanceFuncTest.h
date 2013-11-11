// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// ECamEnhaceFuncTest.h
// 
//

#ifndef ECAMENHANCEFUNCTEST_H
#define ECAMENHANCEFUNCTEST_H

#include <testframework.h>
#include "ECamObserverTest.h"
#include "EnhanceFunctionality.h"


class RECamHistTest : public RTestStep
	{
public:
	static RECamHistTest* NewL(const TDesC& aTestStep);
protected:
	// from RTestStep;
	TVerdict DoTestStepL();
	
	// new
	TVerdict DoTestStep_20L();
	TVerdict DoTestStep_21L();
	TVerdict DoTestStep_22L();
	
	TVerdict DoHistTestStepL();
	
private:
	RECamHistTest(const TDesC& aTestStep);
	};

class RECamHistogramNotificationTest : public RECamObserverTest
	{
public:
	static RECamHistogramNotificationTest* NewL(TBool aAllocTest);

protected:
	// from RTestStep;
	TVerdict DoTestStepL();
	void ConstructL();
	
private:
	RECamHistogramNotificationTest(TBool aAllocTest);
	RArray<TUid> iOriginalSet;
	RArray<TUid> iResultSet;
	TUid iInputEventUid;
	};
	

// Overlay tests class
class RECamOvrlayTest : public RTestStep
	{
public:
	static RECamOvrlayTest* NewL(const TDesC& aTestStep);
protected:
	// from RTestStep;
	TVerdict DoTestStepL();
	TVerdict DoTestStep_30L();
	TVerdict DoTestStep_31L();
	
	TVerdict DoOverlayTestStepL();	
	
private:
	RECamOvrlayTest(const TDesC& aTestStep);
	};


// Snapshot test
class RECamSnapTest : public RTestStep
	{
public:
	static RECamSnapTest* NewL(const TDesC& aTestStep);
protected:
	// from RTestStep;
	TVerdict DoTestStepL();
	
	// new
	TVerdict DoTestStep_40L();
	TVerdict DoTestStep_41L();
	TVerdict DoTestStep_42L();
	
	TVerdict DoSnapTestStepL();
	
private:
	RECamSnapTest(const TDesC& aTestStep);
	};

class RECamSnapNotificationTest : public RECamObserverTest
	{
public:
	static RECamSnapNotificationTest* NewL(TBool aAllocTest);

protected:
	// from RTestStep;
	TVerdict DoTestStepL();
	void ConstructL();
	
private:
	RECamSnapNotificationTest(TBool aAllocTest);
	RArray<TUid> iOriginalSet;
	RArray<TUid> iResultSet;
	TUid iInputEventUid;
	};

#endif // ECAMENHANCEFUNCTEST_H

