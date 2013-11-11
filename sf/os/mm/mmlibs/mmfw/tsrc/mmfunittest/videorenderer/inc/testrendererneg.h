// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef TESTRENDERERNEG_H
#define TESTRENDERERNEG_H

// INCLUDES
#include "testrenderer.h"

/* 
Test creating surface with different parameters
MM-MMF-VIDREND-U-0100 to MM-MMF-VIDREND-U-0107
*/
class RTestRendererCreateSurfaceStep : public RTestRendererStep
	{
public:
	static RTestRendererCreateSurfaceStep* NewL(const TDesC& aStepName, 
													TInt aExpectedError, 
													TRgbFormat aRgbFormat, 
													TSize aSize, 
													TInt aNumBuffers);

protected:
	RTestRendererCreateSurfaceStep(const TDesC& aStepName,
									TInt aExpectedError, 
									TRgbFormat aRgbFormat, 
									TSize aSize, 
									TInt aNumBuffers);
    TVerdict DoTestStepL();
	virtual void FsmL(TTestRendererEvents /*aEventCode*/) {};
    
private:
	TInt iExpectedError;
	TInt iNumBuffers;
	TInt iDisplayId;
	};

/* 
Test update buffer twice in timed and non-timed mode
This test case is run by different script for udeb and urel mode to expect different behaviour
MM-MMF-VIDREND-U-0108 and MM-MMF-VIDREND-U-0109
*/
class RTestRendererUpdateTwiceStep : public RTestRendererStep
	{
public:
	static RTestRendererUpdateTwiceStep* NewL(const TDesC& aStepName, TBool aTimed);

protected:
	RTestRendererUpdateTwiceStep(const TDesC& aStepName, TBool aTimed);
	void FsmL(TTestRendererEvents aEventCode);
	};

/*
Test release buffer twice
This class is also base class for test steps that do not need to start active scheduler
MM-MMF-VIDREND-U-0110
*/
class RTestRendererReleaseTwiceStep : public RTestRendererStep
	{
public:
	static RTestRendererReleaseTwiceStep* NewL(const TDesC& aStepName, TBool aTimed);

protected:
	RTestRendererReleaseTwiceStep(const TDesC& aStepName, TBool aTimed);
	void FsmL(TTestRendererEvents aEventCode);
	TVerdict DoTestStepL();
	
protected:
	TVideoFrameBuffer* iBuffer;
	};

/*
Test using surface after destroying surface
MM-MMF-VIDREND-U-0111
*/
class RTestRendererDestroySurfaceStep : public RTestRendererReleaseTwiceStep
	{
public:
	static RTestRendererDestroySurfaceStep* NewL(const TDesC& aStepName, TBool aTimed);

protected:
	RTestRendererDestroySurfaceStep(const TDesC& aStepName, TBool aTimed);
	void FsmL(TTestRendererEvents aEventCode);
	};

/* 
Test update buffer immediately destroy surface in timed and non-timed mode
MM-MMF-VIDREND-U-0112 and MM-MMF-VIDREND-U-0113
*/
class RTestRendererUpdateAndDestroyStep : public RTestRendererStep
	{
public:
	static RTestRendererUpdateAndDestroyStep* NewL(const TDesC& aStepName, TBool aTimed);
	static TInt TimerCallbackFunc(TAny *aPtr);
	void FsmL(TTestRendererEvents aEventCode);

protected:
	RTestRendererUpdateAndDestroyStep(const TDesC& aStepName, TBool aTimed);
    TVerdict DoTestStepPreambleL();
    TVerdict DoTestStepPostambleL();
	
protected:
	CCallBackTimer* iCallbackTimer;
	};

/* 
Test update buffer immediately delete video renderer in timed and non-timed mode
MM-MMF-VIDREND-U-0114 and MM-MMF-VIDREND-U-0115
*/
class RTestRendererUpdateAndDeleteStep : public RTestRendererReleaseTwiceStep
	{
public:
	static RTestRendererUpdateAndDeleteStep* NewL(const TDesC& aStepName, TBool aTimed);

protected:
	RTestRendererUpdateAndDeleteStep(const TDesC& aStepName, TBool aTimed);
	void FsmL(TTestRendererEvents aEventCode);
	};

/* 
Test update buffer with invalid buffer in timed and non-timed mode
MM-MMF-VIDREND-U-0116 and MM-MMF-VIDREND-U-0117
*/
class RTestRendererUpdateInvalidStep : public RTestRendererReleaseTwiceStep
	{
public:
	static RTestRendererUpdateInvalidStep* NewL(const TDesC& aStepName, TBool aTimed);

protected:
	RTestRendererUpdateInvalidStep(const TDesC& aStepName, TBool aTimed);
	void FsmL(TTestRendererEvents aEventCode);
	};

/* 
Test creating surface twice
MM-MMF-VIDREND-U-0118
*/
class RTestRendererCreateTwiceStep : public RTestRendererReleaseTwiceStep
	{
public:
	static RTestRendererCreateTwiceStep* NewL(const TDesC& aStepName, TBool aTimed);

protected:
	RTestRendererCreateTwiceStep(const TDesC& aStepName, TBool aTimed);
	void FsmL(TTestRendererEvents aEventCode);
	};

/* 
Test destroying surface that does not exist
MM-MMF-VIDREND-U-0119
*/
class RTestRendererDestroyInvalidStep : public RTestRendererReleaseTwiceStep
	{
public:
	static RTestRendererDestroyInvalidStep* NewL(const TDesC& aStepName, TBool aTimed);

protected:
	RTestRendererDestroyInvalidStep(const TDesC& aStepName, TBool aTimed);
	void FsmL(TTestRendererEvents aEventCode);
	};

/* 
Test resoure file reader
MM-MMF-VIDREND-U-0120
*/
class RTestResourceFileReaderStep : public RTestStep
	{
public:
	static RTestResourceFileReaderStep* NewL(const TDesC& aStepName, 
												const TDesC& aFileName, 
												TInt aExpectedFormatErr, 
												TInt aExpectedTimerErr, 
												TInt64 aExpectedDefaultDelay, 
												TInt64 aExpectedDelay);

protected:
	TVerdict DoTestStepL();

private:
	RTestResourceFileReaderStep(const TDesC& aStepName, 
								const TDesC& aFileName, 
								TInt aExpectedFormatErr, 
								TInt aExpectedTimerErr, 
								TInt64 aExpectedDefaultDelay, 
								TInt64 aExpectedDelay);
	TBuf<KMaxFileName> iFileName;
	TInt iExpectedFormatErr;
	TInt iExpectedTimerErr;
	TInt64 iExpectedDefaultDelay;
	TInt64 iExpectedMaxDelay;
	};

/* 
Test creating a delay by delaying set background surface in timed mode
MM-MMF-VIDREND-U-0121
*/
class RTestRendererDelayStep : public RTestRendererUpdateAndDestroyStep
	{
public:
	static RTestRendererDelayStep* NewL(const TDesC& aStepName);
	void FsmL(TTestRendererEvents aEventCode);

protected:
	RTestRendererDelayStep(const TDesC& aStepName);
	};

#endif // TESTRENDERERNEG_H
