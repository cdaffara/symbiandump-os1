// Copyright (c) 2002-2010 Nokia Corporation and/or its subsidiary(-ies).
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


// Test system includes
#include <testframework.h>

#include "CmmfVideoTestControllerUIDs.hrh"

#include <mda/common/video.h>
#include <mmf/common/mmfstandardcustomcommands.h>  
#include <mmf/common/mmfcontroller.h>

#include <videoplayer.h>
#include <videorecorder.h>

#include "TestStepUnitMMFVidClient.h"
#include "TestSuiteUnitMMFVidClient.h"
#include "VideoPlayerBody.h"
#include "VideoRecorderBody.h"
#include "TS_Codes.h"

#ifdef SYMBIAN_BUILD_GCE
#include "mediaclientvideodisplaybody.h"
#endif // SYMBIAN_BUILD_GCE

/**
 *
 * CTestStepUnitMMFVidClient constructor
 *
 * @xxxx
 * 
 */
CTestStepUnitMMFVidClient::CTestStepUnitMMFVidClient() : 
iScheduler( NULL )
	{
	iRes = &iFileName;
	}

/**
 *
 * CTestStepUnitMMFVidClient destructor
 *
 * @xxxx
 * 
 */
CTestStepUnitMMFVidClient::~CTestStepUnitMMFVidClient()
	{
	}


	// accessors
/**
 *
 * CTestStepUnitMMFVidClient accessor : set suite
 *
 * @param "CTestSuiteUnitMMFVidClient*"
 *		  The test suite.
 *
 * @xxxx
 * 
 */
void CTestStepUnitMMFVidClient::SetUnitMMFVidClientSuite(CTestSuiteUnitMMFVidClient* aSuite)
	{ 
	iUnitMMFVidClientSuite = aSuite;
	}

/**
 *
 * CTestStepUnitMMFVidClient accessor : get suite
 *
 * @return "CTestSuiteUnitMMFVidClient*"
 *		  The test suite.
 *
 * @xxxx
 * 
 */
CTestSuiteUnitMMFVidClient* CTestStepUnitMMFVidClient::UnitMMFVidClientSuite() const
	{
	return iUnitMMFVidClientSuite;
	}

/**
DoTestStepPreambleL is the preamble function for test steps
It creates an active scheduler and installs it. This is necessary
since we are multithreading and each test step runs in a separate thread.
@return TVerdict which is either EPass or Efail
 
The function can leave if there is insufficient memeory

@lib "TSU_MMF_VCLNT_01.lib"
*/
enum TVerdict CTestStepUnitMMFVidClient::DoTestStepPreambleL(void)
	{
	iScheduler = new(ELeave) CActiveScheduler;
	CActiveScheduler::Install( iScheduler );
	return EPass;
	}

/**
The post amble step cleans up the allocated memory in the preamble
@return TVerdict EPass or EFail
@leaves can leave due to insufficient memory
@lib "TSU_MMF_VCLNT_01.lib"
*/

enum TVerdict CTestStepUnitMMFVidClient::DoTestStepPostambleL(void)
	{
	iFile.Close();
	iFs.Close();
	delete iScheduler ;
	iScheduler = NULL;
	DeInitWserv();	
	return EPass;
	}

/**
@helper functions
@ open a file and starts the scheduler
@lib "TSU_MMF_VCLNT_01.lib"
*/
void CTestStepUnitMMFVidClient::OpenAndStartSchedulerL(TBool aUseFileHandle)
	{
	if (aUseFileHandle)
		{
		User::LeaveIfError(iFs.Connect());
		User::LeaveIfError(iFs.ShareProtected());
		User::LeaveIfError(iFile.Open(iFs, iFileName, EFileRead));
		if (!iVpu2)
			{
			iPlayer->OpenFileL(iFile);
			}
#ifdef SYMBIAN_BUILD_GCE
		else
			{
			iPlayer2->OpenFileL(iFile);
			}
#endif		
		}
	else
		{
		if (!iVpu2)
			{
			iPlayer->OpenFileL(iFileName);
			}
#ifdef SYMBIAN_BUILD_GCE
		else
			{
			iPlayer2->OpenFileL(iFileName);
			}
#endif	
		}
	CActiveScheduler::Start();
	}

/**
@helper functions
@ open a file using a TMMFileSource object and starts the scheduler
@lib "TSU_MMF_VCLNT_01.lib"
*/
void CTestStepUnitMMFVidClient::OpenAndStartSchedulerL(const TMMSource& aSource)
	{
	if (!iVpu2)
		{
		iPlayer->OpenFileL(aSource);
		}
#ifdef SYMBIAN_BUILD_GCE
	else
		{
		iPlayer2->OpenFileL(aSource);
		}
#endif	
	CActiveScheduler::Start();
	}

/**
@helper functions
@ issues a play and starts the scheduler
@lib "TSU_MMF_VCLNT_01.lib"
*/
void CTestStepUnitMMFVidClient::PlayAndStartSchedulerL()
	{
	if (!iVpu2)
		{
		iPlayer->Play();
		}
#ifdef SYMBIAN_BUILD_GCE
	else
		{
		iPlayer2->Play();
		}
#endif
	CActiveScheduler::Start();
	}

	/**
@helper functions
@ open a file and starts the scheduler
@lib "TSU_MMF_VCLNT_01.lib"
*/
void CTestStepUnitMMFVidClient::RecOpenAndStartSchedulerL(TBool aUseFileHandle)
	{
	const TUid KVidTstControllerUid = {KMmfVideoTestControllerUid}; //(0x101F88D9) //{KMmfTestControllerUid}; //(0x101F88D8)
	if (aUseFileHandle)
		{
		User::LeaveIfError(iFs.Connect());
		User::LeaveIfError(iFs.ShareProtected());
		User::LeaveIfError(iFile.Replace(iFs, iFileName,EFileWrite | EFileShareAny));
		iRecorder->OpenFileL(iFile, NULL, KVidTstControllerUid, TUid::Uid(0x101F88DB) ); //KTstControllerUid );
		}
	else
		{
		iRecorder->OpenFileL(iFileName, NULL, KVidTstControllerUid, TUid::Uid(0x101F88DB) ); //KTstControllerUid );
		}
	CActiveScheduler::Start();
	}

/**
@helper functions
@ issues a play and starts the scheduler
@lib "TSU_MMF_VCLNT_01.lib"
*/
void CTestStepUnitMMFVidClient::RecordAndStartScheduler()
	{
	iRecorder->Record();
	CActiveScheduler::Start();
	}

/**
@helper functions
@ issues a prepare and starts the scheduler
@lib "TSU_MMF_VCLNT_01.lib"
*/
void CTestStepUnitMMFVidClient::RecorderPrepareAndStartScheduler()
	{
	iRecorder->Prepare();
	CActiveScheduler::Start();
	}
	
/**
@helper functions
@ issues a prepare and starts the scheduler
@lib "TSU_MMF_VCLNT_01.lib"
*/
void CTestStepUnitMMFVidClient::PlayerPrepareAndStartScheduler()
	{
	if (!iVpu2)
		{
		iPlayer->Prepare();
		}
#ifdef SYMBIAN_BUILD_GCE
	else
		{
		iPlayer2->Prepare();
		}
#endif
	CActiveScheduler::Start();
	}


/**
@return success or not
@ gets the priority settings of the utility
@lib "TSU_MMF_VCLNT_01.lib"
*/
TInt CTestStepUnitMMFVidClient::GetPlayerPriority(TMMFPrioritySettings& aPrioritySettings)
	{
	if (iPlayer && !iVpu2) 
		{
		aPrioritySettings = iPlayer->iBody->iPrioritySettings;
		return KErrNone;
		}
#ifdef SYMBIAN_BUILD_GCE
	if (iPlayer2 && iVpu2)
		{
		aPrioritySettings = iPlayer2->iBody->iPrioritySettings;
		return KErrNone;
		}
#endif
		
	return KErrNotReady;
	}


/**
@return success or not
@ gets the state of the utility
@lib "TSU_MMF_VCLNT_01.lib"
*/
TBool CTestStepUnitMMFVidClient::PlayerStateStopped()
	{
	TBool state = EFalse;
	if (iPlayer && !iVpu2)
		{
		state = iPlayer->iBody->iState == CVideoPlayerUtility::EStopped;
		}
#ifdef SYMBIAN_BUILD_GCE
	if (iPlayer2 )
		{
		state =  iPlayer2->iBody->iState == CVideoPlayerUtility::EStopped;
		}
#endif
	return state; 
	}

TBool CTestStepUnitMMFVidClient::PlayerStateOpening()
	{
	TBool state = EFalse;
	if (iPlayer && !iVpu2)
		{
		state =  iPlayer->iBody->iState == CVideoPlayerUtility::EOpening;
		}
#ifdef SYMBIAN_BUILD_GCE
	if (iPlayer2 )
		{
		state =  iPlayer2->iBody->iState == CVideoPlayerUtility::EOpening;
		}
#endif
	return state ; 
	}

TBool CTestStepUnitMMFVidClient::PlayerStatePaused()
	{
	TBool state = EFalse;
	if (iPlayer && !iVpu2)
		{
		state = iPlayer->iBody->iState == CVideoPlayerUtility::EPaused;
		}
#ifdef SYMBIAN_BUILD_GCE
	if (iPlayer2 )
		{
		state = iPlayer2->iBody->iState == CVideoPlayerUtility::EPaused;
		}
#endif
	return state; 
	}

TBool CTestStepUnitMMFVidClient::PlayerStatePlaying()
	{
	TBool state = EFalse;
	if (iPlayer && !iVpu2)
		{
		state = iPlayer->iBody->iState == CVideoPlayerUtility::EPlaying;
		}
#ifdef SYMBIAN_BUILD_GCE
	if (iPlayer2 )
		{
		state = iPlayer2->iBody->iState == CVideoPlayerUtility::EPlaying;
		}
#endif
	return state; 
	}

#ifdef SYMBIAN_BUILD_GCE
void CTestStepUnitMMFVidClient::SurfaceSupported()
	{
	if (iPlayer && !iVpu2)
		{
		iSurfaceSupported = iPlayer->iBody->iGraphicsSurfaceSupported;
		}
	if (iPlayer2 )
		{
		iSurfaceSupported = iPlayer2->iBody->iGraphicsSurfaceSupported;
		}
	}

TBool CTestStepUnitMMFVidClient::SurfaceCreated()
	{
	TBool surfaceCreated = EFalse;
	if (iPlayer && !iVpu2 && iPlayer->iBody->iActiveDisplays.Count() > 0)
		{
		surfaceCreated = iPlayer->iBody->iActiveDisplays[0]->IsSurfaceCreated();
		}
	if (iPlayer2 && iPlayer2->iBody->iActiveDisplays.Count() > 0)
		{
		surfaceCreated = iPlayer2->iBody->iActiveDisplays[0]->IsSurfaceCreated();
		}
		
	return surfaceCreated;
	}
		
#endif // SYMBIAN_BUILD_GCE

#ifdef SYMBIAN_MULTIMEDIA_SUBTITLE_SUPPORT
TInt CTestStepUnitMMFVidClient::SubtitleCrpDataCount()
	{
	TInt count(0);
	if (iPlayer2 && iPlayer2->iBody && iPlayer2->iBody->iSubtitleUtility)
		{
		count = iPlayer2->iBody->iSubtitleUtility->iCrpDataArray.Count();
		}
	return count;	
	}

// check that the given subtitle data are found in subtitle utility, return ETrue if found
TBool CTestStepUnitMMFVidClient::CheckSubtitleData(TInt aWindowId, TInt aCrpId, const TRect& aCrpRect)
	{
	const RArray<CMMFSubtitleUtility::TCrpData>& crpDataArray = iPlayer2->iBody->iSubtitleUtility->iCrpDataArray;
	TBool found = EFalse;
	for (TInt i = crpDataArray.Count(); --i >= 0; )
		{
		const CMMFSubtitleUtility::TCrpData& data = crpDataArray[i];
		if (data.iWindowId == aWindowId && data.iCrpRect == aCrpRect && data.iCrpId.Id() == aCrpId)
			{
			found = ETrue;
			break;
			}
		}
	
	if (!found)
		{
		ERR_PRINTF1(_L("Unexpected CrpData."));
		}
	
	return found;
	}
#endif //SYMBIAN_MULTIMEDIA_SUBTITLE_SUPPORT

TVerdict CTestStepUnitMMFVidClient::ControllerCalled(const TDesC8& aText, TBool aExpected)
	{
	return DoControllerCalled(KLogFunction, aText, aExpected);
	}

TVerdict CTestStepUnitMMFVidClient::ControllerCalled2(const TDesC8& aText, TBool aExpected)
	{
	return DoControllerCalled(KLogFunction2, aText, aExpected);
	}

TVerdict CTestStepUnitMMFVidClient::DoControllerCalled(TInt aFunction, const TDesC8& aText, TBool aExpected)
	{
	// Get the controller
	TInt err = KErrNone;
	RMMFController* controller = NULL;
	err = GetPlayerController(controller);
	if (err != KErrNone)
		{
		INFO_PRINTF2(_L("Error getting controller %d"), err);
		return EInconclusive;
		}

	// Check that the correct functions were called in the controller
	TBuf8<64> functionText;
	TUid uid = { KMmfVideoTestControllerUid };
	TMMFMessageDestination handleInfo(uid);
	TMMFMessageDestinationPckg message(handleInfo);

	TBuf<64> functionText16;
	err = controller->CustomCommandSync(message, aFunction, KNullDesC8, KNullDesC8, functionText);
	if (err == KErrNone ) 
		{
		functionText16.Copy(aText);
		
		if (aExpected)
			{
			INFO_PRINTF2(_L("Expect %S"), &functionText16);
			}
		else
			{
			INFO_PRINTF2(_L("Should not have %S"), &functionText16);
			}
			
		if ((functionText.Compare(aText) == 0 && aExpected) ||
			(functionText.Compare(aText) != 0 && !aExpected))
			{
			iTestStepResult = EPass;
			}
		else 
			{
			iTestStepResult = EFail;
			return iTestStepResult;
			}
		}
	return iTestStepResult;
	}

/**
@return success or not
@ gets the state of the utility
@lib "TSU_MMF_VCLNT_01.lib"
*/
TBool CTestStepUnitMMFVidClient::RecorderStateStopped()
	{
	ASSERT(iRecorder);
	return iRecorder->iBody->iState == CVideoRecorderUtility::EStopped;
	}

TBool CTestStepUnitMMFVidClient::RecorderStateOpening()
	{
	ASSERT(iRecorder);
	return iRecorder->iBody->iState == CVideoRecorderUtility::EOpening;
	}

TBool CTestStepUnitMMFVidClient::RecorderStatePaused()
	{
	ASSERT(iRecorder);
	return iRecorder->iBody->iState == CVideoRecorderUtility::EPaused;
	}

TBool CTestStepUnitMMFVidClient::RecorderStateRecording()
	{
	ASSERT(iRecorder);
	return iRecorder->iBody->iState == CVideoRecorderUtility::ERecording;
	}

/**
@return success or not
@ gets the clip rectangle value of the utility
@lib "TSU_MMF_VCLNT_01.lib"
*/
TInt CTestStepUnitMMFVidClient::GetPlayerClipRect(TRect& aRect)
	{
	if (iPlayer == NULL && !iVpu2) 
		{
		return KErrNotReady;
		}

	if (!iVpu2)
		{
		aRect = iPlayer->iBody->iClipRect;
		}
#ifdef SYMBIAN_BUILD_GCE		
	if (iPlayer2 == NULL && iVpu2) 
		{
		return KErrNotReady;
		}

	if (iVpu2)
		{
		aRect = (*(iPlayer2->iBody->iActiveDisplays[0]->iWindowsArrayPtr))[0].iClipRect;
		}
#endif	
	return KErrNone;
	}

/**
@return success or not
@ gets the video extent value of the utility
@lib "TSU_MMF_VCLNT_01.lib"
*/
TInt CTestStepUnitMMFVidClient::GetPlayerWindowRect(TRect& aRect)
	{
	if (iPlayer == NULL && !iVpu2) 
		{
		return KErrNotReady;
		}

	if (!iVpu2)
		{
		aRect = iPlayer->iBody->iWindowRect;
		}
		
#ifdef SYMBIAN_BUILD_GCE		
	if (iPlayer2 == NULL && iVpu2) 
		{
		return KErrNotReady;
		}

	if (iVpu2)
		{
		aRect = (*(iPlayer2->iBody->iActiveDisplays[0]->iWindowsArrayPtr))[0].iVideoExtent;
		}
#endif	
	return KErrNone;
	}

/**
@return success or not
@ gets the init screen number of the utility
@lib "TSU_MMF_VCLNT_01.lib"
*/
TInt CTestStepUnitMMFVidClient::GetInitScreenNumber()
	{
	TInt screenNumber = 0;
	if (iPlayer && !iVpu2)
		{
		screenNumber = iPlayer->iBody->iScreenNumber;
		}
	return screenNumber;
	}

/**
@return success or not
@ gets the RMMFController of the utility
@lib "TSU_MMF_VCLNT_01.lib"
*/
TInt CTestStepUnitMMFVidClient::GetPlayerController(RMMFController*& aController)
	{
	if (iPlayer == NULL && !iVpu2) 
		{
		return KErrNotReady;
		}

	if (!iVpu2)
		{
		aController = &iPlayer->iBody->iController;
		}
		
#ifdef SYMBIAN_BUILD_GCE		
	if (iPlayer2 == NULL && iVpu2) 
		{
		return KErrNotReady;
		}

	if (iVpu2)
		{
		aController = &iPlayer2->iBody->iController;
		}
#endif	
	return KErrNone;
	}


/**
@return success or not
@ gets the RMMFController of the utility
@lib "TSU_MMF_VCLNT_01.lib"
*/
TInt CTestStepUnitMMFVidClient::GetRecorderController(RMMFController*& aController)
	{
	if (iRecorder == NULL)
		{
		return KErrNotReady;
		}
	else
		{
		aController = &iRecorder->iBody->iController;
		return KErrNone;
		}
	}

void CTestStepUnitMMFVidClient::InitWservL()
	{
	TInt err = iWs.Connect();
	if (err != KErrNone)
		{
		// Access violation if ws is null
		INFO_PRINTF1(_L("Cannot test, no window server available"));
		User::Leave(err);
		}
	iScreen = new (ELeave) CWsScreenDevice(iWs); // make device for this session
	User::LeaveIfError(iScreen->Construct()); // and complete its construction

	iRootWindow = RWindowGroup(iWs);
	User::LeaveIfError(iRootWindow.Construct((TUint32)this, ETrue));

	iWindow = new(ELeave) RWindow(iWs);
	User::LeaveIfError(((RWindow*)iWindow)->Construct(iRootWindow,((TUint32)(this)) + 1));
	iWindow->SetRequiredDisplayMode(EColor16MAP);
	iWindow->SetExtent(TPoint(0,0), TSize(400,200));
	iWindow->SetVisible(ETrue);
	iWindow->Activate();
	iWs.Flush();
	}

void CTestStepUnitMMFVidClient::DeInitWserv()
	{
	if (iWindow)
		{
		iWindow->Close();
		delete iWindow;
		iWindow = NULL;
		}
	iRootWindow.Close();
	delete iScreen;
	iScreen = NULL;
	iWs.Flush();
	iWs.Close();
	}

void CTestStepUnitMMFVidClient::InitEikonL()
	{
/*	iEikEnv = new(ELeave) CEikonEnv();
	TRAPD(err, iEikEnv->ConstructL(ETrue));		// N.B. this call must be trapped otherwise it panics
	User::LeaveIfError(err);
*/	}

void CTestStepUnitMMFVidClient::DeInitEikon()
	{
/*	if (iEikEnv)
		{
		iEikEnv->DestroyEnvironment();
		iEikEnv = NULL;
		}
*/	}

// ----------------------------------
