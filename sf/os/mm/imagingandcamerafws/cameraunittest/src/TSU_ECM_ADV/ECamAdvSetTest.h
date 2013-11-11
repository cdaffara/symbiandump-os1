
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

#ifndef ECamAdvSetTest_H
#define ECamAdvSetTest_H

#include <testframework.h>
#include <ecamadvsettings.h>
#include "ECamObserverTest.h"

class RECamAdvSetTest : public RTestStep
	{
public:
	static RECamAdvSetTest* NewL(TBool aAllocTest);
protected:
	// from RTestStep;
	TVerdict DoTestStepL();
	TVerdict DoAdvTestStepL();
private:
	RECamAdvSetTest(TBool aAllocTest);	
	};

class RECamAdvSetNotificationTest : public  RECamObserverTest
	{
public:
	static RECamAdvSetNotificationTest* NewL(TBool aAllocTest);

protected:
	// from RTestStep;
	TVerdict DoTestStepL();
	void ConstructL();
	
private:
	RECamAdvSetNotificationTest(TBool aAllocTest);
	};
	
class RECamAdvSetDefaultsTest : public RECamObserverTest
	{
public:
	static RECamAdvSetDefaultsTest* NewL(TBool aAllocTest);
protected:
	// from RTestStep;
	TVerdict DoTestStepL();
	
private:
	void TestISOTypesL(CCamera::CCameraAdvancedSettings* aSettings, CCamera::CCameraAdvancedSettings::TISORateType aIsoRateType, RArray<TInt> aSupportedIsoRates, TVerdict& aResult);
private:
	RECamAdvSetDefaultsTest(TBool aAllocTest);	
	TUid iInputEventUid;
	};
	
class RECamAdvSetGettersTest : public RECamObserverTest
	{
public:
	static RECamAdvSetGettersTest* NewL(TBool aAllocTest);
protected:
	// from RTestStep;
	TVerdict DoTestStepL();
private:
	RECamAdvSetGettersTest(TBool aAllocTest);	
	TUid iInputEventUid;
	};
	
class RECamAdvSetTwoCamTest : public RECamObserverTest
	{
public:
	static RECamAdvSetTwoCamTest* NewL(TBool aAllocTest);
protected:
	// from RTestStep;
	TVerdict DoTestStepL();
private:
	RECamAdvSetTwoCamTest(TBool aAllocTest);	
	TUid iInputEventUid;
	};
	
class RECamAdvSetNegTest : public RECamObserverTest
	{
public:
	static RECamAdvSetNegTest* NewL(TBool aAllocTest);
protected:
	// from RTestStep;
	TVerdict DoTestStepL();
private:
	RECamAdvSetNegTest(TBool aAllocTest);	
	TUid iInputEventUid;
	};
	
class RECamAdvSetBaselineTest : public RECamObserverTest
	{
public:
	static RECamAdvSetBaselineTest* NewL(TBool aAllocTest);
protected:
	// from RTestStep;
	TVerdict DoTestStepL();
private:
	RECamAdvSetBaselineTest(TBool aAllocTest);	
	TUid iInputEventUid;
	};

class RECamAdvSetTest2 : public RTestStep
	{
public:
	static RECamAdvSetTest2* NewL(TBool aAllocTest);
protected:
	// from RTestStep;
	TVerdict DoTestStepL();
	void RatioSubStepL(CCamera::CCameraAdvancedSettings* aCamera, TInt aSuppRange, TInt aTestValue, TInt aExpValues);
	void FlashSubStepL(CCamera::CCameraAdvancedSettings*, TInt aSuppRange, TInt aTestValue, TInt aExpValues);
	void OvrSubStepL(CCamera& aCamera, TInt aSuppRange, TInt aTestValue, TInt aExpValues);
private:
	RECamAdvSetTest2(TBool aAllocTest);	
	};

class RECamContinuousZoomAllocTest : public RECamContinousZoomObserverTest
	{
public:
	static RECamContinuousZoomAllocTest* NewL(TBool aAllocTest);
protected:
	// from RTestStep;
	TVerdict DoTestStepL();
	TVerdict DoAllocTestStepL();
private:
	RECamContinuousZoomAllocTest(TBool aAllocTest);	
	};

class RECamContinuousZoomTest : public RECamContinousZoomObserverTest
	{
public:
	static RECamContinuousZoomTest* NewL(TBool aAllocTest);
protected:
	// from RTestStep;
	TVerdict DoTestStepL();
private:
	RECamContinuousZoomTest(TBool aAllocTest);
	};

class RECamDependantSupportedDriveModesTest : public RTestStep
	{
public:
	static RECamDependantSupportedDriveModesTest* NewL(TBool aAllocTest);
protected:
	// from RTestStep;
	TVerdict DoTestStepL();
private:
	RECamDependantSupportedDriveModesTest(TBool aAllocTest);
	};

#endif // ECamAdvSetTest_H
