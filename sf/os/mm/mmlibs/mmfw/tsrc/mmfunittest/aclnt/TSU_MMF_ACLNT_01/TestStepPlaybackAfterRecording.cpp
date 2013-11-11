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
// This file contains an example Test step implementation 
// This demonstrates the various functions provided
// by the CTestStep base class which are available within
// a test step 
// 
//

// EPOC includes
#include <f32file.h>
#include <testframework.h>

#include <mdaaudiosampleeditor.h>

// Specific includes for this test suite
#include "TestStepUnitMMFAudClient.h"
#include "TestSuiteUnitMMFAudClient.h"

// Specific includes for these test steps
#include "TestStepRecorder.h"


#include "mmfclientaudiorecorder.h"


NONSHARABLE_CLASS( CRecPlayMuteTest ): public CActive, public MMdaObjectStateChangeObserver {
public:
static	CRecPlayMuteTest*	NewL(const TDesC& aFile);
		~CRecPlayMuteTest();
protected:
			CRecPlayMuteTest():CActive(EPriorityIdle)
			{
			CActiveScheduler::Add(this);
			}
	void MoscoStateChangeEvent(CBase* aObject, TInt aPreviousState, TInt aCurrentState, TInt aErrorCode);

	void OpenClipL(const TDesC& aFile);
	void RecordL();
	void PlayL();
// CActive //
	TInt RunError(TInt aError);
	void RunL();
	void DoCancel() 
		{
		iTimer.Cancel(); 
		iStatus=KErrCancel;
		}
	void RunAfter(TInt aMilisec);
	void RunAgain()
		{
		SetActive();
		TRequestStatus* p=&iStatus;
		User::RequestComplete(p, KErrNone);
		}
public:
	TInt	iError;
private:
	enum {KSubjectVolume=1, KSubjectBalance=2};
	CMdaAudioRecorderUtility*	iRecorderUtility;
	TFileName					iFileName;
	RTimer						iTimer;
	enum TState {
			eOpen, eResponse, eRecord, eRecStop, ePlay, eCheckVolume, eTerminate
		}						iState;
};


// --------------------------------------------

/**
 *
 * Static constructor for CTestStepRecorderPlayAfterRecording.
 *
 * INC047838 Voice Recorder: sound is played on the newly recorded clip while mute was
 *
 * @return	"CTestStepRecorderPlayAfterRecording*"
 *			The constructed CTestStepRecorderPlayAfterRecording
 *
 * @xxxx
 * 
 */
CTestStepRecorderPlayAfterRecording* CTestStepRecorderPlayAfterRecording::NewL()
	{
	CTestStepRecorderPlayAfterRecording* self = new(ELeave) CTestStepRecorderPlayAfterRecording;
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
CTestStepRecorderPlayAfterRecording::CTestStepRecorderPlayAfterRecording()
	{
	iTestStepResult = EPass;
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-ACLNT-U-0855-HP");
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 * 
 */
CTestStepRecorderPlayAfterRecording::~CTestStepRecorderPlayAfterRecording()
	{
	}

/**
 *
 * Test step Preamble.
 *
 * @xxxx
 * 
 */
 enum TVerdict CTestStepRecorderPlayAfterRecording::DoTestStepPreambleL(void)
	{
	 enum TVerdict verdict;
	 // this installs the scheduler
	 verdict = CTestStepUnitMMFAudClient::DoTestStepPreambleL();

	// Printing to the console and log file
	 INFO_PRINTF1(_L("MM-MMF-ACLNT-U-0855-HP"));
	 INFO_PRINTF1(_L("To test the INC047838: Voice Recorder: sound is played on the newly recorded clip while mute was"));

	 iError = KErrNone; 

	 _LIT(KFileName, "AudioPlayRecName");
	 if(!GetStringFromConfig(_L("SectionOne"), KFileName, iFileName) )
		{
		 INFO_PRINTF2(_L("file name %s not found..."), KFileName);
		 return EInconclusive;
		}

	iTestObject = CRecPlayMuteTest::NewL(iFileName);

	 return verdict;
	}

/**
 *
 * Test step Postamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepRecorderPlayAfterRecording::DoTestStepPostambleL(void)
	{
	delete iTestObject;
	iTestObject = NULL;
	//[ Destroy the scheduler ]
	return CTestStepUnitMMFAudClient::DoTestStepPostambleL();
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
TVerdict CTestStepRecorderPlayAfterRecording::DoTestStepL()
	{
	iTestStepResult = EPass;

	CActiveScheduler::Start();

	INFO_PRINTF2(_L("finished with this test step err=%d"), iTestObject->iError);
	if (KErrNone != iTestObject->iError)
		iTestStepResult = EFail;
	// test steps return a result
	return iTestStepResult;
	}



/*static*/
CRecPlayMuteTest* CRecPlayMuteTest::NewL(const TDesC& aFile)
	{
	CRecPlayMuteTest* self= new (ELeave) CRecPlayMuteTest();
	CleanupStack::PushL(self);
	self->iFileName.Copy(aFile);

	self->iRecorderUtility = CMdaAudioRecorderUtility::NewL(*self);

	User::LeaveIfError( self->iTimer.CreateLocal() );
	CleanupStack::Pop(self);
	self->RunAgain();
	return self;
	}

CRecPlayMuteTest::~CRecPlayMuteTest()
	{
	iTimer.Close();
	delete iRecorderUtility;
	}

void CRecPlayMuteTest::MoscoStateChangeEvent(CBase* /*aObject*/, TInt /*aPreviousState*/, TInt /*aState*/, TInt aErrorCode)
	{
	if (aErrorCode!=KErrNone)
		{
		iError = aErrorCode;
		iState = eTerminate;
		RunAgain();
		}
	else 
		{
		if (iState == eResponse)
			RunAgain();
		}
	}

void CRecPlayMuteTest::RunAfter(TInt aMilisec)
	{
	iTimer.After(iStatus, TTimeIntervalMicroSeconds32(aMilisec));
	SetActive();
	}

void CRecPlayMuteTest::OpenClipL(const TDesC& aFile)
	{
	iRecorderUtility->OpenFileL(aFile) ;
	}

void CRecPlayMuteTest::RecordL()
	{
	User::LeaveIfError(iRecorderUtility->SetVolume(KSubjectVolume));
	iRecorderUtility->SetGain(iRecorderUtility->MaxGain());  //sets the current gain
    iRecorderUtility->RecordL();
	iState=TState(iState+1);
	RunAfter(1500000);
	}

void CRecPlayMuteTest::PlayL()
	{
	iRecorderUtility->SetPosition(TTimeIntervalMicroSeconds(0));
	User::LeaveIfError(iRecorderUtility->SetVolume(KSubjectVolume));
	iRecorderUtility->SetVolumeRamp(TTimeIntervalMicroSeconds(3));
	iRecorderUtility->SetPlaybackBalance(KSubjectBalance);
    iRecorderUtility->PlayL();

	RunAfter(1500000);
	iState=TState(iState+1);
	}

TInt CRecPlayMuteTest::RunError(TInt aError)
	{
	iError = aError;
	iState=eTerminate;
	RunAgain();
	return KErrNone;
	}

void CRecPlayMuteTest::RunL()
	{
	iTimer.Cancel();
	switch (iState)
		{
		case eOpen:
			OpenClipL(iFileName);
			iState=TState(iState+1);
			break;

		case eResponse:
			iState=TState(iState+1);
			RunAgain();
			break;

		case eRecord:
			RecordL();
			break;

		case eRecStop:
			iRecorderUtility->Stop();
			iState=TState(iState+1);
			RunAgain();
			break;

		case ePlay:
			PlayL();
			break;

		case eCheckVolume:
			TInt volume; 
			User::LeaveIfError( iRecorderUtility->GetVolume(volume) );
			if (volume != KSubjectVolume )
				{
				User::Leave(KErrAbort);
				}
			TInt balance;
			User::LeaveIfError( iRecorderUtility->GetPlaybackBalance(balance) );
			if (balance!=KSubjectBalance)
				{
				User::Leave(KErrAbort);
				}
			RunAfter(500000);
			iState=TState(iState+1);
			break;

		case eTerminate:
			iRecorderUtility->Stop();
			CActiveScheduler::Stop();
			return;
		}
	}

// --------------------------------------------

/**
 *
 *
 * INC060658  CMdaAudioRecorderUtility::OpenDesL fails with KErrNotFound
 *
 * 
 */

class CRecUtilDesOpenTest: public CActive, public MMdaObjectStateChangeObserver {
public:
static	CRecUtilDesOpenTest*	NewL();
		~CRecUtilDesOpenTest();
protected:
			CRecUtilDesOpenTest():CActive(EPriorityIdle)
			{
			CActiveScheduler::Add(this);
			}
	void ConstructL();
	void MoscoStateChangeEvent(CBase* aObject, TInt aPreviousState, TInt aCurrentState, TInt aErrorCode);

// CActive //
	TInt RunError(TInt aError);
	void RunL();
	void DoCancel() 
		{
		iStatus=KErrCancel;
		}
	void RunAgain()
		{
		SetActive();
		TRequestStatus* p=&iStatus;
		User::RequestComplete(p, KErrNone);
		}
public:
	TInt	iError;
private:
	TBool iControllerTested;
	CMdaAudioRecorderUtility*	iRecorderUtility;
	RMMFControllerImplInfoArray iControllers;
	TInt iControllerIdx;
	TBuf8<256>	iDummyBuf;
	enum TState 
		{
		EOpen=0, 
		EResponse, 
		ETerminate
		}						iState;
};

CRecUtilDesOpenTest* CRecUtilDesOpenTest::NewL()
	{
	CRecUtilDesOpenTest* self= new (ELeave) CRecUtilDesOpenTest();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	self->RunAgain();
	return self;
	}

_LIT(KWavExt, "wav");

void CRecUtilDesOpenTest::ConstructL()
	{
	iRecorderUtility = CMdaAudioRecorderUtility::NewL(*this);
	
	CMMFControllerPluginSelectionParameters* controllerSelection = NULL;
	CMMFFormatSelectionParameters* formatSelect = NULL;

	controllerSelection = CMMFControllerPluginSelectionParameters::NewLC();

	RArray<TUid> mediaIds; //Array to hold media types we wish to support
	CleanupClosePushL(mediaIds);
	mediaIds.AppendL(KUidMediaTypeAudio);

	controllerSelection->SetMediaIdsL(
		mediaIds, CMMFPluginSelectionParameters::EAllowOnlySuppliedMediaIds);

	formatSelect = CMMFFormatSelectionParameters::NewLC();
	
	formatSelect->SetMatchToFileNameL(KWavExt);

	controllerSelection->SetRequiredRecordFormatSupportL(*formatSelect);

	controllerSelection->ListImplementationsL(iControllers);

	CleanupStack::PopAndDestroy(3, controllerSelection);
	}

CRecUtilDesOpenTest::~CRecUtilDesOpenTest()
	{
	delete iRecorderUtility;
	}

void CRecUtilDesOpenTest::RunL()
	{
	switch (iState)
		{
		case EOpen:
			{
			CMMFControllerImplementationInformation* impl=iControllers[iControllerIdx];
			const RMMFFormatImplInfoArray& formats=impl->RecordFormats();
			TInt i;
			CMMFFormatImplementationInformation* fmt=NULL;
			for (i=0; i<formats.Count(); ++i)
				{
				fmt=formats[i];
				if (fmt->SupportsFileExtension(_L8(".wav")))
					{
					break;
					}
				}
			if (i==formats.Count())
				{
				iState = ++iControllerIdx < iControllers.Count() ? EOpen : ETerminate;
				RunAgain();
				}
			else
				{
				iRecorderUtility->OpenDesL(iDummyBuf,impl->Uid(), KNullUid, fmt->Uid());
				iControllerTested=ETrue;
				iState = EResponse;
				}
			}
			break;
		case ETerminate:
			if (!iControllerTested)
				{
				iError = KErrGeneral;
				}
			iRecorderUtility->Stop();
			CActiveScheduler::Stop();
			break;
		default:
			ASSERT(EFalse);
		}
	}
	
TInt CRecUtilDesOpenTest::RunError(TInt aError)
	{
	iError = aError;
	iRecorderUtility->Stop();
	CActiveScheduler::Stop();
	return KErrNone;
	}

void CRecUtilDesOpenTest::MoscoStateChangeEvent(CBase* /*aObject*/, TInt /*aPreviousState*/, TInt /*aState*/, TInt aErrorCode)
	{
	if (aErrorCode!=KErrNone)
		{
		iError = aErrorCode;
		iState = ETerminate;
		RunAgain();
		}
	else 
		{
		if (iState == EResponse)
			{
			iState = ++iControllerIdx < iControllers.Count() ? EOpen : ETerminate;
			RunAgain();
			}
		}
	}

CTestStepRecUtilOpenDesByUid* CTestStepRecUtilOpenDesByUid::NewL()
	{
	return new(ELeave) CTestStepRecUtilOpenDesByUid();
	}

/**
 *
 * Test step constructor.
 * Each test step initialises its own name.
 *
 * @xxxx
 * 
 */
CTestStepRecUtilOpenDesByUid::CTestStepRecUtilOpenDesByUid()
	{
	iTestStepResult = EPass;
	// store the name of this test case
	// this is the name that is used by the script file
	iTestStepName = _L("MM-MMF-ACLNT-U-0950-HP");
	}

/**
 *
 * Test step destructor.
 *
 * @xxxx
 * 
 */
CTestStepRecUtilOpenDesByUid::~CTestStepRecUtilOpenDesByUid()
	{
	}

enum TVerdict CTestStepRecUtilOpenDesByUid::DoTestStepPreambleL(void)
	{
	enum TVerdict verdict;
	 // this installs the scheduler
	verdict = CTestStepUnitMMFAudClient::DoTestStepPreambleL();

	// Printing to the console and log file
	INFO_PRINTF1(_L("MM-MMF-ACLNT-U-0950-HP"));
	INFO_PRINTF1(_L("To test the INC060658  CMdaAudioRecorderUtility::OpenDesL fails with KErrNotFound "));

	iError = KErrNone; 
	iTestObject = CRecUtilDesOpenTest::NewL();

	return verdict;
	}

/**
 *
 * Test step Postamble.
 *
 * @xxxx
 * 
 */
enum TVerdict CTestStepRecUtilOpenDesByUid::DoTestStepPostambleL(void)
	{
	delete iTestObject;
	iTestObject = NULL;
	//[ Destroy the scheduler ]
	return CTestStepUnitMMFAudClient::DoTestStepPostambleL();
	}
/**
 *
 *
 * @return	"TVerdict"
 *			The result of the test step
 *
 * 
 */
TVerdict CTestStepRecUtilOpenDesByUid::DoTestStepL()
	{
	iTestStepResult = EFail;
	CActiveScheduler::Start();
	INFO_PRINTF2(_L("finished with this test step err=%d"), iTestObject->iError);
	if (KErrNone == iTestObject->iError)
		{
		iTestStepResult = EPass;
		}
	// test steps return a result
	return iTestStepResult;
	}


/**
 *
 * DEF061942  KERN-EXEC:15 in the AudioPolicy server
 *
 */
/*static*/
CTestStepRecorderNotifyCheck* CTestStepRecorderNotifyCheck::NewL()
	{
	return new (ELeave) CTestStepRecorderNotifyCheck();
	}
	
CTestStepRecorderNotifyCheck::~CTestStepRecorderNotifyCheck()
	{
	}

CTestStepRecorderNotifyCheck::CTestStepRecorderNotifyCheck()
	{
	iTestStepResult = EPass;
	iTestStepName = _L("MM-MMF-ACLNT-U-0951-HP");
	}
	
TVerdict CTestStepRecorderNotifyCheck::DoTestStepPreambleL()
	{
	TVerdict verdict=EFail;
	 // this installs the scheduler
	if (EPass != (verdict = CTestStepUnitMMFAudClient::DoTestStepPreambleL()))
		{
		return verdict;
		}
	iRecorderUtility = CMdaAudioRecorderUtility::NewL(*this);
	_LIT(KFileName, "AudioPlayRecName");
	if(!GetStringFromConfig(_L("SectionOne"), KFileName, iFileName) )
		{
		INFO_PRINTF2(_L("file name %s not found..."), KFileName);
		return EInconclusive;
		}
	return verdict;
	}
	
TVerdict CTestStepRecorderNotifyCheck::DoTestStepPostambleL()
	{
	delete iRecorderUtility; iRecorderUtility = NULL;
	return CTestStepUnitMMFAudClient::DoTestStepPostambleL();
	}
	
TVerdict CTestStepRecorderNotifyCheck::DoTestStepL()
	{
	iTestStepResult = EFail;
	INFO_PRINTF1(iTestStepName);
	iRecorderUtility->OpenFileL(iFileName);
	CActiveScheduler::Start();
	User::LeaveIfError(iRecorderUtility->RegisterAudioResourceNotification(*this, KMMFEventCategoryAudioResourceAvailable));
	iRecorderUtility->PlayL();
	CActiveScheduler::Start();
	iRecorderUtility->Stop();
	iRecorderUtility->PlayL();
	CActiveScheduler::Start();	
	return (iTestStepResult=(iError==KErrNone? EPass : EFail));
	}

// from MMdaObjectStateChangeObserver
void CTestStepRecorderNotifyCheck::MoscoStateChangeEvent(CBase* /*aObject*/, TInt /*aPreviousState*/, TInt /*aCurrentState*/, TInt aErrorcCode)
	{
	if (aErrorcCode != KErrNone)
		{
		iError = aErrorcCode;
		iTestStepResult = EFail;
		}
	CActiveScheduler::Stop();
	}

void CTestStepRecorderNotifyCheck::MarncResourceAvailable(TUid /*aNotificationEventId*/, const TDesC8& /*aNotificationData*/)
	{
	}
	
/**
 *
 * DEF061821  - CMdaAudioRecorderUtility::PlayL() fail case when registered for notification 
 *
 */
/*static*/
CTestStepNotifyRegister* CTestStepNotifyRegister::NewL()
	{
	return new (ELeave) CTestStepNotifyRegister();
	}
	
CTestStepNotifyRegister::~CTestStepNotifyRegister()
	{
	}

CTestStepNotifyRegister::CTestStepNotifyRegister()
	{
	iTestStepResult = EPass;
	iTestStepName 	= _L("MM-MMF-ACLNT-U-0952-HP");
	iHeapSize 		= 1024*512;
	}
	
TVerdict CTestStepNotifyRegister::DoTestStepPreambleL()
	{
	TVerdict verdict=EFail;
	 // this installs the scheduler
	if (EPass != (verdict = CTestStepUnitMMFAudClient::DoTestStepPreambleL()))
		{
		return verdict;
		}
	_LIT(KFileName, "AudioPlayRecName");
	if(!GetStringFromConfig(_L("SectionOne"), KFileName, iFileName) )
		{
		INFO_PRINTF2(_L("file name %s not found..."), KFileName);
		return EInconclusive;
		}
	return verdict;
	}
	
TVerdict CTestStepNotifyRegister::DoTestStepPostambleL()
	{
	delete iPlayUtility;	
	iPlayUtility=NULL;
	return CTestStepUnitMMFAudClient::DoTestStepPostambleL();
	}
	
TVerdict CTestStepNotifyRegister::DoTestStepL()
	{
	INFO_PRINTF1(iTestStepName);
	iTestStepResult = EFail;

	if (DoClipUtilStepL()!=EPass)
		{
		return EFail;
		}
		
	if (DoStreamUtilStepL()!=EPass)
		{
		return EFail;
		}		
		
	return (iTestStepResult = EPass );
	}

TVerdict CTestStepNotifyRegister::DoStreamUtilStepL()
	{
	CMdaAudioOutputStream* stream=CMdaAudioOutputStream::NewL(*this);
	iPlayUtility = stream;
	User::LeaveIfError( stream->RegisterAudioResourceNotification(*this, KMMFEventCategoryAudioResourceAvailable));
	HBufC8* buf=HBufC8::NewMaxLC(1024*64);
	TPtr8 bf(buf->Des());
	for (TInt i=bf.Length();--i;)
		{
		bf[i]=TText8(i & 0xff);
		}
	stream->Open(NULL);
	CActiveScheduler::Start();
	stream->WriteL(bf);
	CActiveScheduler::Start();
	stream->Stop();
	stream->WriteL(bf);
	CActiveScheduler::Start();	
	CleanupStack::PopAndDestroy(buf);
	delete iPlayUtility;
	iPlayUtility = NULL;
	return iError==KErrNone? EPass : EFail;
	}
	
TVerdict CTestStepNotifyRegister::DoClipUtilStepL()	
	{
	CMdaAudioRecorderUtility* rec=CMdaAudioRecorderUtility::NewL(*this);
	iPlayUtility = rec;
	User::LeaveIfError(rec->RegisterAudioResourceNotification(*this, KMMFEventCategoryAudioResourceAvailable));
	rec->OpenFileL(iFileName);	
	CActiveScheduler::Start();
	rec->PlayL();
	CActiveScheduler::Start();
	rec->Stop();
	rec->PlayL();
	CActiveScheduler::Start();	
	delete iPlayUtility;
	iPlayUtility = NULL;
		
	if (iError!=KErrNone)
		{
		INFO_PRINTF2(_L("CMdaAudioRecorderUtility test failed err=%d"),iError);
		return EFail;
		}

	CMdaAudioPlayerUtility* play=CMdaAudioPlayerUtility::NewL(*this);
	iPlayUtility = play;
	User::LeaveIfError(play->RegisterAudioResourceNotification(*this, KMMFEventCategoryAudioResourceAvailable));		
	play->OpenFileL(iFileName);	
	CActiveScheduler::Start();
	play->Play();
	CActiveScheduler::Start();
	play->Stop();
	play->Play();
	CActiveScheduler::Start();	
	delete iPlayUtility;
	iPlayUtility = NULL;
		
	if (iError!=KErrNone)
		{
		INFO_PRINTF2(_L("CMdaAudioPlayerUtility test failed err=%d"),iError);
		return EFail;
		}
	
	return EPass;
	}
	
// from MMdaObjectStateChangeObserver
void CTestStepNotifyRegister::MoscoStateChangeEvent(CBase* /*aObject*/, TInt /*aPreviousState*/, TInt /*aCurrentState*/, TInt aErrorcCode)
	{
	if (aErrorcCode != KErrNone)
		{
		iError = aErrorcCode;
		iTestStepResult = EFail;
		}
	CActiveScheduler::Stop();
	}

void CTestStepNotifyRegister::MarncResourceAvailable(TUid /*aNotificationEventId*/, const TDesC8& /*aNotificationData*/)
	{
	}
	
void CTestStepNotifyRegister::MaoscOpenComplete(TInt aErrorcCode)
	{
	if (aErrorcCode != KErrNone)
		{
		iError = aErrorcCode;
		iTestStepResult = EFail;
		}
	CActiveScheduler::Stop();	
	}
	
void CTestStepNotifyRegister::MaoscBufferCopied(TInt aErrorcCode, const TDesC8& /*aBuffer*/)
	{
	if (aErrorcCode != KErrNone)
		{
		iError = aErrorcCode;
		iTestStepResult = EFail;
		}
	CActiveScheduler::Stop();	
	}
	
void CTestStepNotifyRegister::MaoscPlayComplete(TInt aErrorcCode)
	{
	if (aErrorcCode!=KErrCancel)
		{
		if (aErrorcCode != KErrNone)
			{
			iError = aErrorcCode;
			iTestStepResult = EFail;
			}
		CActiveScheduler::Stop();	
		}
	}
	

void CTestStepNotifyRegister::MapcInitComplete(TInt aErrorcCode, const TTimeIntervalMicroSeconds& /*aDuration*/)
	{
	if (aErrorcCode != KErrNone)
		{
		iError = aErrorcCode;
		iTestStepResult = EFail;
		}
	CActiveScheduler::Stop();	
	}
	
void CTestStepNotifyRegister::MapcPlayComplete(TInt aErrorcCode)
	{
	if (aErrorcCode != KErrNone)
		{
		iError = aErrorcCode;
		iTestStepResult = EFail;
		}
	CActiveScheduler::Stop();
	}

