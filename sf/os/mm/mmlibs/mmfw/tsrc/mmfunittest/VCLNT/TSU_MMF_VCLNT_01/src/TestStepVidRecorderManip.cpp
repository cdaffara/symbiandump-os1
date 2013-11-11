// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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

// EPOC includes
#include <e32base.h>
#include <e32test.h>
#include <e32keys.h>
#include <c32comm.h>
#include <f32file.h>
#include <etel.h>
#include <etelmm.h>
#include <testframework.h>
#include <badesca.h>
#include <mmf/common/mmfstandardcustomcommands.h>  
#include <mmf/common/mmfvideo.h>
#include <mmf/server/mmfdes.h>
#include <mmf/server/mmffile.h>

#include <mmf/common/mmfcontroller.h>

#include <videorecorder.h>

// Specific includes for this test suite
#include "TestStepUnitMMFVidClient.h"
#include "TestSuiteUnitMMFVidClient.h"

// Specific includes for these test steps
#include "TestStepVidRecorder.h"
#include "Ts_cmmfvideotestcontroller.h"
#include "TS_Codes.h"

_LIT(KClockFileName,"C:\\mm\\mmf\\testfiles\\vclnt\\clock.dummy");
_LIT(KVideoTestControllerName, "CMMFVideoTestController");
//const TUid KVideoTestControllerUid = { KMmfVideoTestControllerUid };	//(0x101F88D9)	// EABI warning removal

const TInt KBufMimeTypeGranularity = 4;
//
// --------------------------------------------
//
/**
 *
 * Static constructor for CTestStepVidRecorderRecord.
 *
 *
 * @return	"CTestStepVidRecorderRecord*"
 *			The constructed CTestStepVidRecorderRecord
 *
 * @xxxx
 * 
 */
CTestStepVidRecorderRecord* CTestStepVidRecorderRecord::NewL()
	{
	CTestStepVidRecorderRecord* self = new(ELeave) CTestStepVidRecorderRecord;
	return self;
	}

/**
 *
 * Test step constructor.
 * Each test step initialises its own name.
 *
 * @xxxx
 * 
 */
CTestStepVidRecorderRecord::CTestStepVidRecorderRecord() : iHasRecorded(EFalse)
	{
	iTestStepResult = EPass;
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-VCLNT-U-0220-CP");
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 * 
 */
CTestStepVidRecorderRecord::~CTestStepVidRecorderRecord()
	{
	}

/**
 * Implementation of the MVideoRecorderUtilityObserver interface functions
 **/
void CTestStepVidRecorderRecord::MvruoOpenComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); // check if asynchronous
	}

void CTestStepVidRecorderRecord::MvruoPrepareComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); // check if asynchronous
	}

void CTestStepVidRecorderRecord::MvruoRecordComplete(TInt aError)
	{
	if (iError==KErrNone)
		iHasRecorded = ETrue;
	iError = aError;
	CActiveScheduler::Stop(); // check if asynchronous
	}
	
void CTestStepVidRecorderRecord::MvruoEvent(const TMMFEvent& /*aEvent*/)
	{
	}


/**
 *
 * Test step Preamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepVidRecorderRecord::DoTestStepPreambleL(void)
	{
	iFileName.Set(KClockFileName);
	enum TVerdict verdict;
	// this installs the scheduler
	verdict = CTestStepUnitMMFVidClient::DoTestStepPreambleL();

	// Printing to the console and log file
	INFO_PRINTF1(_L("MM-MMF-VCLNT-U-0220-CP"));
	INFO_PRINTF1(_L("this test is recording a video with a CVideoRecorderUtility::RecordL()"));

	return verdict;

	}

/**
 *
 * Test step Postamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepVidRecorderRecord::DoTestStepPostambleL(void)
	{
	delete iRecorder;
	iRecorder = NULL;
	//[ Destroy the scheduler ]
	return CTestStepUnitMMFVidClient::DoTestStepPostambleL();
	}

/**
 *
 * Do the test step.
 * Each test step must supply an implementation for DoTestStepL.
 *
 * @return	"TVerdict"
 *			The result of the test step
 *
 * @xxxx
 * 
 */
TVerdict CTestStepVidRecorderRecord::DoTestStepL()
	{
	TInt err;
	err = KErrNone;
	iTestStepResult = EFail;

	iRecorder = CVideoRecorderUtility::NewL(*this);

	if ( !iRecorder ||
		 !RecorderStateStopped() )
		return EInconclusive;

	TRAP(err, RecOpenAndStartSchedulerL() );

	if (err != KErrNone || iError != KErrNone) 
		return EInconclusive;

	RecorderPrepareAndStartScheduler();

	if (err != KErrNone || iError != KErrNone) 
		return EInconclusive;


	RecordAndStartScheduler();

	if (iError == KErrNone && iHasRecorded)
		return EPass;

	delete iRecorder;
	iRecorder = NULL;

	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}

//
// --------------------------------------------
//
/**
 *
 * Static constructor for CTestStepVidRecorderStop.
 *
 *
 * @return	"CTestStepVidRecorderStop*"
 *			The constructed CTestStepVidRecorderStop
 *
 * @xxxx
 * 
 */
CTestStepVidRecorderStop* CTestStepVidRecorderStop::NewL()
	{
	CTestStepVidRecorderStop* self = new(ELeave) CTestStepVidRecorderStop;
	return self;
	}

/**
 *
 * Test step constructor.
 * Each test step initialises its own name.
 *
 * @xxxx
 * 
 */
CTestStepVidRecorderStop::CTestStepVidRecorderStop() 
	{
	iTestStepResult = EPass;
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-VCLNT-U-0221-CP");
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 * 
 */
CTestStepVidRecorderStop::~CTestStepVidRecorderStop()
	{
	}

/**
 * Implementation of the MVideoRecorderUtilityObserver interface functions
 **/
void CTestStepVidRecorderStop::MvruoOpenComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); // check if asynchronous
	}

void CTestStepVidRecorderStop::MvruoPrepareComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); // check if asynchronous
	}

void CTestStepVidRecorderStop::MvruoRecordComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); // check if asynchronous
	}
	
void CTestStepVidRecorderStop::MvruoEvent(const TMMFEvent& /*aEvent*/)
	{
	}


/**
 *
 * Test step Preamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepVidRecorderStop::DoTestStepPreambleL(void)
	{
	enum TVerdict verdict;
	// this installs the scheduler
	verdict = CTestStepUnitMMFVidClient::DoTestStepPreambleL();
	if (verdict != EPass)
		return EInconclusive;

	iFileName.Set(KClockFileName);

	// Printing to the console and log file
	INFO_PRINTF1(_L("MM-MMF-VCLNT-U-0221-CP"));
	INFO_PRINTF1(_L("this test stops Recording a video with CVideoRecorderUtility::StopL()"));

	iError = KErrNone;
	return verdict;
	}

/**
 *
 * Test step Postamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepVidRecorderStop::DoTestStepPostambleL(void)
	{
	delete iRecorder;
	iRecorder = NULL;
	//[ Destroy the scheduler ]
	return CTestStepUnitMMFVidClient::DoTestStepPostambleL();
	}

/**
 *
 * Do the test step.
 * Each test step must supply an implementation for DoTestStepL.
 *
 * @return	"TVerdict"
 *			The result of the test step
 *
 * @xxxx
 * 
 */
TVerdict CTestStepVidRecorderStop::DoTestStepL()
	{
	TInt err = KErrNone;
	iTestStepResult = EPass;

	iRecorder = CVideoRecorderUtility::NewL(*this);

	if (!iRecorder || !RecorderStateStopped())
		return EInconclusive;

	TRAP(err, RecOpenAndStartSchedulerL() );

#ifdef _TYPHOON_TEST_
	//Typhoon fix, video is not supported
	if (err == KErrNotSupported)
		return EPass;
	else 
		return EFail;
#endif

#ifndef _TYPHOON_TEST_
	if (err != KErrNone) 
		return EInconclusive;
#endif

	RecorderPrepareAndStartScheduler();

	if (err != KErrNone || iError != KErrNone) 
		return EInconclusive;

	RecordAndStartScheduler();

	if (err != KErrNone || iError != KErrNone) 
		return EInconclusive;


	err = iRecorder->Stop();

	if (err != KErrNone)
		return EFail;

	if (!RecorderStateStopped())
		 return EFail;

	delete iRecorder;
	iRecorder = NULL;


	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}

//
// --------------------------------------------
//
/**
 *
 * Static constructor for CTestStepVidRecorderPause.
 *
 *
 * @return	"CTestStepVidRecorderPause*"
 *			The constructed CTestStepVidRecorderPause
 *
 * @xxxx
 * 
 */
CTestStepVidRecorderPause* CTestStepVidRecorderPause::NewL()
	{
	CTestStepVidRecorderPause* self = new(ELeave) CTestStepVidRecorderPause;
	return self;
	}

/**
 *
 * Test step constructor.
 * Each test step initialises its own name.
 *
 * @xxxx
 * 
 */
CTestStepVidRecorderPause::CTestStepVidRecorderPause() 
	{
	iTestStepResult = EPass;
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-VCLNT-U-0222-CP");
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 * 
 */
CTestStepVidRecorderPause::~CTestStepVidRecorderPause()
	{
	}

/**
 * Implementation of the MVideoRecorderUtilityObserver interface functions
 **/
void CTestStepVidRecorderPause::MvruoOpenComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); // check if asynchronous
	}

void CTestStepVidRecorderPause::MvruoPrepareComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); // check if asynchronous
	}

void CTestStepVidRecorderPause::MvruoRecordComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); // check if asynchronous
	}
	
void CTestStepVidRecorderPause::MvruoEvent(const TMMFEvent& /*aEvent*/)
	{
	}



/**
 *
 * Test step Preamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepVidRecorderPause::DoTestStepPreambleL(void)
	{
	enum TVerdict verdict;
	// this installs the scheduler
	verdict = CTestStepUnitMMFVidClient::DoTestStepPreambleL();
	if (verdict!=EPass)
		return EInconclusive;

	iFileName.Set(KClockFileName);

	// Printing to the console and log file
	INFO_PRINTF1(_L("MM-MMF-VCLNT-U-0222-CP"));
	INFO_PRINTF1(_L("this test pauses a Recording video with a CVideoRecorderUtility::PauseL()"));

	iError = KErrNone; //KErrTimedOut;

	
	return verdict;
	}

/**
 *
 * Test step Postamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepVidRecorderPause::DoTestStepPostambleL(void)
	{
	delete iRecorder;
	iRecorder = NULL;
	//[ Destroy the scheduler ]
	return CTestStepUnitMMFVidClient::DoTestStepPostambleL();
	}

/**
 *
 * Do the test step.
 * Each test step must supply an implementation for DoTestStepL.
 *
 * @return	"TVerdict"
 *			The result of the test step
 *
 * @xxxx
 * 
 */
TVerdict CTestStepVidRecorderPause::DoTestStepL()
	{
	TInt err = KErrNone;
	iTestStepResult = EPass;


	iRecorder = CVideoRecorderUtility::NewL(*this);

	if (!RecorderStateStopped())
		 return EInconclusive;

	TRAP(err, RecOpenAndStartSchedulerL() );

#ifdef _TYPHOON_TEST_
	//Typhoon fix, video is not supported
	if (err == KErrNotSupported)
		return EPass;
	else
		return EInconclusive;
#endif

#ifndef _TYPHOON_TEST_
	if (err != KErrNone ) 
		return EInconclusive;
#endif

	RecorderPrepareAndStartScheduler();

	if (err != KErrNone || iError != KErrNone) 
		return EInconclusive;

	RecordAndStartScheduler();

	if (err != KErrNone || iError != KErrNone) 
		return EInconclusive;

	TRAP(err, iRecorder->PauseL() );

	if ( !RecorderStatePaused())
		 return EFail;

	delete iRecorder;
	iRecorder = NULL;

	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}



//
// --------------------------------------------
//
/**
 *
 * Static constructor for CTestStepVidRecorderPriority.
 *
 *
 * @return	"CTestStepVidRecorderPriority*"
 *			The constructed CTestStepVidRecorderPriority
 *
 * @xxxx
 * 
 */
CTestStepVidRecorderPriority* CTestStepVidRecorderPriority::NewL()
	{
	CTestStepVidRecorderPriority* self = new(ELeave) CTestStepVidRecorderPriority;
	return self;
	}

/**
 *
 * Test step constructor.
 * Each test step initialises its own name.
 *
 * @xxxx
 * 
 */
CTestStepVidRecorderPriority::CTestStepVidRecorderPriority() 
	{
	iTestStepResult = EPass;
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-VCLNT-U-0223-CP");
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 * 
 */
CTestStepVidRecorderPriority::~CTestStepVidRecorderPriority()
	{
	}

/**
 * Implementation of the MVideoRecorderUtilityObserver interface functions
 **/
void CTestStepVidRecorderPriority::MvruoOpenComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); // check if asynchronous
	}

void CTestStepVidRecorderPriority::MvruoPrepareComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); // check if asynchronous
	}

void CTestStepVidRecorderPriority::MvruoRecordComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); // check if asynchronous
	}
	
void CTestStepVidRecorderPriority::MvruoEvent(const TMMFEvent& /*aEvent*/)
	{
	}


/**
 *
 * Test step Preamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepVidRecorderPriority::DoTestStepPreambleL(void)
	{
	enum TVerdict verdict;
	// this installs the scheduler
	verdict = CTestStepUnitMMFVidClient::DoTestStepPreambleL();
	if (verdict != EPass)
		return verdict;

	iFileName.Set(KClockFileName);

	// Printing to the console and log file
	INFO_PRINTF1(_L("MM-MMF-VCLNT-U-0223-CP"));
	INFO_PRINTF1(_L("this test retrieves the priority setting with CVideoRecorderUtility::PriorityL()"));

	iRecorder = CVideoRecorderUtility::NewL(*this);

	if (iRecorder == NULL ||
				!RecorderStateStopped() )
				return EInconclusive;

	return verdict;

	}

/**
 *
 * Test step Postamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepVidRecorderPriority::DoTestStepPostambleL(void)
	{
	delete iRecorder;
	iRecorder = NULL;
	//[ Destroy the scheduler ]
	return CTestStepUnitMMFVidClient::DoTestStepPostambleL();
	}

/**
 *
 * Do the test step.
 * Each test step must supply an implementation for DoTestStepL.
 *
 * @return	"TVerdict"
 *			The result of the test step
 *
 * @xxxx
 * 
 */
TVerdict CTestStepVidRecorderPriority::DoTestStepL()
	{
	iTestStepResult = EPass;
//	TInt err;
//	err = KErrNone;	// EABI warning removal

	// do the priority test. TBD

	delete iRecorder;
	iRecorder = NULL;

	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}

//
// --------------------------------------------
//
/**
 *
 * Static constructor for CTestStepVidRecorderVFrameRate.
 *
 *
 * @return	"CTestStepVidRecorderVFrameRate*"
 *			The constructed CTestStepVidRecorderVFrameRate
 *
 * @xxxx
 * 
 */
CTestStepVidRecorderVFrameRate* CTestStepVidRecorderVFrameRate::NewL()
	{
	CTestStepVidRecorderVFrameRate* self = new(ELeave) CTestStepVidRecorderVFrameRate;
	return self;
	}

/**
 *
 * Test step constructor.
 * Each test step initialises its own name.
 *
 * @xxxx
 * 
 */
CTestStepVidRecorderVFrameRate::CTestStepVidRecorderVFrameRate() 
	{
	iTestStepResult = EPass;
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-VCLNT-U-0225-HP");
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 * 
 */
CTestStepVidRecorderVFrameRate::~CTestStepVidRecorderVFrameRate()
	{
	}

/**
 * Implementation of the MVideoRecorderUtilityObserver interface functions
 **/
void CTestStepVidRecorderVFrameRate::MvruoOpenComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); // check if asynchronous
	}

void CTestStepVidRecorderVFrameRate::MvruoPrepareComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); // check if asynchronous
	}

void CTestStepVidRecorderVFrameRate::MvruoRecordComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); // check if asynchronous
	}
	
void CTestStepVidRecorderVFrameRate::MvruoEvent(const TMMFEvent& /*aEvent*/)
	{
	}


/**
 *
 * Test step Preamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepVidRecorderVFrameRate::DoTestStepPreambleL(void)
	{
	enum TVerdict verdict;
	// this installs the scheduler
	verdict = CTestStepUnitMMFVidClient::DoTestStepPreambleL();
	if (verdict != EPass)
		return verdict;

	iFileName.Set(KClockFileName);

	// Printing to the console and log file
	INFO_PRINTF1(_L("MM-MMF-VCLNT-U-0225-HP"));
	INFO_PRINTF1(_L("this test sets/gets the frame rate with CVideoRecorderUtility::[Set]VideoFrameRateL()"));

	return verdict;

	}

/**
 *
 * Test step Postamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepVidRecorderVFrameRate::DoTestStepPostambleL(void)
	{
	delete iRecorder;
	iRecorder = NULL;
	//[ Destroy the scheduler ]
	return CTestStepUnitMMFVidClient::DoTestStepPostambleL();
	}

/**
 *
 * Do the test step.
 * Each test step must supply an implementation for DoTestStepL.
 *
 * @return	"TVerdict"
 *			The result of the test step
 *
 * @xxxx
 * 
 */
TVerdict CTestStepVidRecorderVFrameRate::DoTestStepL()
	{
	iTestStepResult = EPass;
	TInt err = KErrNone;
	TReal32 frameRate = 0;

	iRecorder = CVideoRecorderUtility::NewL(*this);

	if (!iRecorder ||!RecorderStateStopped() )
				return EInconclusive;

	TRAP(err, RecOpenAndStartSchedulerL() );

	if (err != KErrNone) 
		return EInconclusive;

	TRAP(err, frameRate = iRecorder->VideoFrameRateL() );

	if (err != KErrNone ||frameRate < 0 )  // or check with a known frame rate clip when there is a video controller and decoder available
		return EFail;

	frameRate = 11.5;
	TRAP(err, iRecorder->SetVideoFrameRateL(frameRate) );

	if (err != KErrNone) 
		return EFail;

	TReal32 refFrameRate = 0;
	TRAP(err, refFrameRate = iRecorder->VideoFrameRateL() );

	if (err != KErrNone ||
		frameRate != refFrameRate ) 
		return EFail;

	delete iRecorder;
	iRecorder = NULL;

	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}

//
// --------------------------------------------
//
/**
 *
 * Static constructor for CTestStepVidRecorderVFrameSize.
 *
 *
 * @return	"CTestStepVidRecorderVFrameSize*"
 *			The constructed CTestStepVidRecorderVFrameSize
 *
 * @xxxx
 * 
 */
CTestStepVidRecorderVFrameSize* CTestStepVidRecorderVFrameSize::NewL()
	{
	CTestStepVidRecorderVFrameSize* self = new(ELeave) CTestStepVidRecorderVFrameSize;
	return self;
	}

/**
 *
 * Test step constructor.
 * Each test step initialises its own name.
 *
 * @xxxx
 * 
 */
CTestStepVidRecorderVFrameSize::CTestStepVidRecorderVFrameSize() 
	{
	iTestStepResult = EPass;
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-VCLNT-U-0229-HP");
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 * 
 */
CTestStepVidRecorderVFrameSize::~CTestStepVidRecorderVFrameSize()
	{
	}

/**
 * Implementation of the MVideoRecorderUtilityObserver interface functions
 **/
void CTestStepVidRecorderVFrameSize::MvruoOpenComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); // check if asynchronous
	}

void CTestStepVidRecorderVFrameSize::MvruoPrepareComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); // check if asynchronous
	}

void CTestStepVidRecorderVFrameSize::MvruoRecordComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); // check if asynchronous
	}
	
void CTestStepVidRecorderVFrameSize::MvruoEvent(const TMMFEvent& /*aEvent*/)
	{
	}



/**
 *
 * Test step Preamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepVidRecorderVFrameSize::DoTestStepPreambleL(void)
	{
	enum TVerdict verdict;
	// this installs the scheduler
	verdict = CTestStepUnitMMFVidClient::DoTestStepPreambleL();
	if (verdict != EPass)
		return verdict;

	iFileName.Set(KClockFileName);

	// Printing to the console and log file
	INFO_PRINTF1(_L("MM-MMF-VCLNT-U-0229-HP"));
	INFO_PRINTF1(_L("this test gets the video frame size with CVideoRecorderUtility::VideoFrameSizeL()"));

	return verdict;

	}

/**
 *
 * Test step Postamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepVidRecorderVFrameSize::DoTestStepPostambleL(void)
	{
	delete iRecorder;
	iRecorder = NULL;
	//[ Destroy the scheduler ]
	return CTestStepUnitMMFVidClient::DoTestStepPostambleL();
	}

/**
 *
 * Do the test step.
 * Each test step must supply an implementation for DoTestStepL.
 *
 * @return	"TVerdict"
 *			The result of the test step
 *
 * @xxxx
 * 
 */
TVerdict CTestStepVidRecorderVFrameSize::DoTestStepL()
	{
	iTestStepResult = EPass;
	TInt err = KErrNone;
	TSize frameSize;

	iRecorder = CVideoRecorderUtility::NewL(*this);

	if (iRecorder == NULL ||
				!RecorderStateStopped() )
				return EInconclusive;

	TRAP(err, RecOpenAndStartSchedulerL() );

	if (err != KErrNone) 
		return EInconclusive;


	TRAP(err, iRecorder->GetVideoFrameSizeL(frameSize) );

	if (err != KErrNone ||
		frameSize.iWidth < 0 ||
		frameSize.iHeight < 0 ) 
		return EFail;

	frameSize.iWidth = 80;
	frameSize.iHeight = 60; 
	TRAP(err, iRecorder->SetVideoFrameSizeL(frameSize) );


	if (err != KErrNone) 
		return EFail;

	TRAP(err, iRecorder->GetVideoFrameSizeL(frameSize) );

	if (err != KErrNone ||
		frameSize.iWidth != 80 ||
		frameSize.iHeight != 60 ) 
		return EFail;

	delete iRecorder;
	iRecorder = NULL;

	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}

//
// --------------------------------------------
//
/**
 *
 * Static constructor for CTestStepVidRecorderVBitRate.
 *
 *
 * @return	"CTestStepVidRecorderVBitRate*"
 *			The constructed CTestStepVidRecorderVBitRate
 *
 * @xxxx
 * 
 */
CTestStepVidRecorderVBitRate* CTestStepVidRecorderVBitRate::NewL()
	{
	CTestStepVidRecorderVBitRate* self = new(ELeave) CTestStepVidRecorderVBitRate;
	return self;
	}

/**
 *
 * Test step constructor.
 * Each test step initialises its own name.
 *
 * @xxxx
 * 
 */
CTestStepVidRecorderVBitRate::CTestStepVidRecorderVBitRate() 
	{
	iTestStepResult = EPass;
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-VCLNT-U-0230-HP");
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 * 
 */
CTestStepVidRecorderVBitRate::~CTestStepVidRecorderVBitRate()
	{
	}

/**
 * Implementation of the MVideoRecorderUtilityObserver interface functions
 **/
void CTestStepVidRecorderVBitRate::MvruoOpenComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); // check if asynchronous
	}

void CTestStepVidRecorderVBitRate::MvruoPrepareComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); // check if asynchronous
	}

void CTestStepVidRecorderVBitRate::MvruoRecordComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); // check if asynchronous
	}
	
void CTestStepVidRecorderVBitRate::MvruoEvent(const TMMFEvent& /*aEvent*/)
	{
	}


/**
 *
 * Test step Preamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepVidRecorderVBitRate::DoTestStepPreambleL(void)
	{
	enum TVerdict verdict;
	// this installs the scheduler
	verdict = CTestStepUnitMMFVidClient::DoTestStepPreambleL();
	if (verdict != EPass)
		return verdict;

	TInt err = KErrNone;

	iFileName.Set(KClockFileName);

	// Printing to the console and log file
	INFO_PRINTF1(_L("MM-MMF-VCLNT-U-0230-HP"));
	INFO_PRINTF1(_L("this test gets the video bit rate with CVideoRecorderUtility::VideoBitRateL()"));

	iRecorder = CVideoRecorderUtility::NewL(*this);

	if (iRecorder == NULL ||
				!RecorderStateStopped() )
				return EInconclusive;

	TRAP(err, RecOpenAndStartSchedulerL() );

#ifdef _TYPHOON_TEST_
	//Typhoon fix, video is not supported
	if (err != KErrNotSupported)
		return EInconclusive;
	else 
		return EPass;
#endif

	if (err != KErrNone) 
		return EInconclusive;

	return verdict;
	}

/**
 *
 * Test step Postamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepVidRecorderVBitRate::DoTestStepPostambleL(void)
	{
	delete iRecorder;
	iRecorder = NULL;
	//[ Destroy the scheduler ]
	return CTestStepUnitMMFVidClient::DoTestStepPostambleL();
	}

/**
 *
 * Do the test step.
 * Each test step must supply an implementation for DoTestStepL.
 *
 * @return	"TVerdict"
 *			The result of the test step
 *
 * @xxxx
 * 
 */
TVerdict CTestStepVidRecorderVBitRate::DoTestStepL()
	{
	iTestStepResult = EPass;
	TInt err = KErrNone;
	TInt bitRate = 0; 

	TRAP(err, bitRate = iRecorder->VideoBitRateL() );

#ifdef _TYPHOON_TEST_
	//Typhoon fix, video is not supported
	if (err != KErrNotSupported)
		return EFail;
#endif

#ifndef _TYPHOON_TEST_
	if (err != KErrNone ||
		bitRate < 0 )  
		return EFail;
#endif

	TRAP(err, iRecorder->SetVideoBitRateL(100) );

#ifdef _TYPHOON_TEST_
	//Typhoon fix, video is not supported
	if (err != KErrNotSupported)
		return EFail;
#endif

#ifndef _TYPHOON_TEST_
	if (err != KErrNone)  
		return EFail;
#endif

	delete iRecorder;
	iRecorder = NULL;

	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}

//
// --------------------------------------------
//
/**
 *
 * Static constructor for CTestStepVidRecorderABitRate.
 *
 *
 * @return	"CTestStepVidRecorderABitRate*"
 *			The constructed CTestStepVidRecorderABitRate
 *
 * @xxxx
 * 
 */
CTestStepVidRecorderABitRate* CTestStepVidRecorderABitRate::NewL()
	{
	CTestStepVidRecorderABitRate* self = new(ELeave) CTestStepVidRecorderABitRate;
	return self;
	}

/**
 *
 * Test step constructor.
 * Each test step initialises its own name.
 *
 * @xxxx
 * 
 */
CTestStepVidRecorderABitRate::CTestStepVidRecorderABitRate() 
	{
	iTestStepResult = EPass;
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-VCLNT-U-0231-HP");
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 * 
 */
CTestStepVidRecorderABitRate::~CTestStepVidRecorderABitRate()
	{
	}

/**
 * Implementation of the MVideoRecorderUtilityObserver interface functions
 **/
void CTestStepVidRecorderABitRate::MvruoOpenComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); // check if asynchronous
	}

void CTestStepVidRecorderABitRate::MvruoPrepareComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); // check if asynchronous
	}

void CTestStepVidRecorderABitRate::MvruoRecordComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); // check if asynchronous
	}
	
void CTestStepVidRecorderABitRate::MvruoEvent(const TMMFEvent& /*aEvent*/)
	{
	}



/**
 *
 * Test step Preamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepVidRecorderABitRate::DoTestStepPreambleL(void)
	{
	enum TVerdict verdict;
	// this installs the scheduler
	verdict = CTestStepUnitMMFVidClient::DoTestStepPreambleL();
	if (verdict != EPass)
		return verdict;

	TInt err = KErrNone;

	iFileName.Set(KClockFileName);

	// Printing to the console and log file
	INFO_PRINTF1(_L("MM-MMF-VCLNT-U-0231-HP"));
	INFO_PRINTF1(_L("this test gets the audio bit rate with CVideoRecorderUtility::AudioBitRateL()"));

	iRecorder = CVideoRecorderUtility::NewL(*this);

	if (iRecorder == NULL ||
				!RecorderStateStopped() )
				return EInconclusive;

	TRAP(err, RecOpenAndStartSchedulerL() );

#ifdef _TYPHOON_TEST_
	//Typhoon fix, video is not supported
	if (err != KErrNotSupported)
		return EInconclusive;
	else 
		return EPass;
#endif

	if (err != KErrNone) 
		return EInconclusive;

	return verdict;
	}

/**
 *
 * Test step Postamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepVidRecorderABitRate::DoTestStepPostambleL(void)
	{
	delete iRecorder;
	iRecorder = NULL;
	//[ Destroy the scheduler ]
	return CTestStepUnitMMFVidClient::DoTestStepPostambleL();
	}

/**
 *
 * Do the test step.
 * Each test step must supply an implementation for DoTestStepL.
 *
 * @return	"TVerdict"
 *			The result of the test step
 *
 * @xxxx
 * 
 */
TVerdict CTestStepVidRecorderABitRate::DoTestStepL()
	{
	iTestStepResult = EPass;
	TInt err = KErrNone;
	TInt bitRate = 0; 

	TRAP(err, bitRate = iRecorder->AudioBitRateL() );

#ifdef _TYPHOON_TEST_
	//Typhoon fix, video is not supported
	if (err != KErrNotSupported)
		return EFail;
#endif

#ifndef _TYPHOON_TEST_
	if (err != KErrNone ||
		bitRate < 0 )  
		return EFail;
#endif

	TRAP(err, iRecorder->SetAudioBitRateL(100) );

#ifdef _TYPHOON_TEST_
	//Typhoon fix, video is not supported
	if (err != KErrNotSupported)
		return EFail;
#endif

#ifndef _TYPHOON_TEST_
	if (err != KErrNone)  
		return EFail;
#endif

	delete iRecorder;
	iRecorder = NULL;

	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}

//
// --------------------------------------------
//
/**
 *
 * Static constructor for CTestStepVidRecorderAudEnabled.
 *
 *
 * @return	"CTestStepVidRecorderAudEnabled*"
 *			The constructed CTestStepVidRecorderAudEnabled
 *
 * @xxxx
 * 
 */
CTestStepVidRecorderAudEnabled* CTestStepVidRecorderAudEnabled::NewL()
	{
	CTestStepVidRecorderAudEnabled* self = new(ELeave) CTestStepVidRecorderAudEnabled;
	return self;
	}

/**
 *
 * Test step constructor.
 * Each test step initialises its own name.
 *
 * @xxxx
 * 
 */
CTestStepVidRecorderAudEnabled::CTestStepVidRecorderAudEnabled() 
	{
	iTestStepResult = EPass;
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-VCLNT-U-0232-HP");
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 * 
 */
CTestStepVidRecorderAudEnabled::~CTestStepVidRecorderAudEnabled()
	{
	}

/**
 * Implementation of the MVideoRecorderUtilityObserver interface functions
 **/
void CTestStepVidRecorderAudEnabled::MvruoOpenComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); // check if asynchronous
	}

void CTestStepVidRecorderAudEnabled::MvruoPrepareComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); // check if asynchronous
	}

void CTestStepVidRecorderAudEnabled::MvruoRecordComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); // check if asynchronous
	}
	
void CTestStepVidRecorderAudEnabled::MvruoEvent(const TMMFEvent& /*aEvent*/)
	{
	}



/**
 *
 * Test step Preamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepVidRecorderAudEnabled::DoTestStepPreambleL(void)
	{
	enum TVerdict verdict;
	// this installs the scheduler
	verdict = CTestStepUnitMMFVidClient::DoTestStepPreambleL();
	if (verdict != EPass)
		return verdict;

	iFileName.Set(KClockFileName);

	// Printing to the console and log file
	INFO_PRINTF1(_L("MM-MMF-VCLNT-U-0232-HP"));
	INFO_PRINTF1(_L("this test checks if the audio is enabled with CVideoRecorderUtility::AudioEnabledL()"));

	return verdict;
	}

/**
 *
 * Test step Postamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepVidRecorderAudEnabled::DoTestStepPostambleL(void)
	{
	delete iRecorder;
	iRecorder = NULL;
	//[ Destroy the scheduler ]
	return CTestStepUnitMMFVidClient::DoTestStepPostambleL();
	}

/**
 *
 * Do the test step.
 * Each test step must supply an implementation for DoTestStepL.
 *
 * @return	"TVerdict"
 *			The result of the test step
 *
 * @xxxx
 * 
 */
TVerdict CTestStepVidRecorderAudEnabled::DoTestStepL()
	{
	iTestStepResult = EPass;
	TInt err = KErrNone;
//	TBool audEnabled;

	iRecorder = CVideoRecorderUtility::NewL(*this);

	if (iRecorder == NULL ||
				!RecorderStateStopped() )
				return EInconclusive;

	TRAP(err, RecOpenAndStartSchedulerL() );

	if (err != KErrNone) 
		return EInconclusive;

//	TRAP(err, audEnabled = iRecorder->AudioEnabledL() );
	TRAP(err, iRecorder->AudioEnabledL() );	// EABI warning removal

	if (err != KErrNone )
		return EFail;

	delete iRecorder;
	iRecorder = NULL;

	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}

//
// --------------------------------------------
//
/**
 *
 * Static constructor for CTestStepVidRecorderRecordTime.
 *
 *
 * @return	"CTestStepVidRecorderRecordTime*"
 *			The constructed CTestStepVidRecorderRecordTime
 *
 * @xxxx
 * 
 */
CTestStepVidRecorderRecordTime* CTestStepVidRecorderRecordTime::NewL()
	{
	CTestStepVidRecorderRecordTime* self = new(ELeave) CTestStepVidRecorderRecordTime;
	return self;
	}

/**
 *
 * Test step constructor.
 * Each test step initialises its own name.
 *
 * @xxxx
 * 
 */
CTestStepVidRecorderRecordTime::CTestStepVidRecorderRecordTime() 
	{
	iTestStepResult = EPass;
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-VCLNT-U-0241-CP");
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 * 
 */
CTestStepVidRecorderRecordTime::~CTestStepVidRecorderRecordTime()
	{
	}

/**
 * Implementation of the MVideoRecorderUtilityObserver interface functions
 **/
void CTestStepVidRecorderRecordTime::MvruoOpenComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); // check if asynchronous
	}

void CTestStepVidRecorderRecordTime::MvruoPrepareComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); // check if asynchronous
	}

void CTestStepVidRecorderRecordTime::MvruoRecordComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); // check if asynchronous
	}
	
void CTestStepVidRecorderRecordTime::MvruoEvent(const TMMFEvent& /*aEvent*/)
	{
	}


/**
 *
 * Test step Preamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepVidRecorderRecordTime::DoTestStepPreambleL(void)
	{
	enum TVerdict verdict;
	// this installs the scheduler
	verdict = CTestStepUnitMMFVidClient::DoTestStepPreambleL();
	if (verdict != EPass)
		return verdict;

	iFileName.Set(KClockFileName);

	// Printing to the console and log file
	INFO_PRINTF1(_L("MM-MMF-VCLNT-U-0241-CP"));
	INFO_PRINTF1(_L("this test gets the record time available using CVideoRecorderUtility::GetRecordTimeAvailable()"));

	return verdict;
	}

/**
 *
 * Test step Postamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepVidRecorderRecordTime::DoTestStepPostambleL(void)
	{
	delete iRecorder;
	iRecorder = NULL;
	//[ Destroy the scheduler ]
	return CTestStepUnitMMFVidClient::DoTestStepPostambleL();
	}

/**
 *
 * Do the test step.
 * Each test step must supply an implementation for DoTestStepL.
 *
 * @return	"TVerdict"
 *			The result of the test step
 *
 * @xxxx
 * 
 */
TVerdict CTestStepVidRecorderRecordTime::DoTestStepL()
	{
	iTestStepResult = EPass;
	TInt err = KErrNone;
	TTimeIntervalMicroSeconds recordTime;

	iRecorder = CVideoRecorderUtility::NewL(*this);

	if (!iRecorder || !RecorderStateStopped())
		 return EInconclusive;

	TRAP(err, RecOpenAndStartSchedulerL() );

#ifdef _TYPHOON_TEST_
	//Typhoon fix, video is not supported
	if (err != KErrNotSupported)
		return EPass;
#else
	if (err != KErrNone)
		return EInconclusive;
#endif

	TRAP(err, recordTime = iRecorder->RecordTimeAvailable() );

#ifdef _TYPHOON_TEST_
	//Typhoon fix, video is not supported
	if (err != KErrNotSupported)
		{
		return EFail;
		}
#else
	TTimeIntervalMicroSeconds refTime (KMMFVideoRecordTimeAvailable);
	if (err != KErrNone || recordTime != refTime)
		return EFail;
#endif

	delete iRecorder;
	iRecorder = NULL;

	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}


//
// --------------------------------------------
//
/**
 *
 * Static constructor for CTestStepVidRecorderDuration.
 *
 *
 * @return	"CTestStepVidRecorderDuration*"
 *			The constructed CTestStepVidRecorderDuration
 *
 * @xxxx
 * 
 */
CTestStepVidRecorderDuration* CTestStepVidRecorderDuration::NewL()
	{
	CTestStepVidRecorderDuration* self = new(ELeave) CTestStepVidRecorderDuration;
	return self;
	}

/**
 *
 * Test step constructor.
 * Each test step initialises its own name.
 *
 * @xxxx
 * 
 */
CTestStepVidRecorderDuration::CTestStepVidRecorderDuration() 
	{
	iTestStepResult = EPass;
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-VCLNT-U-0234-CP");
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 * 
 */
CTestStepVidRecorderDuration::~CTestStepVidRecorderDuration()
	{
	}

/**
 * Implementation of the MVideoRecorderUtilityObserver interface functions
 **/
void CTestStepVidRecorderDuration::MvruoOpenComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); // check if asynchronous
	}

void CTestStepVidRecorderDuration::MvruoPrepareComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); // check if asynchronous
	}

void CTestStepVidRecorderDuration::MvruoRecordComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); // check if asynchronous
	}
	
void CTestStepVidRecorderDuration::MvruoEvent(const TMMFEvent& /*aEvent*/)
	{
	}


/**
 *
 * Test step Preamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepVidRecorderDuration::DoTestStepPreambleL(void)
	{
	enum TVerdict verdict;
	// this installs the scheduler
	verdict = CTestStepUnitMMFVidClient::DoTestStepPreambleL();
	if (verdict != EPass)
		return verdict;

	iFileName.Set(KClockFileName);

	// Printing to the console and log file
	INFO_PRINTF1(_L("MM-MMF-VCLNT-U-0234-CP"));
	INFO_PRINTF1(_L("this test gets the duration of a video clip with CVideoRecorderUtility::DurationL()"));

	return verdict;
	}

/**
 *
 * Test step Postamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepVidRecorderDuration::DoTestStepPostambleL(void)
	{
	delete iRecorder;
	iRecorder = NULL;
	//[ Destroy the scheduler ]
	return CTestStepUnitMMFVidClient::DoTestStepPostambleL();
	}

/**
 *
 * Do the test step.
 * Each test step must supply an implementation for DoTestStepL.
 *
 * @return	"TVerdict"
 *			The result of the test step
 *
 * @xxxx
 * 
 */
TVerdict CTestStepVidRecorderDuration::DoTestStepL()
	{
	iTestStepResult = EPass;
	TInt err = KErrNone;
	TTimeIntervalMicroSeconds dur;
	TTimeIntervalMicroSeconds refDur;

	iRecorder = CVideoRecorderUtility::NewL(*this);

	if (!iRecorder || !RecorderStateStopped())
		 return EInconclusive;

	TRAP(err, RecOpenAndStartSchedulerL() );

	if (err != KErrNone || iError != KErrNone) 
		return EInconclusive;
		
	TRAP(err, dur = iRecorder->DurationL() );

	if (err != KErrNone )
		return EInconclusive;

	//added for defect INC21987
	TRAP(err, iRecorder->SetMaxClipSizeL(1000) );
	if (err != KErrNone )
		return EInconclusive;

	TRAP(err, refDur = iRecorder->DurationL() );

	if (err != KErrNone || dur != refDur)
		return EFail;

	delete iRecorder;
	iRecorder = NULL;

	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}

//
// --------------------------------------------
//
/**
 *
 * Static constructor for CTestStepVidRecorderMaxClipSize.
 *
 *
 * @return	"CTestStepVidRecorderMaxClipSize*"
 *			The constructed CTestStepVidRecorderMaxClipSize
 *
 * @xxxx
 * 
 */
CTestStepVidRecorderMaxClipSize* CTestStepVidRecorderMaxClipSize::NewL()
	{
	CTestStepVidRecorderMaxClipSize* self = new(ELeave) CTestStepVidRecorderMaxClipSize;
	return self;
	}

/**
 *
 * Test step constructor.
 * Each test step initialises its own name.
 *
 * @xxxx
 * 
 */
CTestStepVidRecorderMaxClipSize::CTestStepVidRecorderMaxClipSize() 
	{
	iTestStepResult = EPass;
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-VCLNT-U-0235-CP");
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 * 
 */
CTestStepVidRecorderMaxClipSize::~CTestStepVidRecorderMaxClipSize()
	{
	}

/**
 * Implementation of the MVideoRecorderUtilityObserver interface functions
 **/
void CTestStepVidRecorderMaxClipSize::MvruoOpenComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); // check if asynchronous
	}

void CTestStepVidRecorderMaxClipSize::MvruoPrepareComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); // check if asynchronous
	}

void CTestStepVidRecorderMaxClipSize::MvruoRecordComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); // check if asynchronous
	}
	
void CTestStepVidRecorderMaxClipSize::MvruoEvent(const TMMFEvent& /*aEvent*/)
	{
	}


/**
 *
 * Test step Preamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepVidRecorderMaxClipSize::DoTestStepPreambleL(void)
	{
	enum TVerdict verdict;
	// this installs the scheduler
	verdict = CTestStepUnitMMFVidClient::DoTestStepPreambleL();
	if (verdict != EPass)
		return verdict;

	iFileName.Set(KClockFileName);

	// Printing to the console and log file
	INFO_PRINTF1(_L("MM-MMF-VCLNT-U-0235-CP"));
	INFO_PRINTF1(_L("this test sets the maximum recording clip duration with CVideoRecorderUtility::SetMaxClipSizeL()"));

	return verdict;

	}

/**
 *
 * Test step Postamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepVidRecorderMaxClipSize::DoTestStepPostambleL(void)
	{
	delete iRecorder;
	iRecorder = NULL;
	//[ Destroy the scheduler ]
	return CTestStepUnitMMFVidClient::DoTestStepPostambleL();
	}

/**
 *
 * Do the test step.
 * Each test step must supply an implementation for DoTestStepL.
 *
 * @return	"TVerdict"
 *			The result of the test step
 *
 * @xxxx
 * 
 */
TVerdict CTestStepVidRecorderMaxClipSize::DoTestStepL()
	{
	iTestStepResult = EPass;
	TInt err = KErrNone;

	iRecorder = CVideoRecorderUtility::NewL(*this);

	if (iRecorder == NULL ||
			!RecorderStateStopped() )
			return EInconclusive;

	TRAP(err, RecOpenAndStartSchedulerL() );

	if (err != KErrNone) 
		return EInconclusive;


	TRAP(err, iRecorder->SetMaxClipSizeL(100000) );

	if (err != KErrNone) 
		return EFail;


	delete iRecorder;
	iRecorder = NULL;

	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}


//
// --------------------------------------------
//
/**
 *
 * Static constructor for CTestStepVidRecorderVideoType.
 *
 *
 * @return	"CTestStepVidRecorderVideoType*"
 *			The constructed CTestStepVidRecorderVideoType
 *
 * @xxxx
 * 
 */
CTestStepVidRecorderVideoType* CTestStepVidRecorderVideoType::NewL()
	{
	CTestStepVidRecorderVideoType* self = new(ELeave) CTestStepVidRecorderVideoType;
	return self;
	}

/**
 *
 * Test step constructor.
 * Each test step initialises its own name.
 *
 * @xxxx
 * 
 */
CTestStepVidRecorderVideoType::CTestStepVidRecorderVideoType() 
	{
	iTestStepResult = EPass;
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-VCLNT-U-0236-CP");
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 * 
 */
CTestStepVidRecorderVideoType::~CTestStepVidRecorderVideoType()
	{
	}

/**
 * Implementation of the MVideoRecorderUtilityObserver interface functions
 **/
void CTestStepVidRecorderVideoType::MvruoOpenComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); // check if asynchronous
	}

void CTestStepVidRecorderVideoType::MvruoPrepareComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); // check if asynchronous
	}

void CTestStepVidRecorderVideoType::MvruoRecordComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); // check if asynchronous
	}
	
void CTestStepVidRecorderVideoType::MvruoEvent(const TMMFEvent& /*aEvent*/)
	{
	}



/**
 *
 * Test step Preamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepVidRecorderVideoType::DoTestStepPreambleL(void)
	{
	enum TVerdict verdict;
	// this installs the scheduler
	verdict = CTestStepUnitMMFVidClient::DoTestStepPreambleL();
	if (verdict != EPass)
		return verdict;

	iFileName.Set(KClockFileName);

	// Printing to the console and log file
	INFO_PRINTF1(_L("MM-MMF-VCLNT-U-0236-CP"));
	INFO_PRINTF1(_L("this test is about the video type of a video clip: CVideoRecorderUtility::[GetSupported|Set]VideoType[s]L()"));

	return verdict;

	}

/**
 *
 * Test step Postamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepVidRecorderVideoType::DoTestStepPostambleL(void)
	{
	delete iRecorder;
	iRecorder = NULL;
	//[ Destroy the scheduler ]
	return CTestStepUnitMMFVidClient::DoTestStepPostambleL();
	}

/**
 *
 * Do the test step.
 * Each test step must supply an implementation for DoTestStepL.
 *
 * @return	"TVerdict"
 *			The result of the test step
 *
 * @xxxx
 * 
 */
TVerdict CTestStepVidRecorderVideoType::DoTestStepL()
	{
	iTestStepResult = EPass;
	TInt err = KErrNone;
	TBuf8<256> type;

	iRecorder = CVideoRecorderUtility::NewL(*this);

	if (iRecorder == NULL ||
			!RecorderStateStopped() )
			return EInconclusive;

	TRAP(err, RecOpenAndStartSchedulerL() );

	if (err != KErrNone || iError != KErrNone) 
		return EInconclusive;


	TRAP(err, type = iRecorder->VideoFormatMimeType() );

#ifdef _TYPHOON_TEST_
	//Typhoon fix, video is not supported
	if (err != KErrNotSupported )
		return EFail;
	else 
		return EPass;
#endif

	if (err != KErrNone)
		return EFail;

	// check what mime types are available
	CDesC8ArrayFlat* vidTypes = new (ELeave) CDesC8ArrayFlat(KBufMimeTypeGranularity);
	CleanupStack::PushL(vidTypes);
	TRAP(err, iRecorder->GetSupportedVideoTypesL(*vidTypes));
	INFO_PRINTF2(_L("GetSupportedVideoTypesL returned %d"), err);

	if (err != KErrNone)
		iTestStepResult = EFail;
	else
		{
		for (TInt i = 0; i < vidTypes->Count(); i++)
			{
			TBuf<64> temp;
			temp.Copy((*vidTypes)[i]);
			INFO_PRINTF2(_L("Mime Type:  %S"), &temp);	
			}
		}

	CleanupStack::PopAndDestroy(vidTypes);

	delete iRecorder;
	iRecorder = NULL;

	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}

//
// --------------------------------------------
//
/**
 *
 * Static constructor for CTestStepVidRecorderAudioType.
 *
 *
 * @return	"CTestStepVidRecorderAudioType*"
 *			The constructed CTestStepVidRecorderAudioType
 *
 * @xxxx
 * 
 */
CTestStepVidRecorderAudioType* CTestStepVidRecorderAudioType::NewL()
	{
	CTestStepVidRecorderAudioType* self = new(ELeave) CTestStepVidRecorderAudioType;
	return self;
	}

/**
 *
 * Test step constructor.
 * Each test step initialises its own name.
 *
 * @xxxx
 * 
 */
CTestStepVidRecorderAudioType::CTestStepVidRecorderAudioType() 
	{
	iTestStepResult = EPass;
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-VCLNT-U-0237-CP");
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 * 
 */
CTestStepVidRecorderAudioType::~CTestStepVidRecorderAudioType()
	{
	}

/**
 * Implementation of the MVideoRecorderUtilityObserver interface functions
 **/
void CTestStepVidRecorderAudioType::MvruoOpenComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); // check if asynchronous
	}

void CTestStepVidRecorderAudioType::MvruoPrepareComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); // check if asynchronous
	}

void CTestStepVidRecorderAudioType::MvruoRecordComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); // check if asynchronous
	}
	
void CTestStepVidRecorderAudioType::MvruoEvent(const TMMFEvent& /*aEvent*/)
	{
	}


/**
 *
 * Test step Preamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepVidRecorderAudioType::DoTestStepPreambleL(void)
	{
	enum TVerdict verdict;
	// this installs the scheduler
	verdict = CTestStepUnitMMFVidClient::DoTestStepPreambleL();
	if (verdict != EPass)
		return verdict;

	iFileName.Set(KClockFileName);

	// Printing to the console and log file
	INFO_PRINTF1(_L("MM-MMF-VCLNT-U-0237-CP"));
	INFO_PRINTF1(_L("this test is about the audio type of a video clip: CVideoRecorderUtility::[GetSupported|Set]AudioType[s]L()"));


	return verdict;

	}

/**
 *
 * Test step Postamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepVidRecorderAudioType::DoTestStepPostambleL(void)
	{
	delete iRecorder;
	iRecorder = NULL;
	//[ Destroy the scheduler ]
	return CTestStepUnitMMFVidClient::DoTestStepPostambleL();
	}

/**
 *
 * Do the test step.
 * Each test step must supply an implementation for DoTestStepL.
 *
 * @return	"TVerdict"
 *			The result of the test step
 *
 * @xxxx
 * 
 */
TVerdict CTestStepVidRecorderAudioType::DoTestStepL()
	{
	iTestStepResult = EPass;
	TInt err = KErrNone;
	TFourCC type;

	iRecorder = CVideoRecorderUtility::NewL(*this);

	if (iRecorder == NULL ||
				!RecorderStateStopped() )
				return EInconclusive;

	TRAP(err, RecOpenAndStartSchedulerL() );

	if (err != KErrNone || iError != KErrNone) 
		return EInconclusive;
		
	TRAP(err, type = iRecorder->AudioTypeL() );

	if (err != KErrNone)
		return EFail;

	// TBD
	//RArray<TFourCC> vidTypes;
	//TRAP(err, GetSupportedAudioTypesL(vidTypes) );
	//else if (err != KErrNone) 
	//	return EFail;

	// check a known audio type when codecs are available

	delete iRecorder;
	iRecorder = NULL;

	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}

//
// --------------------------------------------
//
/**
 *
 * Static constructor for CTestStepVidRecorderGain.
 *
 *
 * @return	"CTestStepVidRecorderGain*"
 *			The constructed CTestStepVidRecorderGain
 *
 * @xxxx
 * 
 */
CTestStepVidRecorderGain* CTestStepVidRecorderGain::NewL()
	{
	CTestStepVidRecorderGain* self = new(ELeave) CTestStepVidRecorderGain;
	return self;
	}

/**
 *
 * Test step constructor.
 * Each test step initialises its own name.
 *
 * @xxxx
 * 
 */
CTestStepVidRecorderGain::CTestStepVidRecorderGain() 
	{
	iTestStepResult = EPass;
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-VCLNT-U-0238-CP");
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 * 
 */
CTestStepVidRecorderGain::~CTestStepVidRecorderGain()
	{
	}

/**
 * Implementation of the MVideoRecorderUtilityObserver interface functions
 **/
void CTestStepVidRecorderGain::MvruoOpenComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); // check if asynchronous
	}

void CTestStepVidRecorderGain::MvruoPrepareComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); // check if asynchronous
	}

void CTestStepVidRecorderGain::MvruoRecordComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); // check if asynchronous
	}
	
void CTestStepVidRecorderGain::MvruoEvent(const TMMFEvent& /*aEvent*/)
	{
	}

/**
 *
 * Test step Preamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepVidRecorderGain::DoTestStepPreambleL(void)
	{
	enum TVerdict verdict;
	// this installs the scheduler
	verdict = CTestStepUnitMMFVidClient::DoTestStepPreambleL();
	if (verdict != EPass)
		return verdict;

	iFileName.Set(KClockFileName);

	// Printing to the console and log file
	INFO_PRINTF1(_L("MM-MMF-VCLNT-U-0238-CP"));
	INFO_PRINTF1(_L("this tests the volue functions with CVideoRecorderUtility::[Max|Set|]GainL()"));

	return verdict;
	}

/**
 *
 * Test step Postamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepVidRecorderGain::DoTestStepPostambleL(void)
	{
	delete iRecorder;
	iRecorder = NULL;
	//[ Destroy the scheduler ]
	return CTestStepUnitMMFVidClient::DoTestStepPostambleL();
	}

/**
 *
 * Do the test step.
 * Each test step must supply an implementation for DoTestStepL.
 *
 * @return	"TVerdict"
 *			The result of the test step
 *
 * @xxxx
 * 
 */
TVerdict CTestStepVidRecorderGain::DoTestStepL()
	{
	iTestStepResult = EPass;
	TInt err = KErrNone;
	TInt gain, refGain;

	gain = 0;
	refGain = 0;

	iRecorder = CVideoRecorderUtility::NewL(*this);

	if (iRecorder == NULL ||
				!RecorderStateStopped() )
				return EInconclusive;

	TRAP(err, RecOpenAndStartSchedulerL() );

	if (err != KErrNone) 
		return EInconclusive;


	TRAP(err, gain = iRecorder->MaxGainL() );

	if (err != KErrNone ||
		gain < 0)
		return EFail;

	TRAP(err, iRecorder->SetGainL(gain/2) );

	if (err != KErrNone)
		return EFail;

	TRAP(err, refGain = iRecorder->GainL() );

	if (err != KErrNone ||
		refGain != gain/2 )
		return EFail;


	delete iRecorder;
	iRecorder = NULL;

	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}

//
// --------------------------------------------
//
/**
 *
 * Static constructor for CTestStepVidRecorderMeta.
 *
 *
 * @return	"CTestStepVidRecorderMeta*"
 *			The constructed CTestStepVidRecorderMeta
 *
 * @xxxx
 * 
 */
CTestStepVidRecorderMeta* CTestStepVidRecorderMeta::NewL()
	{
	CTestStepVidRecorderMeta* self = new(ELeave) CTestStepVidRecorderMeta;
	return self;
	}

/**
 *
 * Test step constructor.
 * Each test step initialises its own name.
 *
 * @xxxx
 * 
 */
CTestStepVidRecorderMeta::CTestStepVidRecorderMeta() 
	{
	iTestStepResult = EPass;
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-VCLNT-U-0239-HP");
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 * 
 */
CTestStepVidRecorderMeta::~CTestStepVidRecorderMeta()
	{
	}

/**
 * Implementation of the MVideoRecorderUtilityObserver interface functions
 **/
void CTestStepVidRecorderMeta::MvruoOpenComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); // check if asynchronous
	}

void CTestStepVidRecorderMeta::MvruoPrepareComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); // check if asynchronous
	}

void CTestStepVidRecorderMeta::MvruoRecordComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); // check if asynchronous
	}
	
void CTestStepVidRecorderMeta::MvruoEvent(const TMMFEvent& /*aEvent*/)
	{
	}


/**
 *
 * Test step Preamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepVidRecorderMeta::DoTestStepPreambleL(void)
	{
	enum TVerdict verdict;
	// this installs the scheduler
	verdict = CTestStepUnitMMFVidClient::DoTestStepPreambleL();
	if (verdict != EPass)
		return verdict;

	TInt err = KErrNone;

	iFileName.Set(KClockFileName);

	// Printing to the console and log file
	INFO_PRINTF1(_L("MM-MMF-VCLNT-U-0239-HP"));
	INFO_PRINTF1(_L("this tests the metadata functions with CVideoRecorderUtility::[NumberOf]MetaDataEntr[y|ies]L()"));

	iRecorder = CVideoRecorderUtility::NewL(*this);

	if (iRecorder == NULL ||
				!RecorderStateStopped() )
				return EInconclusive;

	TRAP(err, RecOpenAndStartSchedulerL() );
	
	if (err != KErrNone || iError != KErrNone) 
		return EInconclusive;
	
	return verdict;
	}

/**
 *
 * Test step Postamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepVidRecorderMeta::DoTestStepPostambleL(void)
	{
	delete iRecorder;
	iRecorder = NULL;
	//[ Destroy the scheduler ]
	return CTestStepUnitMMFVidClient::DoTestStepPostambleL();
	}

/**
 *
 * Do the test step.
 * Each test step must supply an implementation for DoTestStepL.
 *
 * @return	"TVerdict"
 *			The result of the test step
 *
 * @xxxx
 * 
 */
TVerdict CTestStepVidRecorderMeta::DoTestStepL()
	{
	iTestStepResult = EPass;
	TInt err = KErrNone;
	
	iRecorder = CVideoRecorderUtility::NewL(*this);

	if (iRecorder == NULL ||
				!RecorderStateStopped() )
				return EInconclusive;

	TRAP(err, RecOpenAndStartSchedulerL() );

	if (err != KErrNone) 
		return EInconclusive;

	TInt numOfMeta = 0;
	TRAP(err, numOfMeta = iRecorder->NumberOfMetaDataEntriesL() );

	if (err != KErrNone)
		return EFail;
	if (numOfMeta>0)
		TRAP(err, iRecorder->MetaDataEntryL(numOfMeta-1) )

	if (err != KErrNone)
		return EFail;

	// awaits video codec implementation
	//for (TInt i=0; i<numOfMeta; i++)
	//	{
	//	TRAP(err, meta = iRecorder->MetaDataEntryL(i) );
	//	if (err != KErrNone ||
	//		meta != NULL)
	//		return EFail;
	//	}

	delete iRecorder;
	iRecorder = NULL;

	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}


//

CTestStepVidRecorderHandleEvent* CTestStepVidRecorderHandleEvent::NewL()
	{
	CTestStepVidRecorderHandleEvent* self = new(ELeave) CTestStepVidRecorderHandleEvent();
	return self;
	}

CTestStepVidRecorderHandleEvent::CTestStepVidRecorderHandleEvent()
	{
	iTestStepResult = EFail;
	iTestStepName = _L("MM-MMF-VCLNT-U-0240-LP");
	}

CTestStepVidRecorderHandleEvent::~CTestStepVidRecorderHandleEvent()
	{
	}

TVerdict CTestStepVidRecorderHandleEvent::DoTestStepL()
	{
	INFO_PRINTF1(_L("This tests the MvruoRecordComplete callback (Error code != KErrNone)"));
	INFO_PRINTF1(_L("See INC015645 for details"));

	TInt err = KErrNone;
	TUid uid = { KMmfVideoTestControllerUid };
	iFileName.Set(KClockFileName);

	// Create video recorder
	iRecorder = CVideoRecorderUtility::NewL(*this);

	// Get the controller
	RMMFController* controller = NULL;
	err = GetRecorderController(controller);
	if (err != KErrNone)
		{
		INFO_PRINTF2(_L("Error getting recorder controller %d"), err);
		return EInconclusive;
		}

	// Open file
	TRAP(err, RecOpenAndStartSchedulerL());
	if (err != KErrNone || iError != KErrNone)
		{
		INFO_PRINTF2(_L("Error opening recorder %d"), err);
		return EInconclusive;
		}

	// Add data source
	TMMFFileConfig config;
	config().iPath = KClockFileName;
	err = controller->AddDataSource(KUidMmfFileSource, config);
	if (err != KErrNone)
		{
		INFO_PRINTF2(_L("Error adding data source %d"), err);
		return EInconclusive;
		}

	// Set the error value for recording
	const TInt KExpectedErrorVal = KErrNotSupported;
	TMMFMessageDestination handleInfo(uid);
	TMMFMessageDestinationPckg message(handleInfo);
	TBuf8<64> returnBuf;
	TPckgBuf<TInt> errorValPckg = KExpectedErrorVal;		// error value we want
	TPckgBuf<TInt> errorTypePckg = EMmfTC_PlayError;	// type of error we want
	err = controller->CustomCommandSync(message, KTestSetError, errorValPckg, errorTypePckg, returnBuf);
	if (err != KErrNone)
		{
		INFO_PRINTF2(_L("Error setting the controller error %d"), err);
		return EInconclusive;
		}

	// Prepare
	TRAP(err, RecorderPrepareAndStartScheduler());
	if (err != KErrNone || iError != KErrNone) 
		return EInconclusive;

	// Start recording
	iRecorder->Record();
	CActiveScheduler::Start();	// wait for MvruoRecordComplete

	if (iError != KExpectedErrorVal)
		{
		INFO_PRINTF3(_L("iError does not match expected value: got %d, expecting %d"), iError, KExpectedErrorVal);
		iTestStepResult = EFail;
		}
	else
		{
		// error returned in MvruoRecordComplete was as expected
		iTestStepResult = EPass;
		}

	return iTestStepResult;
	}

//from MVideoRecorderUtilityObserver
void CTestStepVidRecorderHandleEvent::MvruoOpenComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop();
	}

void CTestStepVidRecorderHandleEvent::MvruoPrepareComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop();
	}

void CTestStepVidRecorderHandleEvent::MvruoRecordComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop();
	}
	
void CTestStepVidRecorderHandleEvent::MvruoEvent(const TMMFEvent& /*aEvent*/)
	{
	}

//
//
// --------------------------------------------
//
/**
 *
 * Static constructor for CTestStepVidRecorderGetImplementationInfo.
 *
 *
 * @return	"CTestStepVidRecorderGetImplementationInfo*"
 *			The constructed CTestStepVidRecorderGetImplementationInfo
 *
 * @xxxx
 * 
 */
CTestStepVidRecorderGetImplementationInfo* CTestStepVidRecorderGetImplementationInfo::NewL()
	{
	CTestStepVidRecorderGetImplementationInfo* self = new(ELeave) CTestStepVidRecorderGetImplementationInfo;
	return self;
	}

/**
 *
 * Test step constructor.
 * Each test step initialises its own name.
 *
 * @xxxx
 * 
 */
CTestStepVidRecorderGetImplementationInfo::CTestStepVidRecorderGetImplementationInfo() : iHasPlayed(EFalse)
	{
	iTestStepResult = EPass;
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-VCLNT-U-0250-HP");
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 * 
 */
CTestStepVidRecorderGetImplementationInfo::~CTestStepVidRecorderGetImplementationInfo()
	{
	}

/**
 * Implementation of the MVideoRecorderUtilityObserver interface functions
 **/
void CTestStepVidRecorderGetImplementationInfo::MvruoOpenComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); 
	}

void CTestStepVidRecorderGetImplementationInfo::MvruoPrepareComplete(TInt /*aError*/)
	{
	}


void CTestStepVidRecorderGetImplementationInfo::MvruoRecordComplete(TInt /*aError*/)
	{
	}
	
void CTestStepVidRecorderGetImplementationInfo::MvruoEvent(const TMMFEvent& /*aEvent*/)
	{
	}


/**
 *
 * Test step Preamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepVidRecorderGetImplementationInfo::DoTestStepPreambleL(void)
	{
	enum TVerdict verdict;
	// this installs the scheduler
	verdict = CTestStepUnitMMFVidClient::DoTestStepPreambleL();
	if (verdict != EPass)
		return verdict;

	iFileName.Set(KClockFileName);

	// Printing to the console and log file
	INFO_PRINTF1(iTestStepName);
	INFO_PRINTF1(_L("this test is playing a video with a CVideoRecorderUtility::ControllerImplementationInfoL()"));

	return verdict;
	}

/**
 *
 * Test step Postamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepVidRecorderGetImplementationInfo::DoTestStepPostambleL(void)
	{
	delete iRecorder;
	iRecorder = NULL;
	//[ Destroy the scheduler ]
	return CTestStepUnitMMFVidClient::DoTestStepPostambleL();
	}

/**
 *
 * Do the test step.
 * Each test step must supply an implementation for DoTestStepL.
 *
 * @return	"TVerdict"
 *			The result of the test step
 *
 * @xxxx
 * 
 */
TVerdict CTestStepVidRecorderGetImplementationInfo::DoTestStepL()
	{
	TInt err;
	err = KErrNone;
	iTestStepResult = EFail;

	iRecorder = CVideoRecorderUtility::NewL(*this);

	TRAP(err, RecOpenAndStartSchedulerL() );

	if (iError != KErrNone || err != KErrNone) 
		return EInconclusive;

	const CMMFControllerImplementationInformation* info = NULL;
	
	TRAP(err, info = &iRecorder->ControllerImplementationInformationL());
	if (err != KErrNone)
		return EFail;

	if (info->DisplayName().Compare(KVideoTestControllerName)!=0)
		iTestStepResult = EFail;
	else
		iTestStepResult = EPass;

	delete iRecorder;
	iRecorder = NULL;

	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}
//
//
// --------------------------------------------
//
/**
 *
 * Static constructor for CTestStepVidRecorderSendCustomCommandSync.
 *
 *
 * @return	"CTestStepVidRecorderSendCustomCommandSync*"
 *			The constructed CTestStepVidRecorderSendCustomCommandSync
 *
 * @xxxx
 * 
 */
CTestStepVidRecorderSendCustomCommandSync* CTestStepVidRecorderSendCustomCommandSync::NewL()
	{
	CTestStepVidRecorderSendCustomCommandSync* self = new(ELeave) CTestStepVidRecorderSendCustomCommandSync;
	return self;
	}

/**
 *
 * Test step constructor.
 * Each test step initialises its own name.
 *
 * @xxxx
 * 
 */
CTestStepVidRecorderSendCustomCommandSync::CTestStepVidRecorderSendCustomCommandSync() : iHasPlayed(EFalse)
	{
	iTestStepResult = EPass;
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-VCLNT-U-0251-HP");
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 * 
 */
CTestStepVidRecorderSendCustomCommandSync::~CTestStepVidRecorderSendCustomCommandSync()
	{
	}

/**
 * Implementation of the MVideoRecorderUtilityObserver interface functions
 **/
void CTestStepVidRecorderSendCustomCommandSync::MvruoOpenComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); 
	}

void CTestStepVidRecorderSendCustomCommandSync::MvruoPrepareComplete(TInt /*aError*/)
	{
	}


void CTestStepVidRecorderSendCustomCommandSync::MvruoRecordComplete(TInt /*aError*/)
	{
	}
	
void CTestStepVidRecorderSendCustomCommandSync::MvruoEvent(const TMMFEvent& /*aEvent*/)
	{
	}


/**
 *
 * Test step Preamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepVidRecorderSendCustomCommandSync::DoTestStepPreambleL(void)
	{
	enum TVerdict verdict;
	// this installs the scheduler
	verdict = CTestStepUnitMMFVidClient::DoTestStepPreambleL();
	if (verdict != EPass)
		return verdict;

	iFileName.Set(KClockFileName);

	// Printing to the console and log file
	INFO_PRINTF1(iTestStepName);
	INFO_PRINTF1(_L("this test is a test of CVideoRecorderUtility::CustomCommandSync()"));

	return verdict;
	}

/**
 *
 * Test step Postamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepVidRecorderSendCustomCommandSync::DoTestStepPostambleL(void)
	{
	delete iRecorder;
	iRecorder = NULL;
	//[ Destroy the scheduler ]
	return CTestStepUnitMMFVidClient::DoTestStepPostambleL();
	}

/**
 *
 * Do the test step.
 * Each test step must supply an implementation for DoTestStepL.
 *
 * @return	"TVerdict"
 *			The result of the test step
 *
 * @xxxx
 * 
 */
TVerdict CTestStepVidRecorderSendCustomCommandSync::DoTestStepL()
	{
	TInt err;
	err = KErrNone;
	iTestStepResult = EFail;

	iRecorder = CVideoRecorderUtility::NewL(*this);

	TRAP(err, RecOpenAndStartSchedulerL() );

	if (iError != KErrNone && err!=KErrNone)
		return EInconclusive;

	TUid uid;
	uid = TUid::Uid(0x101f72B4);
	TMMFMessageDestinationPckg destPckg(uid);
	// test without a return value
	err = iRecorder->CustomCommandSync(destPckg,
								EMMFTestCustomCommandGeneralSynchronous,
								KFirstParameter,
								KSecondParameter);
								
	if (err!=KTestCustomCommandSuccess)
		return EFail;
		
	TBuf8<256> buf;
		
	err = iRecorder->CustomCommandSync(destPckg,
								EMMFTestCustomCommandGeneralSynchronousWithReturn,
								KFirstParameter,
								KSecondParameter,
								buf);
	
	if (err!=KTestCustomCommandSuccess)
		return EFail;
		
	if (buf.Compare(KReturnParameter)==0)
		iTestStepResult = EPass;
	else
		iTestStepResult = EFail;

	delete iRecorder;
	iRecorder = NULL;

	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}
//
//
// --------------------------------------------
//
/**
 *
 * Static constructor for CTestStepVidRecorderSendCustomCommandAsync.
 *
 *
 * @return	"CTestStepVidRecorderSendCustomCommandAsync*"
 *			The constructed CTestStepVidRecorderSendCustomCommandAsync
 *
 * @xxxx
 * 
 */
CTestStepVidRecorderSendCustomCommandAsync* CTestStepVidRecorderSendCustomCommandAsync::NewL()
	{
	CTestStepVidRecorderSendCustomCommandAsync* self = new(ELeave) CTestStepVidRecorderSendCustomCommandAsync;
	return self;
	}

/**
 *
 * Test step constructor.
 * Each test step initialises its own name.
 *
 * @xxxx
 * 
 */
CTestStepVidRecorderSendCustomCommandAsync::CTestStepVidRecorderSendCustomCommandAsync() : iHasPlayed(EFalse)
	{
	iTestStepResult = EPass;
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-VCLNT-U-0252-HP");
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 * 
 */
CTestStepVidRecorderSendCustomCommandAsync::~CTestStepVidRecorderSendCustomCommandAsync()
	{
	}

/**
 * Implementation of the MVideoRecorderUtilityObserver interface functions
 **/
void CTestStepVidRecorderSendCustomCommandAsync::MvruoOpenComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); 
	}

void CTestStepVidRecorderSendCustomCommandAsync::MvruoPrepareComplete(TInt /*aError*/)
	{
	}


void CTestStepVidRecorderSendCustomCommandAsync::MvruoRecordComplete(TInt /*aError*/)
	{
	}
	
void CTestStepVidRecorderSendCustomCommandAsync::MvruoEvent(const TMMFEvent& /*aEvent*/)
	{
	}


/**
 *
 * Test step Preamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepVidRecorderSendCustomCommandAsync::DoTestStepPreambleL(void)
	{
	enum TVerdict verdict;
	// this installs the scheduler
	verdict = CTestStepUnitMMFVidClient::DoTestStepPreambleL();
	if (verdict != EPass)
		return verdict;
	
	iFileName.Set(KClockFileName);
		
	iAsyncObserver = new (ELeave) CAsyncObserver;

	// Printing to the console and log file
	INFO_PRINTF1(iTestStepName);
	INFO_PRINTF1(_L("this test is a test of CVideoRecorderUtility::CustomCommandAsync()"));

	return verdict;
	}

/**
 *
 * Test step Postamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepVidRecorderSendCustomCommandAsync::DoTestStepPostambleL(void)
	{
	delete iRecorder;
	iRecorder = NULL;
	delete iAsyncObserver;
	//[ Destroy the scheduler ]
	return CTestStepUnitMMFVidClient::DoTestStepPostambleL();
	}

/**
 *
 * Do the test step.
 * Each test step must supply an implementation for DoTestStepL.
 *
 * @return	"TVerdict"
 *			The result of the test step
 *
 * @xxxx
 * 
 */
TVerdict CTestStepVidRecorderSendCustomCommandAsync::DoTestStepL()
	{
	TInt err;
	err = KErrNone;
	iTestStepResult = EFail;

	iRecorder = CVideoRecorderUtility::NewL(*this);


	TRAP(err, RecOpenAndStartSchedulerL() );
	
	if (err!=KErrNone)
		return EInconclusive;

	TUid uid;
	uid = TUid::Uid(0x101f72B4);
	iDestinationPckg = TMMFMessageDestinationPckg(uid);
	
	// test without a return value
	iRecorder->CustomCommandAsync(iDestinationPckg,
								EMMFTestCustomCommandGeneralAsynchronous,
								KFirstParameter,
								KSecondParameter,
								iAsyncObserver->ActiveStatus());

	CActiveScheduler::Start();	
	if (iAsyncObserver->iStatus.Int() !=KTestCustomCommandSuccess)
		return EFail;
								
	TBuf8<256> buf;
		
	iRecorder->CustomCommandAsync(iDestinationPckg,
								EMMFTestCustomCommandGeneralAsynchronousWithReturn,
								KFirstParameter,
								KSecondParameter,
								buf,
								iAsyncObserver->ActiveStatus());

	CActiveScheduler::Start();	
	if (iAsyncObserver->iStatus.Int() !=KTestCustomCommandSuccess)
		return EFail;
		
	if (buf.Compare(KReturnParameter)==0)
		iTestStepResult = EPass;
	else
		iTestStepResult = EFail;

	delete iRecorder;
	iRecorder = NULL;

	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}


//
//
// --------------------------------------------
//
/**
 *
 * Static constructor for CTestStepVidRecorderGeneralEvent.
 *
 *
 * @return	"CTestStepVidRecorderGeneralEvent*"
 *			The constructed CTestStepVidRecorderGeneralEvent
 *
 * @xxxx
 * 
 */
CTestStepVidRecorderGeneralEvent* CTestStepVidRecorderGeneralEvent::NewL()
	{
	CTestStepVidRecorderGeneralEvent* self = new(ELeave) CTestStepVidRecorderGeneralEvent;
	return self;
	}

/**
 *
 * Test step constructor.
 * Each test step initialises its own name.
 *
 * @xxxx
 * 
 */
CTestStepVidRecorderGeneralEvent::CTestStepVidRecorderGeneralEvent() 
	{
	iTestStepResult = EPass;
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-VCLNT-U-0253-HP");
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 * 
 */
CTestStepVidRecorderGeneralEvent::~CTestStepVidRecorderGeneralEvent()
	{
	}

/**
 * Implementation of the MVideoRecorderUtilityObserver interface functions
 **/
void CTestStepVidRecorderGeneralEvent::MvruoOpenComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); 
	}

void CTestStepVidRecorderGeneralEvent::MvruoPrepareComplete(TInt /*aError*/)
	{
	}

void CTestStepVidRecorderGeneralEvent::MvruoRecordComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); 
	}
	
void CTestStepVidRecorderGeneralEvent::MvruoEvent(const TMMFEvent& aEvent)
	{
	iEventReceived = ETrue;
	if (aEvent.iEventType == KMMFEventCategoryVideoPlayerGeneralError &&
		aEvent.iErrorCode == KTestGeneralError)
		iCorrectEventReceived = ETrue;
	else
		iCorrectEventReceived = EFalse;
	CActiveScheduler::Stop(); 
	}


/**
 *
 * Test step Preamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepVidRecorderGeneralEvent::DoTestStepPreambleL(void)
	{
	enum TVerdict verdict;
	// this installs the scheduler
	verdict = CTestStepUnitMMFVidClient::DoTestStepPreambleL();
	if (verdict != EPass)
		return verdict;

	iFileName.Set(KClockFileName);

	// Printing to the console and log file
	INFO_PRINTF1(iTestStepName);
	INFO_PRINTF1(_L("this test is of receiving a general event from a controller"));

	return verdict;
	}

/**
 *
 * Test step Postamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepVidRecorderGeneralEvent::DoTestStepPostambleL(void)
	{
	delete iPlayer;
	iPlayer = NULL;
	//[ Destroy the scheduler ]
	return CTestStepUnitMMFVidClient::DoTestStepPostambleL();
	}

/**
 *
 * Do the test step.
 * Each test step must supply an implementation for DoTestStepL.
 *
 * @return	"TVerdict"
 *			The result of the test step
 *
 * @xxxx
 * 
 */
TVerdict CTestStepVidRecorderGeneralEvent::DoTestStepL()
	{
	TInt err;
	err = KErrNone;
	iTestStepResult = EFail;

	iRecorder = CVideoRecorderUtility::NewL(*this);

	TRAP(err, RecOpenAndStartSchedulerL() );

	if (err!=KErrNone)
		return EInconclusive;

	TUid uid;
	uid = TUid::Uid(0x101f72B4);
	TMMFMessageDestinationPckg destPckg(uid);
	// test without a return value
	err = iRecorder->CustomCommandSync(destPckg,
								EMMFTestCustomCommandRequestGeneralEvent,
								KNullDesC8,
								KNullDesC8);
								
	if (err!=KErrNone)
		return EInconclusive;
		
	CActiveScheduler::Start();
	iTestStepResult = EFail;
	
	if (iEventReceived)
		{
		if (iCorrectEventReceived)
			iTestStepResult = EPass;
		else
			INFO_PRINTF1(_L("incorrect event received"));
		}
	else
		INFO_PRINTF1(_L("incorrect callback received"));
	
	delete iPlayer;
	iPlayer = NULL;

	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}



//
//
// --------------------------------------------
//
/**
 *
 * Static constructor for CTestStepVidRecorderCheckImplementationInfo.
 *
 *
 * @return	"CTestStepVidRecorderCheckImplementationInfo*"
 *			The constructed CTestStepVidRecorderCheckImplementationInfo
 *
 * @xxxx
 * 
 */
CTestStepVidRecorderCheckImplementationInfo* CTestStepVidRecorderCheckImplementationInfo::NewL()
	{
	CTestStepVidRecorderCheckImplementationInfo* self = new(ELeave) CTestStepVidRecorderCheckImplementationInfo;
	return self;
	}

/**
 *
 * Test step constructor.
 * Each test step initialises its own name.
 *
 * @xxxx
 * 
 */
CTestStepVidRecorderCheckImplementationInfo::CTestStepVidRecorderCheckImplementationInfo() : iHasPlayed(EFalse)
	{
	iTestStepResult = EPass;
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-VCLNT-U-0255-HP");
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 * 
 */
CTestStepVidRecorderCheckImplementationInfo::~CTestStepVidRecorderCheckImplementationInfo()
	{
	}

/**
 * Implementation of the MVideoRecorderUtilityObserver interface functions
 **/
void CTestStepVidRecorderCheckImplementationInfo::MvruoOpenComplete(TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop(); 
	}

void CTestStepVidRecorderCheckImplementationInfo::MvruoPrepareComplete(TInt /*aError*/)
	{
	}


void CTestStepVidRecorderCheckImplementationInfo::MvruoRecordComplete(TInt /*aError*/)
	{
	}
	
void CTestStepVidRecorderCheckImplementationInfo::MvruoEvent(const TMMFEvent& /*aEvent*/)
	{
	}


/**
 *
 * Test step Preamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepVidRecorderCheckImplementationInfo::DoTestStepPreambleL(void)
	{
	// this installs the scheduler
	TVerdict verdict = CTestStepUnitMMFVidClient::DoTestStepPreambleL();
	
	if (verdict != EPass)
		{
		return verdict;	
		}	

	iFileName.Set(KClockFileName);

	// Printing to the console and log file
	INFO_PRINTF1(iTestStepName);
	INFO_PRINTF1(_L("this test is playing a video with a CVideoRecorderUtility::ControllerImplementationInfoL()"));

	return verdict;
	}

/**
 *
 * Test step Postamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepVidRecorderCheckImplementationInfo::DoTestStepPostambleL(void)
	{
	//[ Destroy the scheduler ]
	return CTestStepUnitMMFVidClient::DoTestStepPostambleL();
	}

/**
 *
 * Do the test step.
 * Each test step must supply an implementation for DoTestStepL.
 *
 * @return	"TVerdict"
 *			The result of the test step
 *
 * @xxxx
 * 
 */
TVerdict CTestStepVidRecorderCheckImplementationInfo::DoTestStepL()
	{
	TInt err = KErrNone;
	iTestStepResult = EFail;

	iRecorder = CVideoRecorderUtility::NewL(*this);

	TRAP(err, RecOpenAndStartSchedulerL() );

	if (iError != KErrNone || err != KErrNone) 
		return EInconclusive;
	
	TRAP(err,  &iRecorder->ControllerImplementationInformationL());
	if (err != KErrNone)
		{
		return EFail;	
		}

	iRecorder->Close();
	
	TRAP(err,  &iRecorder->ControllerImplementationInformationL());
	
	// Checking for KErrNotReady because we dont have one more videocontroller 
	// to compare
	if (err == KErrNotReady  )
		{
		iTestStepResult = EPass;			
		}
	else
		{
		iTestStepResult = EFail;	
		}
	
	delete iRecorder;
	iRecorder = NULL;

	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}
