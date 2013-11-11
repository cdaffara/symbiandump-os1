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

#include <e32base.h>

#include <simulprocclient.h>

#include "TSI_MMF_SDEVSOUND_MPBASE.h"
#include "TSI_MMF_SDEVSOUND_SUITE.h"

#include <mmf/plugin/mmfhwdeviceimplementationuids.hrh>

CSDSMPTSBase* CSDSMPTSBase::NewL(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName)
	{
	CSDSMPTSBase* self = new (ELeave) CSDSMPTSBase(aTestName, aSectName, aKeyName);
	return self;
	}

CSDSMPTSBase::CSDSMPTSBase(const TDesC& aTestName, const TDesC& aSectName, const TDesC& aKeyName)
	{
	// store the name of this test case
	// this is the name that is used by the script file
	// Each test step initialises it's own name
	iTestStepName = aTestName;
	iSectName = aSectName;
	iKeyName = aKeyName;
	}

/**
 *
 * DoTestStepPreambleL
 * @result TVerdict
 *
 */
TVerdict CSDSMPTSBase::DoTestStepPreambleL()
	{
	//Install the Active Scheduler
	iAS = new (ELeave) CActiveScheduler;
	CActiveScheduler::Install (iAS);

	iAL= new(ELeave)CActiveListener;
	
	INFO_PRINTF1(_L("Creating DevSound Object"));
	TRAPD(err, iMMFDevSound = CMMFDevSound::NewL());
	if (err)
		{
		return EInconclusive;
		}
	else
		{
		return EPass;
		}
	}

TVerdict CSDSMPTSBase::DoTestStepL( void )
	{
	INFO_PRINTF1( _L("CSDSMPTSBase : DevSoundMP"));
	iVerdict = EFail;

	return iVerdict;
	}

/**
 *
 * TestInitialize
 * @param aMode
 * @result TVerdict
 *
 */

TVerdict CSDSMPTSBase::TestInitialize(TMMFState aMode)
	{
	iCallbackError = KErrNone;
	iExpectedValue = KErrNone;
	INFO_PRINTF1(_L("Initializing DevSound"));
	ResetCallbacks();
	iAL->InitialiseActiveListener();
	// Initialize
	TRAPD(err, iMMFDevSound->InitializeL(*this, aMode));
	if (err)
		{
		WARN_PRINTF2 (_L("DevSound InitializeL left with error = %d"), err);
		return EInconclusive;
		}
	else
		{
		CActiveScheduler::Start();
		if (iCallbackError != iExpectedValue)
			{
			ERR_PRINTF3 (_L("DevSound InitializeComplete returned %d, expected %d"), iCallbackError, iExpectedValue);
			return EFail;
			}
		if (iCallbackArray[EInitComplete] != 1)
			{
			ERR_PRINTF2 (_L("DevSound InitializeComplete was called %d times, expected 1"), iCallbackArray[EInitComplete]);
			return EFail;
			}
		}
	return EPass;
	}

/**
 *
 * ResetCallbacks
 *
 */
void CSDSMPTSBase::ResetCallbacks()
	{
	//Initialize callback array
	iCallbackArray.Reset();
	}

/**
 *
 * GetCallbackTotal
 * @result TInt
 *
 */
TInt CSDSMPTSBase::GetCallbackTotal()
	{
	TInt total = 0;
	for (TInt i = EInitComplete; i < EDeviceMsg; i++)
		{
		total += iCallbackArray[i];
		}
	return total;
	}

/**
 *
 * TestSetPriority
 * @param aPriority
 *
 */
void CSDSMPTSBase::TestSetPriority(TInt aPriority)
	{
	INFO_PRINTF2(_L("Setting Priority = %d"), aPriority);

	TMMFPrioritySettings	prioritySettings;

	prioritySettings.iPref = EMdaPriorityPreferenceNone;
	prioritySettings.iPriority = aPriority;

	iMMFDevSound->SetPrioritySettings( prioritySettings );
	}

/**
 *
 * TestSetVolume
 * @param aVol
 *
 */
void CSDSMPTSBase::TestSetVolume(TInt aVol)
	{
	INFO_PRINTF2(_L("Setting Volume = %d"), aVol);
	iMMFDevSound->SetVolume(aVol);
	}

void CSDSMPTSBase::DoProcess(TInt aError)
	{
	if (aError == KErrNone)
		{
		InternalState nextState = iState;
		switch (iState)
			{
		case EWaitingForServer:
			nextState = EInitPlayer;
			break;
		case EInitPlayer : 
			nextState = EPlaying;
			break;
		case EPlaying :
			iVerdict = EPass;
			CActiveScheduler::Stop();
			break;
			}
		iState = nextState;
		}
	else 
		{
		iVerdict = EFail;
		INFO_PRINTF2(_L("Unexpected failure in test, error code %d"), aError);
		CActiveScheduler::Stop();
		}
	}

/**
 *
 * TestSetGain
 * @param aGain
 *
 */
void CSDSMPTSBase::TestSetGain(TInt aGain)
	{
	INFO_PRINTF2(_L("Setting Gain = %d"), aGain);
	iMMFDevSound->SetGain(aGain);
	}

CSDSMPTSBase::~CSDSMPTSBase()
	{
	}

void CSDSMPTSBase::Close()
	{
	delete iServerMonitor;
	}

/*
 * class CSDSMPTSBase::CServerMonitor
 */

CSDSMPTSBase::CServerMonitor::CServerMonitor(CSDSMPTSBase* aParent)
	:CActive(EPriorityNormal), iParent(aParent)
	{
	}

	
CSDSMPTSBase::CServerMonitor* CSDSMPTSBase::CServerMonitor::NewL(CSDSMPTSBase* aParent) 
	{
	CServerMonitor* self = new (ELeave) CServerMonitor(aParent);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

void CSDSMPTSBase::CServerMonitor::ConstructL() 
	{
	CActiveScheduler::Add(this);
	}

void CSDSMPTSBase::CServerMonitor::DoCancel() 
	{
	Cancel();
	}

void CSDSMPTSBase::CServerMonitor::RunL() 
	{
	// start the client state machine
	iParent->DoProcess(iStatus.Int());
	}

TRequestStatus& CSDSMPTSBase::CServerMonitor::ActiveStatus() 
	{
	SetActive();
	return iStatus;
	}

