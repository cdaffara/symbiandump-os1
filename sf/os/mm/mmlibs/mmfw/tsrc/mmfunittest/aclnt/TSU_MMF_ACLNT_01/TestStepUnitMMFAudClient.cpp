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


// Test system includes
#include "TestStepUnitMMFAudClient.h"
#include "TestSuiteUnitMMFAudClient.h"

#include "mmfclientaudiooutputstream.h"
#include "mmfclientaudioinputstream.h"
#include "mmfclientaudiorecorder.h"
#include "mmfclientaudioplayer.h"
#include "mmfclientaudioconverter.h"
#include "mmfclienttoneplayer.h"
#include "mmfclientutility.h"

const TInt KMaxHeaderSize = 256;
const TUid KUidTestControllerMediaId = {0x101F7D8D};

const TUid KUidTestDummyControllerMediaId = {0x10273825};

/**
 * Standard static NewL() taking a callback function
 */
CCallBackTimer* CCallBackTimer::NewL(TCallBack aCallBack, TPriority aPriority)
	{
	CCallBackTimer* self = new(ELeave) CCallBackTimer(aCallBack, aPriority);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(); // self
	return self;
	}

/**
 * Private c'tor
 */
CCallBackTimer::CCallBackTimer(TCallBack aCallBack, TPriority aPriority)
: CTimer(aPriority),
iCallBack(aCallBack)
	{
	CActiveScheduler::Add(this);
	}

/*
 * Callback on timer complete
 */
void CCallBackTimer::RunL()
	{
	iCallBack.CallBack();
	}


//

/**
 *
 * CTestStepUnitMMFAudClient constructor
 *
 * @xxxx
 * 
 */
CTestStepUnitMMFAudClient::CTestStepUnitMMFAudClient() : 
iScheduler( NULL )
	{
	iRes = &iFileName;
	}

/**
 *
 * CTestStepUnitMMFAudClient destructor
 *
 * @xxxx
 * 
 */
CTestStepUnitMMFAudClient::~CTestStepUnitMMFAudClient()
	{
	}

	// accessors
/**
 *
 * CTestStepUnitMMFAudClient accessor : set suite
 *
 * @param "CTestSuiteUnitMMFAudClient*"
 *		  The test suite.
 *
 * @xxxx
 * 
 */
void CTestStepUnitMMFAudClient::SetUnitMMFAudClientSuite(CTestSuiteUnitMMFAudClient* aSuite)
	{ 
	iUnitMMFAudClientSuite = aSuite;
	}

/**
 *
 * CTestStepUnitMMFAudClient accessor : get suite
 *
 * @return "CTestSuiteUnitMMFAudClient*"
 *		  The test suite.
 *
 * @xxxx
 * 
 */
CTestSuiteUnitMMFAudClient* CTestStepUnitMMFAudClient::UnitMMFAudClientSuite() const
	{
	return iUnitMMFAudClientSuite;
	}

/**
 *
 * get default path
 *
 * @return "TDesC&"
 *		  default path.
 *
 * @xxxx
 * 
 */
const TDesC& CTestStepUnitMMFAudClient::DefaultPath() const
	{
	return iUnitMMFAudClientSuite->DefaultPath();	
	}

/**
DoTestStepPreambleL is the preamble function for test steps
It creates an active scheduler and installs it. This is necessary
since we are multithreading and each test step runs in a separate thread.
@returns TVerdict which is either EPass or Efail
 
The function can leave if there is insufficient memeory

@lib "TSU_MMF_ACLNT_01.lib"
*/
enum TVerdict CTestStepUnitMMFAudClient::DoTestStepPreambleL(void)
	{
	
	iScheduler = new(ELeave)CActiveScheduler;
    //[ push the scheduler on the stack ]
	CleanupStack::PushL( iScheduler );
	
	//[install the active scheduler ]
	CActiveScheduler::Install( iScheduler );
	
	// Pop the element from the stack
	CleanupStack::Pop();
	
	return EPass;
	}

/**
The post amble step cleans up the allocated memory in the preamble
@returns TVerdict EPass or EFail
@leaves can leave due to insufficient memory
@lib "TSU_MMF_ACLNT_01.lib"
*/

enum TVerdict CTestStepUnitMMFAudClient::DoTestStepPostambleL(void)
	{
	
	//[ Destroy the scheduler ]
	delete iScheduler ;
	iScheduler = NULL;

	// XXX: determine whether this is reasonable: allow ECOM, et al to shut down properly
	// it is rather horrible, but it may not be an issue at all (just a side affect of the whole
	// EXE test bed structure
	User::After(1000000); 
	
	return EPass;
	}

TInt CTestStepUnitMMFAudClient::GetAudPlayerState(CMMFMdaAudioPlayerUtility* aPlayer, TInt& aState)
	{
	if (aPlayer == NULL)
		return KErrNotReady;

	aState = aPlayer->iState;
	return KErrNone;
	}

TInt CTestStepUnitMMFAudClient::GetAudPlayerSourceHandle(CMMFMdaAudioPlayerUtility* aPlayer,
														 TMMFMessageDestination* /*aSrcHandle*/)
	{
	if (aPlayer == NULL)
		return KErrNotReady;
	return KErrNone;
	}

TInt CTestStepUnitMMFAudClient::GetAudPlayerPriority(CMMFMdaAudioPlayerUtility* aPlayer,
												     TMMFPrioritySettings& aPrior )
	{
	if (aPlayer == NULL)
		return KErrNotReady;

	aPrior = aPlayer->iPrioritySettings;
	return KErrNone;
	}

TInt CTestStepUnitMMFAudClient::GetAudToneType(CMMFMdaAudioToneUtility* aTone,
											   TInt& aType )
	{
	if (aTone == NULL)
		return KErrNotReady;

	if (aTone->iToneConfig == NULL)
		{
		INFO_PRINTF1(_L("iTone->iToneConfig NULL"));
		}

	aType = aTone->iToneConfig->Type();
	return KErrNone;
	}

// modification of GetAudToneType() for MM-MMF-ACLNT-U-0908-HP (INC087436)
// returns KErrNotFound if iToneConfig == NULL
// this ensures that test does not panic, without altering the other tests
TInt CTestStepUnitMMFAudClient::GetAudToneTypeSafe(CMMFMdaAudioToneUtility* aTone,
											   TInt& aType )
	{
	if (aTone == NULL)
		return KErrNotReady;

	if (aTone->iToneConfig == NULL)
		{
		INFO_PRINTF1(_L("iTone->iToneConfig NULL"));
		return KErrNotFound;
		}

	aType = aTone->iToneConfig->Type();
	return KErrNone;
	}

TInt CTestStepUnitMMFAudClient::GetAudTonePriority(CMMFMdaAudioToneUtility* aTone,
												   TMMFPrioritySettings& aPrior )
	{
	if (aTone == NULL)
		return KErrNotReady;

	aPrior = aTone->iPrioritySettings;
	return KErrNone;
	}

TInt CTestStepUnitMMFAudClient::GetAudOutStreamPlaying(CMMFMdaAudioOutputStream* aAudOutStream,
																TBool& aPlaying)
	{
	if (aAudOutStream == NULL)
		return KErrNotReady;

	aPlaying = (aAudOutStream->iState == CMMFMdaAudioOutputStream::EPlaying);
	return KErrNone;
	}

TInt CTestStepUnitMMFAudClient::GetAudInStreamRecording(CMMFMdaAudioInputStream* aAudInStream,
																TBool& aRecording)
	{
	if (aAudInStream == NULL)
		return KErrNotReady;

	aRecording = (aAudInStream->iState == CMMFMdaAudioInputStream::ERecording);
	return KErrNone;
	}

TInt CTestStepUnitMMFAudClient::GetAudOutStreamPosition(CMMFMdaAudioOutputStream* aAudOutStream,
														TTimeIntervalMicroSeconds& aPosition)
	{
	if (aAudOutStream == NULL)
		return KErrNotReady;

	aPosition = aAudOutStream->iPosition;
	return KErrNone;
	}

TInt CTestStepUnitMMFAudClient::GetAudInStreamPosition(CMMFMdaAudioInputStream* aAudInStream,
														TTimeIntervalMicroSeconds& aPosition)
	{
	if (aAudInStream == NULL)
		return KErrNotReady;

	aPosition = aAudInStream->iPosition;
	return KErrNone;
	}

TInt CTestStepUnitMMFAudClient::GetAudOutStreamStopped(CMMFMdaAudioOutputStream* aAudOutStream,
																TBool& aStopped)
	{
	if (aAudOutStream == NULL)
		return KErrNotReady;

	aStopped = (aAudOutStream->iState == CMMFMdaAudioOutputStream::EStopped);
	return KErrNone;
	}
		
TInt CTestStepUnitMMFAudClient::GetAudInStreamStopped(CMMFMdaAudioInputStream* aAudInStream,
																TBool& aStopped)
	{
	if (aAudInStream == NULL)
		return KErrNotReady;

	aStopped = (aAudInStream->iState == CMMFMdaAudioInputStream::EStopped);
	return KErrNone;
	}
		
TInt CTestStepUnitMMFAudClient::GetAudRecorderPrioritySettings(CMMFMdaAudioRecorderUtility* aAudRec,
																TMMFPrioritySettings& aPrior)
	{
	if (aAudRec == NULL)
		return KErrNotReady;

	aPrior = static_cast<CMMFMdaAudioRecorderUtility*>(aAudRec)->iPrioritySettings;
	return KErrNone;
	}
		
TInt CTestStepUnitMMFAudClient::GetAudRecorderRecTimeAvail(CMMFMdaAudioRecorderUtility* aAudRec,
														   TTimeIntervalMicroSeconds& aTime)
	{
	if (aAudRec == NULL)
		return KErrNotReady;

	aTime = static_cast<CMMFMdaAudioRecorderUtility*>(aAudRec)->iRecordTimeAvailable;
	return KErrNone;
	}

TInt CTestStepUnitMMFAudClient::SetAudRecorderParent(CMMFMdaAudioRecorderUtility* aUtility, CMdaAudioRecorderUtility* aParent)
	{
	if (aUtility == NULL)
		return KErrNotReady;

	aUtility->iParent = aParent;
	return KErrNone;
	}
	
void CTestStepUnitMMFAudClient::SetTestMediaIdL(CMMFMdaAudioPlayerUtility* aPlayer)
	{
	aPlayer->iMediaIds.Reset();
	aPlayer->iMediaIds.Append(KUidTestControllerMediaId);
	aPlayer->iFindAndOpenController->Configure(aPlayer->iMediaIds[0], aPlayer->iPrioritySettings);
	}
	
void CTestStepUnitMMFAudClient::SetTestMediaIdL(CMMFMdaAudioRecorderUtility* aRecorder)
	{
	aRecorder->iMediaIds.Reset();
	aRecorder->iMediaIds.Append(KUidTestControllerMediaId);
	aRecorder->iFindAndOpenController->Configure(aRecorder->iMediaIds[0], aRecorder->iPrioritySettings);
	}

void CTestStepUnitMMFAudClient::SetTestMediaIdL(CMMFMdaAudioConvertUtility* aConverter)
	{
	aConverter->iMediaIds.Reset();
	aConverter->iMediaIds.Append(KUidTestControllerMediaId);
	aConverter->iFindAndOpenController->Configure(aConverter->iMediaIds[0], aConverter->iPrioritySettings);
	}

	
TInt CTestStepUnitMMFAudClient::OpenFileWithTestControllerL(CMMFMdaAudioPlayerUtility* aPlayer, TDesC& aFileName)
	{
	RMMFControllerImplInfoArray controllers;
	CleanupResetAndDestroyPushL(controllers);
	CMMFControllerPluginSelectionParameters* cSelect = CMMFControllerPluginSelectionParameters::NewLC();
	CMMFFormatSelectionParameters* fSelect = CMMFFormatSelectionParameters::NewLC();

	// Select the media IDs to allow
	RArray<TUid> mediaIds;
	CleanupClosePushL(mediaIds);
	mediaIds.Append(KUidMediaTypeAudio);
	cSelect->SetMediaIdsL(mediaIds,CMMFPluginSelectionParameters::ENoMediaIdMatch);
	CleanupStack::PopAndDestroy();//mediaIds


	fSelect->SetMatchToFileNameL(aFileName);
	cSelect->SetRequiredPlayFormatSupportL(*fSelect);
	cSelect->ListImplementationsL(controllers);


	// if we have found no controllers, we will try a match to header data.
	if (controllers.Count()==0)
		{
		HBufC8* headerBuffer = HBufC8::NewLC(KMaxHeaderSize);
		TPtr8 des = headerBuffer->Des();
		TMMFileSource fileSource(aFileName);
		CMMFUtilityFileInfo* fileInfo = CMMFUtilityFileInfo::NewLC(fileSource);
		fileInfo->GetFileHeaderDataL(des, KMaxHeaderSize);
		fSelect->SetMatchToHeaderDataL(des);
		cSelect->SetRequiredPlayFormatSupportL(*fSelect);
		cSelect->ListImplementationsL(controllers);
		CleanupStack::PopAndDestroy(2, headerBuffer); // headerBuffer, fileInfo

		// if we still don't have a match, leave with unsupported
		if (controllers.Count()==0)
			User::Leave(KErrNotSupported);
		}

	// Check whether file actually exists
	RFs fsSession;
	CleanupClosePushL(fsSession);
	User::LeaveIfError(fsSession.Connect());
	TEntry entry;
	if (fsSession.Entry(aFileName, entry) != KErrNone)
		User::Leave(KErrNotFound);
	CleanupStack::PopAndDestroy();	//fsSession

	// Open and configure a controller
	
	TInt err = KErrNotSupported;
	for (TInt n=0; n<controllers.Count(); n++)
		{
		aPlayer->iControllerUid = KNullUid;
		aPlayer->iFindAndOpenController->ConfigureSourceSink(
			CMMFFindAndOpenController::TSourceSink(KUidMmfFileSource,CMMFFindAndOpenController::GetConfigFile(aFileName)),
			CMMFFindAndOpenController::TSourceSink(KUidMmfAudioOutput));
		aPlayer->iFindAndOpenController->OpenByControllerUid(controllers[n]->Uid());
		CActiveScheduler::Start(); 
		if (aPlayer->iControllerUid != KNullUid)
			{
			err = KErrNone;
			break;
			}
		}

	// Clean up
	CleanupStack::PopAndDestroy(3);//controllers, cSelect, fSelect
	return err;
	}
	
void CTestStepUnitMMFAudClient::SetTestDummyMediaIdL(CMMFMdaAudioPlayerUtility* aPlayer)
	{
	if(!aPlayer)
		{
		User::Leave(KErrArgument);
		}
	
	aPlayer->iMediaIds.Reset();
	aPlayer->iMediaIds.Append(KUidTestDummyControllerMediaId);
	aPlayer->iFindAndOpenController->Configure(aPlayer->iMediaIds[0], aPlayer->iPrioritySettings);
	
	}
	
void CTestStepUnitMMFAudClient::SetTestDummyMediaIdL(CMMFMdaAudioRecorderUtility* aRecorder)
	{
	if(!aRecorder)
		{
		User::Leave(KErrArgument);
		}
	aRecorder->iMediaIds.Reset();
	aRecorder->iMediaIds.Append(KUidTestDummyControllerMediaId);
	aRecorder->iFindAndOpenController->Configure(aRecorder->iMediaIds[0], aRecorder->iPrioritySettings);
	}

// Test for DEF113412:CMMFFindAndOpenController::SendError enters infinite loop on.
CTsuClientUtilityControllerLoading* CTsuClientUtilityControllerLoading::NewL(const TDesC& aStepName, const TDesC& aFileName)
	{
	CTsuClientUtilityControllerLoading* self = new(ELeave)CTsuClientUtilityControllerLoading(aStepName, aFileName);	
	return self;
	}

CTsuClientUtilityControllerLoading::CTsuClientUtilityControllerLoading(const TDesC& aStepName, const TDesC& aFileName) :
	iFileName(aFileName), iUtility(NULL)
	{
	iTestStepName = aStepName;	
	}

void CTsuClientUtilityControllerLoading::MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& /*aDuration*/)
	{
	if(aError != KErrNone)		
		{
		StopTest(aError, EFail)	;
		}
	else
		{
		StopTest(aError);
		}
	}
void CTsuClientUtilityControllerLoading::MapcPlayComplete(TInt aError)
	{
	StopTest(aError);	
	}

void CTsuClientUtilityControllerLoading::KickoffTestL()
	{
	User::LeaveIfError(iFs.Connect());
	INFO_PRINTF1(_L("Test for - DEF113412:CMMFFindAndOpenController::SendError enters infinite loop on "));
	
	//create the source file to open for play
	iFs.Delete(iFileName);
	RFile file;
	User::LeaveIfError(file.Create(iFs, iFileName, EFileWrite));
	CleanupClosePushL(file);
	//take a buffer that can accomodate the header "loadingtest" with null character
	TBuf8<25> buf; 
	buf.Format(_L8("loadingtest%d"),TUint8(0x00));
	file.Write(buf);
	file.Close();
	CleanupStack::PopAndDestroy();

	//create audio player and open it
	iUtility = CMdaAudioPlayerUtility::NewL(*this);
	INFO_PRINTF1(_L("Calling iUtility->OpenFileL()"));
	iUtility->OpenFileL(iFileName);
	}

void CTsuClientUtilityControllerLoading::CloseTest()
	{
	iFs.Delete(iFileName);
	delete iUtility;
	iUtility = NULL;
	iFs.Close();	
	}
