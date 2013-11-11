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
// TestStepVidRecorderNew.cpp
// 
//

#include <testframework.h>

#include <videorecorder.h>

// Specific includes for this test suite
#include "TestStepUnitMMFVidClient.h"
#include "TestSuiteUnitMMFVidClient.h"

// Specific includes for these test steps
#include "TestStepVidRecorder.h"

#define KAviRecordControllerUid 0x102737E1
#define KAviRecordFormatUid 0x102737E3
#define KTimedRecTicks			2
#define KTimedRecIntervallSecs	5

_LIT(KTestFileName,"C:\\mm\\mmf\\testfiles\\vclnt\\test.dummy");
_LIT(KTestFileNamePlayback,"C:\\mm\\mmf\\testfiles\\vclnt\\small2.wav");
_LIT(KTestCameraPluginName, "C:\\102070cc.txt");

_LIT(KMMCameraPluginName, "C:\\mmcameraclientplugin.txt");

/**
 *
 * Static constructor for CTestStepVidRecordError.
 *
 *
 * @return	"CTestStepVidRecordError*"
 *			The constructed CTestStepVidRecordError
 *
 * @xxxx
 * 
 */
CTestStepVidRecordError* CTestStepVidRecordError::NewL()
	{
	CTestStepVidRecordError* self = new(ELeave) CTestStepVidRecordError;
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
CTestStepVidRecordError::CTestStepVidRecordError()
	{
	iTestStepResult = EPass;
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-VCLNT-U-0258-CP");
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 * 
 */
CTestStepVidRecordError::~CTestStepVidRecordError()
	{
	}

/**
 * Implementation of the MVideoRecorderUtilityObserver interface functions
 **/
void CTestStepVidRecordError::MvruoOpenComplete(TInt aError)
	{
	iError = aError;
	if( iError != KErrNone )
		{
		ERR_PRINTF2(_L("MvruoOpenComplete returned with error: %d"),iError);
		return;
		}
	INFO_PRINTF1(_L("MvruoOpenComplete"));
	iRecorder->Prepare();
	}

void CTestStepVidRecordError::MvruoPrepareComplete(TInt aError)
	{
	iError = aError;
	if( iError != KErrNone )
		{
		ERR_PRINTF2(_L("MvruoPrepareComplete returned with error: %d"),iError);
		return;
		}
	INFO_PRINTF1(_L("MvruoPrepareComplete"));
		
	TRAPD(err, iRecorder->SetAudioEnabledL(TRUE));
	if( err != KErrNone )
		{
		ERR_PRINTF2(_L("SetAudioEnabled Failed with Error: %d"),err);
		iError = err;
		return;
		}
	TRAP(err, iRecorder->SetVideoFrameRateL(10));
	if( err != KErrNone )
		{
		ERR_PRINTF2(_L("SetVideoFrameRateL Failed with Error: %d"),err);
		iError = err;
		return;
		}
	TRAP(err, iRecorder->SetVideoFrameSizeL(TSize(176, 144)));
	if( err != KErrNone )
		{
		ERR_PRINTF2(_L("SetVideoFrameSizeL Failed with Error: %d"),err);
		iError = err;
		return;
		}
	iRecorder->Record();
	
	INFO_PRINTF1(_L("Start Timer"));
	iTimedRecording->IssueRequest(iRecorder, iAudioPlayer);
	}

void CTestStepVidRecordError::MvruoRecordComplete(TInt aError)
	{
	iError = aError;
	if( iError != KErrNone )
		{
		ERR_PRINTF2(_L("MvruoRecordComplete returned with error: %d"),iError);
		}

	TRAPD(err, iDuration = iRecorder->DurationL());
	
	if(err != KErrNone)
		{
		ERR_PRINTF2(_L("iRecorder->DurationL() left with error: %d"),err);
		}

	CActiveScheduler::Stop();
	}
	
void CTestStepVidRecordError::MvruoEvent(const TMMFEvent& /*aEvent*/)
	{
	INFO_PRINTF1(_L("MvruoEvent"));
	}


/**
 * Implementation of the MCameraObserver2 interface functions
 **/
void CTestStepVidRecordError::HandleEvent(const TECAMEvent& /*aEvent*/)
	{
	
	}
	
void CTestStepVidRecordError::ViewFinderReady(MCameraBuffer& /*aCameraBuffer*/,TInt /*aError*/)
	{
	
	}
	
void CTestStepVidRecordError::ImageBufferReady(MCameraBuffer& /*aCameraBuffer*/,TInt /*aError*/)
	{
	
	}
	
void CTestStepVidRecordError::VideoBufferReady(MCameraBuffer& /*aCameraBuffer*/,TInt /*aError*/)
	{
	
	}
	
/**
 * Implementation of the MMdaAudioPlayerCallback interface functions
 **/
void CTestStepVidRecordError::MapcInitComplete(TInt /*aError*/, const TTimeIntervalMicroSeconds& /*aDuration*/)
	{
	}

void CTestStepVidRecordError::MapcPlayComplete(TInt /*aError*/)
	{
	
	}

/* ------------------------------------------------- */
/**
 * Utility functions
 */

void CTestStepVidRecordError::CreateDependencyFileL()
	{
	// create the file the plugin depends on.
	INFO_PRINTF1(_L("Create dependency file for camera plugin"));
	User::LeaveIfError(iFs.Connect());

	iFs.Delete(KMMCameraPluginName);

	RFile file;
	TInt err = file.Replace(iFs, KTestCameraPluginName, EFileShareAny);
  	file.Close();
	iFs.Close();
  	if(err != KErrNone)
  		{
	    User::LeaveIfError(KErrNotSupported);
  		}
	}
void CTestStepVidRecordError::RemoveDependencyFileL()
	{
	User::LeaveIfError(iFs.Connect());
	iFs.Delete(KTestCameraPluginName);
	iFs.Close();
	}


void CTestStepVidRecordError::RecOpenAndStartSchedulerL()
	{
	
	INFO_PRINTF1(_L("Reserve Camera"));
	iCamera->Reserve();
	
	INFO_PRINTF1(_L("Power On Camera"));
	iCamera->PowerOn();
	const TUid KVidAviFormatUid = {KAviRecordFormatUid};
	const TUid KVidAviControllerUid = {KAviRecordControllerUid};
	User::LeaveIfError(iFs.Connect());
	User::LeaveIfError(iFs.ShareProtected());
	User::LeaveIfError(iFile.Replace(iFs, iFileName, EFileWrite | EFileShareAny));
	
	INFO_PRINTF1(_L("call iRecorder->OpenFileL"));
	iRecorder->OpenFileL(iFile, iCamera->Handle(), KVidAviControllerUid, KVidAviFormatUid ); 

	CActiveScheduler::Start();
	}

/* ------------------------------------------------- */


/**
 *
 * Test step Preamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepVidRecordError::DoTestStepPreambleL(void)
	{
	CTestStepUnitMMFVidClient::DoTestStepPreambleL();
	// Printing to the console and log file
	INFO_PRINTF1(_L("MM-MMF-VCLNT-U-0258-CP"));

	iFileName.Set(KTestFileName);
	iFileNamePlay.Set(KTestFileNamePlayback);

	CreateDependencyFileL();
	TInt noCam = CCamera::CamerasAvailable();
	INFO_PRINTF2(_L("Number of Cameras: %d"),noCam);

	INFO_PRINTF1(_L("connecting to the font and bitmap server with Connect()"));
	User::LeaveIfError(RFbsSession::Connect());

	INFO_PRINTF1(_L("initialising a CCamera object with New2L()"));
	iCamera = CCamera::New2L(*this, 0, 0);

	INFO_PRINTF1(_L("initialising a CTimedRecording object with NewL(), 5 seconds"));
	iTimedRecording = CTimedRecording::NewL(KTimedRecTicks, KTimedRecIntervallSecs);
	
	iDuration = 0;
	iError = KErrNone;
	
	return EPass;
	}

/**
 *
 * Test step Postamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepVidRecordError::DoTestStepPostambleL(void)
	{
	delete iCamera;
	delete iTimedRecording;
	
	RFbsSession::Disconnect();
	RemoveDependencyFileL();
		
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
TVerdict CTestStepVidRecordError::DoTestStepL()
	{
	iTestStepResult = EPass;

	__MM_HEAP_MARK;	
	INFO_PRINTF1(_L("initialising a CVideoRecorderUtility object with NewL()"));
	iRecorder = CVideoRecorderUtility::NewL(*this, EMdaPriorityNormal);
	
	INFO_PRINTF1(_L("initialising a CMdaAudioPlayerUtility object with NewFilePlayerL()"));
	iAudioPlayer = CMdaAudioPlayerUtility::NewFilePlayerL(iFileNamePlay,*this,EMdaPriorityMax);

	if(iRecorder == NULL ||	!RecorderStateStopped() || iAudioPlayer == NULL)
		{
		return EFail;
		}

	TInt err =0;
	TRAP(err, RecOpenAndStartSchedulerL());
	if (err != KErrNone)
		{
		ERR_PRINTF2(_L("RecOpenAndStartSchedulerL returned Error: %d"),err);
		iTestStepResult = EInconclusive;
		}
	else
		{
		if( iError != KErrInUse )
			{
			ERR_PRINTF2(_L("Return error is not KErrInUse, it is %d"),iError);
			iTestStepResult = EFail;
			}
		if( iDuration == 0 )
			{
			ERR_PRINTF1(_L("iDuration is 0"));
			iTestStepResult = EFail;
			}
		}
	
	delete iAudioPlayer;

	iRecorder->Close();
	delete iRecorder;

	iFile.Close();
	iFs.Close();

	iCamera->PowerOff();
	iCamera->Release();

	__MM_HEAP_MARKEND;

	INFO_PRINTF1(_L("finished with this test step"));
	// test steps return a result
	return iTestStepResult;
	}



	
//
//
// -----> CTimedRecording (implementation)
//
//

CTimedRecording::CTimedRecording() : CTimer(CActive::EPriorityStandard)
	  // Construct  zero-priority active object
	{};

CTimedRecording* CTimedRecording::NewLC( TInt aTicksRequested, TInt aSeconds )
	{
	CTimedRecording* self=new (ELeave) CTimedRecording;
	CleanupStack::PushL(self);
	self->ConstructL( aTicksRequested, aSeconds );
	return self;
	}

CTimedRecording* CTimedRecording::NewL( TInt aTicksRequested, TInt aSeconds )
	{
	CTimedRecording* self = NewLC( aTicksRequested, aSeconds );
	CleanupStack::Pop(self);
	return self;
	}

void CTimedRecording::ConstructL( TInt aTicksRequested, TInt aSeconds )
	{
	  // Base class second-phase construction.
	CTimer::ConstructL();
	  // Set members from arguments
	iTicksRequested = aTicksRequested; // Ticks requested
	iSeconds  = aSeconds;  // Interval between ticks

	  // Add active object to active scheduler
	CActiveScheduler::Add(this); 
	}


CTimedRecording::~CTimedRecording()
	{
	  // Make sure we're cancelled
	Cancel();
	}

void CTimedRecording::DoCancel()
	{
   // Call base class version of function
   // Base class
	CTimer::DoCancel(); 
   // Reset any variable needed if the object is re-activated later
   // Reset this variable - needed if the object is re-activated later
	iTicksDone = 0;
	}

void CTimedRecording::IssueRequest(CVideoRecorderUtility* aRecorder, CMdaAudioPlayerUtility* aAudioPlayer)
	{
	// panics
	enum
		{
		EPanicAlreadyActive=1000,
		};
	iRecorder = aRecorder;
	iAudioPlayer = aAudioPlayer;
   // There should never be an outstanding request at this point.
	_LIT(KMsgAlreadyActive, "Is already Active");
	__ASSERT_ALWAYS(!IsActive(), User::Panic(KMsgAlreadyActive, EPanicAlreadyActive));
   // Request another wait
	CTimer::After( iSeconds * 1000000);
	}

void CTimedRecording::RunL()
	{
	// Handle request completion
   // Remember to keep track ofiTicksDone (how many times message request is issued)
   // Message must be sent iTicksRequested times
   // stop scheduler when all requested messages sent
   
	// One more tick done
	iTicksDone++;
	// Issue new request, or stop if we have reached the limit
	if (iTicksDone  < iTicksRequested)
		{
		if(iTicksDone == 1)
			{
			iAudioPlayer->Play();
			}
		IssueRequest(iRecorder, iAudioPlayer);
		}
	else
		{
	   // Reset this variable - needed if the object is re-activated later
		iTicksDone=0;
		iRecorder->Stop();
		iAudioPlayer->Stop();
		// Can now stop the active scheduler
		CActiveScheduler::Stop();
		}
	}
