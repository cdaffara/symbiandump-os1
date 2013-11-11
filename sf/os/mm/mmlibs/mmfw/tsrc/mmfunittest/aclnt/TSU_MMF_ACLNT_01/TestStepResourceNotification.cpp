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

#include "TestStepUnitMMFAudClient.h"

#include "TestStepResourceNotification.h"


class CDelayTimer:public CTimer
	{
public:
	
	static CDelayTimer* NewL(const TCallBack& aTimerCb);
	
protected:	
	CDelayTimer(const TCallBack& aTimerCb);
	void RunL();
private:
	TCallBack	iCallback;
	};
	
	
/*static*/
CDelayTimer* CDelayTimer::NewL(const TCallBack& aTimerCb)
	{
	CDelayTimer* self=new (ELeave) CDelayTimer(aTimerCb);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CDelayTimer::CDelayTimer(const TCallBack& aTimerCb):CTimer(EPriorityStandard), iCallback(aTimerCb)
		{
		CActiveScheduler::Add(this);
		}
	
void CDelayTimer::RunL()
	{
	iCallback.CallBack();
	}

/*static*/
CTestStepResNotificationData* CTestStepResNotificationData::NewL()
	{
	return new (ELeave) CTestStepResNotificationData();
	}
	
CTestStepResNotificationData::~CTestStepResNotificationData()
	{
	}

void CTestStepResNotificationData::Close()
	{
	delete iDelayTimer;
	delete iRecorderUtilityLowP;
	delete iRecorderUtilityHighP;
	}
	
CTestStepResNotificationData::CTestStepResNotificationData()
	{
	iTestStepResult = EPass;
	iTestStepName = _L("MM-MMF-ACLNT-U-0953-HP");
	}
	
TVerdict CTestStepResNotificationData::DoTestStepPreambleL()
	{
	TVerdict verdict=EFail;
	 // this installs the scheduler
	if (EPass != (verdict = CTestStepUnitMMFAudClient::DoTestStepPreambleL()))
		{
		return verdict;
		}
		
	_LIT(KFileName, "AudioPlayFName10");
	if(!GetStringFromConfig(_L("SectionOne"), KFileName, iFileName) )
		{
		INFO_PRINTF2(_L("file name %s not found..."), KFileName);
		return EInconclusive;
		}
				
	iRecorderUtilityLowP = CMdaAudioRecorderUtility::NewL(*this);
	iRecorderUtilityHighP= CMdaAudioRecorderUtility::NewL(*this);
	iDelayTimer			 = CDelayTimer::NewL(TCallBack(TimerCbGate, this));
	return EPass;
	}
	
TVerdict CTestStepResNotificationData::DoTestStepPostambleL()
	{
	return CTestStepUnitMMFAudClient::DoTestStepPostambleL();
	}
	
TVerdict CTestStepResNotificationData::DoTestStepL()
	{
	iTestStepResult = EFail;
	INFO_PRINTF1(iTestStepName);
	INFO_PRINTF1(_L("Open 2 recorder utilities: low and high priority"));
	INFO_PRINTF1(_L("Low priority starts, then after 2 seconds high one does"));
	INFO_PRINTF1(_L("Low one should be preemted, after it finishes low one should be notified about resource availability"));
	
	User::LeaveIfError(iRecorderUtilityLowP->RegisterAudioResourceNotification(*this, KMMFEventCategoryAudioResourceAvailable));
	iRecorderUtilityLowP->OpenFileL(iFileName);
	iRecorderUtilityLowP->SetPriority( EMdaPriorityMax, EMdaPriorityPreferenceNone );
	iRecorderUtilityHighP->OpenFileL(iFileName);
	iRecorderUtilityLowP->SetPriority( EMdaPriorityMin, EMdaPriorityPreferenceNone );
	CActiveScheduler::Start();
	
	iRecorderUtilityLowP->PlayL();
	iDelayTimer->After(TTimeIntervalMicroSeconds32(2*1000*1000)); // 2 seconds
	CActiveScheduler::Start();
	
	if (!iGotNotification)
		{
		INFO_PRINTF1(_L("Error: There was no notification"));
		return EFail;
		}
	return (iTestStepResult=(iError==KErrNone? EPass : EFail));
	}

/*static*/
TInt CTestStepResNotificationData::TimerCbGate(TAny* aPtr)
	{
	reinterpret_cast<CTestStepResNotificationData*>(aPtr)->StartHighRecorder();
	return KErrNone;
	}
	
void CTestStepResNotificationData::StartHighRecorder()
	{
	iRecorderUtilityHighP->PlayL();
	}

// from MMdaObjectStateChangeObserver
void CTestStepResNotificationData::MoscoStateChangeEvent(CBase* aObject, TInt aPreviousState, TInt aCurrentState, TInt aErrorcCode)
	{
	if (aErrorcCode != KErrNone)
		{
		iError = aErrorcCode;
		}
	if (iError!=KErrNone)
		{
		// we shall recieve error saying that low priority recorder
		if (aObject==iRecorderUtilityLowP && 
				(aCurrentState==CMdaAudioRecorderUtility::EPlaying || aPreviousState==CMdaAudioRecorderUtility::EPlaying)
			)
			{
			INFO_PRINTF1(_L("Low priority recorder has been preemted...OK"));
			iError = KErrNone;
			}
		else
			{
			INFO_PRINTF4(_L("Error in MoscoStateChangeEvent prev %d new %d Error %d"),aPreviousState, aCurrentState, aErrorcCode);
			iTestStepResult = EFail;
			CActiveScheduler::Stop();
			}
		return;
		}
		
	 if (aPreviousState==0 && aCurrentState==CMdaAudioRecorderUtility::EOpen && ++iOpenCompletedCnt==2)
		{
		CActiveScheduler::Stop();
		return;
		}
		
	if (aPreviousState==CMdaAudioRecorderUtility::EPlaying && aCurrentState==CMdaAudioRecorderUtility::EOpen)
		{
		if (aObject==iRecorderUtilityHighP)
			{
			iRecorderUtilityHighP->Stop();
			}
		else
			{
			iError=KErrGeneral;
			INFO_PRINTF1(_L("Error: Low priority decoder stopped = unexpected"));
			CActiveScheduler::Stop();
			}
		return;
		}
	}

void CTestStepResNotificationData::MarncResourceAvailable(TUid /*aNotificationEventId*/, const TDesC8& aNotificationData)
	{
	INFO_PRINTF2(_L("Got notification, data length=%d"),aNotificationData.Length());
	if (aNotificationData.Length() == 0)
		{
		INFO_PRINTF2(_L("Got notification, data too short %d"),aNotificationData.Length());
		iError = KErrGeneral;
		}
	else
		{
		if (aNotificationData.Length()==sizeof(TTimeIntervalMicroSeconds))
			{
			TTimeIntervalMicroSeconds pos(0);
			TPckg<TTimeIntervalMicroSeconds> posPckg(pos);
			posPckg.Copy(aNotificationData);
			if (pos < TTimeIntervalMicroSeconds(0) || pos > TTimeIntervalMicroSeconds(6*1000*1000))
				{
				INFO_PRINTF2(_L("Got wrong resume position %ld"),pos.Int64());
				iError = KErrGeneral;
				}
			}
		else
			{
			INFO_PRINTF2(_L("Got strange data size %d"),aNotificationData.Length());
			}
		iGotNotification = (iError == KErrNone);
		}
	CActiveScheduler::Stop();
	}
	
