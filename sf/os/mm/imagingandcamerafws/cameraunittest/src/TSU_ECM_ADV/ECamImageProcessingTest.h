
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
//

#ifndef ECamImageProcessingTest_H
#define ECamImageProcessingTest_H

#include <testframework.h>
#include "ECamObserverTest.h"

class RECamImageProcessingTest : public RTestStep
	{
public:
	static RECamImageProcessingTest* NewL(TBool aAllocTest);
protected:
	// from RTestStep;
	TVerdict DoTestStepL();
	TVerdict DoImageProcessingTestStepL();
private:
	RECamImageProcessingTest(TBool aAllocTest);	
	};
	
class RECamImageProcDefaultsTest : public RECamObserverTest /*RTestStep, public MCameraObserver2*/
	{
public:
	static RECamImageProcDefaultsTest* NewL(TBool aAllocTest);
protected:
	// from RTestStep;
	TVerdict DoTestStepL();
	TBool CompareUidArrays(RArray<TUid>& firstArray, RArray<TUid>& secondArray);
private:
	RECamImageProcDefaultsTest(TBool aAllocTest);
	void TestColorSwapL(CCamera::CCameraImageProcessing* aImageProcess, TVerdict& aResult);
	void TestColorAccentL(CCamera::CCameraImageProcessing* aImageProcess, TVerdict& aResult);
private:
	TUid iInputEventUid;	
	};

class RECamImageProcRangesTest : public RECamObserverTest /*RTestStep, public MCameraObserver2*/
	{
public:
	static RECamImageProcRangesTest* NewL(TBool aAllocTest);
protected:
	// from RTestStep;
	TVerdict DoTestStepL();
private:
	RECamImageProcRangesTest(TBool aAllocTest);	
	TUid iInputEventUid;	
	};

class RECamImageProcNegTest : public RECamObserverTest /*RTestStep, public MCameraObserver2*/
	{
public:
	static RECamImageProcNegTest* NewL(TBool aAllocTest);
protected:
	// from RTestStep;
	TVerdict DoTestStepL();
private:
	RECamImageProcNegTest(TBool aAllocTest);
	TUid iInputEventUid;	
	};	
	
class RECamImageProcBaselineTest : public RECamObserverTest /*RTestStep, public MCameraObserver2*/
	{
public:
	static RECamImageProcBaselineTest* NewL(TBool aAllocTest);
protected:
	// from RTestStep;
	TVerdict DoTestStepL();
	TBool CompareUidArrays(RArray<TUid>& firstArray, RArray<TUid>& secondArray);
private:
	RECamImageProcBaselineTest(TBool aAllocTest);
	TUid iInputEventUid;	
	};	
#endif // ECamImageProcessingTest_H
