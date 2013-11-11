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

#include <testframework.h>

#include "TSU_MMFMIDICLNT.h"
#include "TS_MMFMIDICLNTsuite.h"

#include "TestMidiClientUtility.h"

_LIT(KDrmSectName, "SectionDRM");
_LIT(KDrmFileKey, "MidiDrmContent");

const TInt K1Second		=1000000;
const TInt K5Seconds	= 5 * K1Second;
const TInt K30Seconds	=30 * K1Second;

class CCallbackTimer : public CTimer
   	{
public:
   	static CCallbackTimer* NewL();
   	~CCallbackTimer();
   	void SetCb(TCallBack aCallback)
   		{
   		iCallback = aCallback;
   		}
	void After(TTimeIntervalMicroSeconds32 aCancelDelay)
		{
		CTimer::After(aCancelDelay);
		}
	void After(TTimeIntervalMicroSeconds32 aCancelDelay, TCallBack aCallback);

private:
   CCallbackTimer();
   // from CActive
    void RunL();

private:
    TCallBack iCallback; 
    };
    
CCallbackTimer* CCallbackTimer::NewL()
    {
	CCallbackTimer* self=new (ELeave) CCallbackTimer();
   	CleanupStack::PushL(self);
   	self->ConstructL();
   	CleanupStack::Pop();
    return self;
    }
    
CCallbackTimer::CCallbackTimer():CTimer(EPriorityHigh)
	{
	CActiveScheduler::Add(this);
	}

CCallbackTimer::~CCallbackTimer()
    {
    Cancel();
    }


void CCallbackTimer::After(TTimeIntervalMicroSeconds32 aCancelDelay, TCallBack aCallback)
    {
   	iCallback = aCallback;   
   	CTimer::After(aCancelDelay);
    }

/**
*
* RunL
*
**/
void CCallbackTimer::RunL()
    {
    iCallback.CallBack();
    }



CTestStepDrm::CTestStepDrm(const TDesC& aTestName, const TTestStepType aTestType, const TDrmTestParams& aDrmParams):
								CTestMmfMidiClntStep(aTestName, aTestType),
								iDrmParams(aDrmParams)
	{
	}

TVerdict CTestStepDrm::DoTestStepPreambleL()
	{
	if (EPass==CTestMmfMidiClntStep::DoTestStepPreambleL())
		{
		iTimer = CCallbackTimer::NewL();
		iTimer->SetCb(TCallBack(TimerGate, this));
		User::LeaveIfError( iFs.Connect() );
		return EPass;
		}
	return EFail;
	}

TVerdict CTestStepDrm::DoTestStepPostambleL()
	{
	delete iTimer;
	iTimer = NULL;
	
	iFile.Close();
	iFs.Close();
	return CTestMmfMidiClntStep::DoTestStepPostambleL();
	}

TVerdict CTestStepDrm::DoTestStepL()
	{
	if (iDrmParams.iTestMode != TDrmTestParams::EAlloc)
		{
		return CTestMmfMidiClntStep::DoTestStepL();
		}
	TVerdict result=EPass;
	TInt failCount = 1;
	TBool completed = EFalse;
	CMidiClientUtility* midiClnt = NULL;
	TInt err;
	while(!completed)	
		{
		iError = KErrNone;
		__UHEAP_SETFAIL(RHeap::EFailNext ,failCount);
		__MM_HEAP_MARK;

		TRAP( err, 
				midiClnt = CMidiClientUtility::NewL(*this);
				result=DoTestL(midiClnt);
			);
		iTimer->Cancel();
		
 		completed = EFalse;
		if (err == KErrNone)
			{
			TAny *testAlloc = User::Alloc(1); // when this fails, we passed through all allocs within test
			if (testAlloc==NULL)
				{
				completed = ETrue;
				}
			else
				{
				completed = ETrue;
				User::Free(testAlloc);
				}
			}
		else if (err != KErrNoMemory) // bad error code
			{
			completed 	= ETrue;
			result		= EFail;
			}
		else
			{
			++failCount;
			}
			
		delete midiClnt;
		midiClnt = NULL;
					
		__UHEAP_RESET;
		__MM_HEAP_MARKEND;
		}
	return result;
	}

/*static*/
TInt CTestStepDrm::TimerGate(TAny* aPtr)
	{
	reinterpret_cast<CTestStepDrm*>(aPtr)->OnTimer();
	return KErrNone;
	}
	
void CTestStepDrm::OnTimer()
	{
	iTimer->Cancel();
	CActiveScheduler::Stop();
	iError = KErrTimedOut;	
	}

TVerdict CTestStepDrm::OpenFileL(CMidiClientUtility* aMidi)
	{
	TPtrC fileName;
	if (iDrmParams.iFileNameKey==NULL)
		{
		if(!GetStringFromConfig(KDrmSectName,KDrmFileKey,fileName))
			{
			return EInconclusive;
			}		
		}
	else
		{
		if(!GetStringFromConfig(KDrmSectName,TPtrC(iDrmParams.iFileNameKey), fileName))
			{
			return EInconclusive;
			}				
		}
	if (iDrmParams.iSourceType==TDrmTestParams::EFileName)
		{
		TPtrC uniqueId(ContentAccess::KDefaultContentObject);
		if (iDrmParams.iUniqueId)
			{
			uniqueId.Set(TPtrC(iDrmParams.iUniqueId));
			}
		if (iDrmParams.iUniqueId==NULL && iDrmParams.iIntent==TDrmTestParams::KNoIntent)
			{
			aMidi->OpenFile(TMMFileSource(fileName));
			}
		else if (iDrmParams.iIntent==TDrmTestParams::KNoIntent)
			{
			aMidi->OpenFile(TMMFileSource(fileName, uniqueId));
			}
		else
			{
			aMidi->OpenFile(TMMFileSource(fileName, uniqueId, iDrmParams.iIntent));
			}		
		}
	else
		{
		iFile.Close();
		User::LeaveIfError( iFs.ShareProtected() );
		User::LeaveIfError( iFile.Open(iFs, fileName, EFileRead | EFileShareExclusive) );
		
		TPtrC uniqueId(ContentAccess::KDefaultContentObject);
		if (iDrmParams.iUniqueId)
			{
			uniqueId.Set(TPtrC(iDrmParams.iUniqueId));
			}
		if (iDrmParams.iUniqueId==NULL && iDrmParams.iIntent==TDrmTestParams::KNoIntent)
			{
			aMidi->OpenFile(TMMFileHandleSource(iFile));
			}
		else if (iDrmParams.iIntent==TDrmTestParams::KNoIntent)
			{
			aMidi->OpenFile(TMMFileHandleSource(iFile, uniqueId));
			}
		else
			{
			aMidi->OpenFile(TMMFileHandleSource(iFile, uniqueId, iDrmParams.iIntent));
			}				
		}

	return EPass;	
	}


/*static*/
CTestDrmNegative* CTestDrmNegative::NewL(const TDesC& aTestName, const TTestStepType aTestType, const TDrmTestParams& aDrmParams)
	{
	return new (ELeave) CTestDrmNegative(aTestName, aTestType, aDrmParams);
	}

CTestDrmNegative::CTestDrmNegative(const TDesC& aTestName, const TTestStepType aTestType, const TDrmTestParams& aDrmParams):
						CTestStepDrm(aTestName, aTestType, aDrmParams)
	{
	}

void CTestDrmNegative::MmcuoStateChanged(TMidiState /*aOldState*/,TMidiState /*aNewState*/, const TTimeIntervalMicroSeconds& /*aTime*/,TInt aError)
	{
	iError = aError;
	CActiveScheduler::Stop();
	}

TVerdict CTestDrmNegative::DoTestL(CMidiClientUtility* aMidi)
	{
	iTimer->After( TTimeIntervalMicroSeconds32(K5Seconds) );
	TVerdict result=OpenFileL(aMidi);
	if (result!=EPass)
		{
		iTimer->Cancel();
		return result; 
		}
	CActiveScheduler::Start();

	if (iError!=iDrmParams.iExpectedError)
		{
		result=EFail;
		ERR_PRINTF3(_L("Open() gave error %d (expected %d)"),iError, iDrmParams.iExpectedError);
		}
	iTimer->Cancel();
	return result;
	}


/*static*/
CTestDrmPlay* CTestDrmPlay::NewL(const TDesC& aTestName, const TTestStepType aTestType, const TDrmTestParams& aDrmParams)
	{
	return new (ELeave) CTestDrmPlay(aTestName, aTestType, aDrmParams);
	}

CTestDrmPlay::CTestDrmPlay(const TDesC& aTestName, const TTestStepType aTestType, const TDrmTestParams& aDrmParams):
						CTestStepDrm(aTestName, aTestType, aDrmParams)
	{
	}

void CTestDrmPlay::MmcuoStateChanged(TMidiState /*aOldState*/,TMidiState aNewState, const TTimeIntervalMicroSeconds& /*aTime*/,TInt aError)
	{
	iError 		= aError;
	iLatestState= aNewState;
	CActiveScheduler::Stop();
	}

TVerdict CTestDrmPlay::PlayL(CMidiClientUtility* aMidi, TTimeIntervalMicroSeconds32 aStopAfter)
	{
	iTimer->Cancel();
	iTimer->After(aStopAfter);
	
	TVerdict result=OpenFileL(aMidi);
	if (result!=EPass)
		{
		iTimer->Cancel();
		return result; 
		}
	CActiveScheduler::Start();
	User::LeaveIfError(iError);
	
	aMidi->Play();
	CActiveScheduler::Start();
	User::LeaveIfError(iError);
	if (iLatestState != EMidiStateOpenPlaying)
		{
		iTimer->Cancel();
		ERR_PRINTF3(_L("Play() got state %d (expected %d)"),iLatestState, EMidiStateOpenPlaying);	
		return EFail;
		}
	
	CActiveScheduler::Start();
	
	iTimer->Cancel();
	if (iError==KErrNoMemory && iDrmParams.iTestMode==TDrmTestParams::EAlloc)
		{
		User::Leave(iError);
		}
		
	return result;
	}

TVerdict CTestDrmPlay::DoTestL(CMidiClientUtility* aMidi)
	{
	TVerdict res=EPass;
	TInt i=iDrmParams.iNumPlayLoops > 0 ? iDrmParams.iNumPlayLoops : 1;
	for (; i && res==EPass && iError==KErrNone; --i)
		{
		TRAP( iError,
				res=PlayL(aMidi, TTimeIntervalMicroSeconds32(K30Seconds));
			);
		}
	if (iError==KErrNoMemory && iDrmParams.iTestMode==TDrmTestParams::EAlloc)
		{
		User::Leave(iError);
		}		
	if (res==EPass)
		{
		res=(i==0 && iDrmParams.iExpectedError==iError)? EPass : EFail;
		}
		
	if (iError!=iDrmParams.iExpectedError)
		{
		ERR_PRINTF4(_L("Play() gave error %d (expected %d) loops left=%d"),iError, iDrmParams.iExpectedError, i);
		}		
	return res;		
	}

/*
***** Play() -> Stop()
*/
CTestDrmPlayStop* CTestDrmPlayStop::NewL(const TDesC& aTestName, const TTestStepType aTestType, const TDrmTestParams& aDrmParams)
	{
	return new (ELeave) CTestDrmPlayStop(aTestName, aTestType, aDrmParams);
	}


CTestDrmPlayStop::CTestDrmPlayStop(const TDesC& aTestName, const TTestStepType aTestType, const TDrmTestParams& aDrmParams):
					CTestDrmPlay(aTestName, aTestType, aDrmParams)
	{
	}
	
void CTestDrmPlayStop::OnTimer()
	{
	if (iLatestState != EMidiStateOpenPlaying || iStopping)
		{
		iError = KErrGeneral;
		CActiveScheduler::Stop();
		ERR_PRINTF3(_L("CTestDrmPlayStop::OnTimer() got err %d stopping %d"),iError, iStopping);	
		}
	else
		{
		iMidi->Stop(TTimeIntervalMicroSeconds(100000));
		iStopping=ETrue;
		iTimer->After(K5Seconds);
		}
	}
	
TVerdict CTestDrmPlayStop::DoTestL(CMidiClientUtility* aMidi)
	{
	iMidi = aMidi;
	TVerdict res=PlayL(aMidi, TTimeIntervalMicroSeconds32(K5Seconds));
	iTimer->Cancel();
	if (res!=EPass)
		{
		return res;
		}
	return iError==iDrmParams.iExpectedError ? EPass : EFail;
	}

/*
***** Play() -> Stop()
*/

/*static*/
CTestDrmPlayWithUI* CTestDrmPlayWithUI::NewL(const TDesC& aTestName, const TTestStepType aTestType, const TDrmTestParams& aDrmParams)
	{
	return new (ELeave) CTestDrmPlayWithUI(aTestName, aTestType, aDrmParams);
	}

CTestDrmPlayWithUI::CTestDrmPlayWithUI(const TDesC& aTestName, const TTestStepType aTestType, const TDrmTestParams& aDrmParams):
						CTestStepDrm(aTestName, aTestType, aDrmParams)
	{
	}

void CTestDrmPlayWithUI::MmcuoStateChanged(TMidiState /*aOldState*/,TMidiState aNewState, const TTimeIntervalMicroSeconds& /*aTime*/,TInt aError)
	{
	iError 		= aError;
	iLatestState= aNewState;
	CActiveScheduler::Stop();
	}

TVerdict CTestDrmPlayWithUI::PlayL(CMidiClientUtility* aMidi, TTimeIntervalMicroSeconds32 aStopAfter)
	{
	iTimer->Cancel();
	iTimer->After(aStopAfter);
	
	TVerdict result=OpenFileL(aMidi);
	if (result!=EPass)
		{
		iTimer->Cancel();
		return result; 
		}
	CActiveScheduler::Start();
	User::LeaveIfError(iError);
	
	MMMFDRMCustomCommand* cmd=aMidi->GetDRMCustomCommand();
	if (cmd==NULL)
		{
		ERR_PRINTF1(_L("GetDRMCustomCommand() returned NULL"));
		return EFail;		
		}
	iError=cmd->SetAgentProperty(ContentAccess::EAgentPropertyAgentUI, ETrue);
	// The test agent should return KErrCANotSupported...
	if (iError != KErrCANotSupported)
		{
		ERR_PRINTF2(_L("SetAgentProperty() Error=%d"),iError);
		return EFail;		
		}
	else
		{
		// ..it has, so flag that there's no error!
		iError = KErrNone;
		}
	User::LeaveIfError(iError);
	
	aMidi->Play();
	CActiveScheduler::Start();
	User::LeaveIfError(iError);
	
	if (iLatestState != EMidiStateOpenPlaying)
		{
		iTimer->Cancel();
		ERR_PRINTF3(_L("Play() got state %d (expected %d)"),iLatestState, EMidiStateOpenPlaying);	
		return EFail;
		}	
	
	CActiveScheduler::Start();
	
	iTimer->Cancel();
	if (iError==KErrNoMemory && iDrmParams.iTestMode==TDrmTestParams::EAlloc)
		{
		User::Leave(iError);
		}
		
	return EPass;
	}

TVerdict CTestDrmPlayWithUI::DoTestL(CMidiClientUtility* aMidi)
	{
	TVerdict res=EPass;
	TInt i=iDrmParams.iNumPlayLoops > 0 ? iDrmParams.iNumPlayLoops : 1;
	for (; i && res==EPass; --i)
		{
		res=PlayL(aMidi, TTimeIntervalMicroSeconds32(K30Seconds));
		}
	if (res==EFail)
		{
		return res;
		}
	if (iError!=iDrmParams.iExpectedError)
		{
		res=EFail;
		ERR_PRINTF3(_L("Play() gave error %d (expected %d)"),iError, iDrmParams.iExpectedError);
		}				
	return res;
	}

