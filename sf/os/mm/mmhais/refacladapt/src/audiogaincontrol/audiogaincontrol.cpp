// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#include "audiogaincontrol.h"
#include <a3f/audioprocessingunittypeuids.h>

// ======== LOCAL FUNCTIONS ========
void Panic(TInt aPanicCode)
	{
	_LIT(KAudioGainControlPanicCategory, "audiogaincontrol");
	User::Panic(KAudioGainControlPanicCategory, aPanicCode);
	}

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
CAudioGainControl::CAudioGainControl()
	{
	TRACE_CREATE();
	DP_CONTEXT(CAudioGainControl::CAudioGainControl *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	DP_OUT();
	}

// ---------------------------------------------------------------------------
// Factory method
// ---------------------------------------------------------------------------
EXPORT_C CAudioGainControl* CAudioGainControl::NewL()
	{
	DP_STATIC_CONTEXT(CAudioGainControl::NewL *CD0*, CtxDevSound, DPLOCAL);
	DP_IN();
	CAudioGainControl* self = new(ELeave)CAudioGainControl();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	DP0_RET(self, "0x%x");
	}

// ---------------------------------------------------------------------------
// Second phase constructor
// ---------------------------------------------------------------------------
void CAudioGainControl::ConstructL()
	{
	DP_CONTEXT(CAudioGainControl::ConstructL *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	DP_OUT();
	}

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
CAudioGainControl::~CAudioGainControl()
	{
	DP_CONTEXT(CAudioGainControl::~CAudioGainControl *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	iGainObservers.Close();
	DP_OUT();
	}

EXPORT_C void CAudioGainControl::SetHelper(MGainHelper &aHelper)
	{
	iHelper = &aHelper;
	}
// ---------------------------------------------------------------------------
// CAudioGainControl::IssueGainChangedCallBack
// ---------------------------------------------------------------------------
EXPORT_C void CAudioGainControl::IssueGainChangedCallBack(TInt aError)
	{
	DP_CONTEXT(CAudioGainControl::IssueGainChangedCallBack *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	TUint count = iGainObservers.Count();
	for(TUint i(0); i<count; i++)
		{
		iGainObservers[i]->GainChanged(*this, aError);
		}
	DP_OUT();
	}

// ---------------------------------------------------------------------------
// CAudioGainControl::IssueMaxGainChangedCallBack
// ---------------------------------------------------------------------------
void CAudioGainControl::IssueMaxGainChangedCallBack()
	{
	DP_CONTEXT(CAudioGainControl::IssueMaxGainChangedCallBack *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	TUint count = iGainObservers.Count();
	for(TUint i(0); i<count; i++)
		{
		iGainObservers[i]->MaxGainChanged(*this);
		}
	DP_OUT();
	}

// ---------------------------------------------------------------------------
// CAudioGainControl::IssueMaxRampTimeGainChangedCallBack
// ---------------------------------------------------------------------------
void CAudioGainControl::IssueMaxRampTimeChangedCallBack()
	{
	DP_CONTEXT(CAudioGainControl::IssueMaxRampTimeChangedCallBack *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	TUint count = iGainObservers.Count();
	for(TUint i(0); i<count; i++)
		{
		 iGainObservers[i]->MaxRampTimeChanged(*this);
		}
	DP_OUT();
	}

// ---------------------------------------------------------------------------
// CAudioGainControl::ConfigureRamp
// ---------------------------------------------------------------------------
EXPORT_C TInt CAudioGainControl::ConfigureRamp(TUid aRampOperation, const TTimeIntervalMicroSeconds& aRampDuration)
	{
	DP_CONTEXT(CAudioGainControl::ConfigureRamp *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();

	TInt err = KErrNone;
	// Apply ramp
	if(iHelper)
		{
		err = iHelper->ConfigureRamp(aRampOperation, aRampDuration);
		}
		
	DP0_RET(err, "%d");
	}

// ---------------------------------------------------------------------------
// from class MAudioGainControl
// CAudioGainControl::GetMaxGain
// ---------------------------------------------------------------------------
TInt CAudioGainControl::GetMaxGain(TInt& aMaxGain) const
	{
	DP_CONTEXT(CAudioGainControl::GetMaxGain *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	TInt err = KErrNone;
	aMaxGain = KDefaultMaxGain;
	DP0_RET(err, "%d");
	}

// ---------------------------------------------------------------------------
// from class MAudioGainControl
// CAudioGainControl::GetMaxRampTime
// ---------------------------------------------------------------------------
TInt CAudioGainControl::GetMaxRampTime(TTimeIntervalMicroSeconds& aMaxRampTime) const
	{
	DP_CONTEXT(CAudioGainControl::GetMaxRampTime *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	aMaxRampTime = KDefaultMaxRampTime;
	DP0_RET(KErrNone, "%d");
	} 

// ---------------------------------------------------------------------------
// from class MAudioGainControl
// CAudioGainControl::GetGain
// ---------------------------------------------------------------------------
TInt CAudioGainControl::GetGain(RArray<TAudioChannelGain>& aChannels) const
	{
	DP_CONTEXT(CAudioGainControl::GetGain *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	TInt err = KErrNone;

	aChannels.Reset();
	TInt count = iGains.Count();

	for (TUint i(0); i < count; i++)
		{
		err = aChannels.Append(iGains[i]);
		if(err != KErrNone)
			{
			break;
			}
		}
	
	DP0_RET(err, "%d");
	}

// ---------------------------------------------------------------------------
// from class MAudioGainControl
// CAudioGainControl::SetGain
// ---------------------------------------------------------------------------
TInt CAudioGainControl::SetGain( RArray<TAudioChannelGain>& /*aChannels*/, TUid /*aRampOperation*/, 
	const TTimeIntervalMicroSeconds& /*aRampDuration*/)
	{
	DP_CONTEXT(CAudioGainControl::SetGain *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	__ASSERT_DEBUG(EFalse, Panic(EAdaptationOldSetGainCalledPanic));
	DP0_RET(KErrNone, "%d");
	}

// ---------------------------------------------------------------------------
// from class MAudioGainControl
// CAudioGainControl::SetGain
// ---------------------------------------------------------------------------
TInt CAudioGainControl::SetGain(RArray<TAudioChannelGain>& aChannels)
	{
	DP_CONTEXT(CAudioGainControl::SetGain *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	TInt err = KErrNone;

	// Keep array cache
	iGains.Reset();
	TUint count = aChannels.Count();
	for (TUint i(0); i < count; i++)
		{
		err = iGains.Append(aChannels[i]);
		if(err != KErrNone)
			{
			break;
			}
		}

	// Apply gain
	if(iHelper && err == KErrNone)
		{
		err = iHelper->SetGain(aChannels);
		}
	DP0_RET(KErrNone, "%d");
	}

// ---------------------------------------------------------------------------
// from class MAudioGainControl
// CAudioGainControl::RegisterAudioGainControlObserver
// ---------------------------------------------------------------------------
TInt CAudioGainControl::RegisterAudioGainControlObserver(MAudioGainControlObserver& aObserver)
	{
	DP_CONTEXT(CAudioGainControl::RegisterAudioStreamObserver *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	TInt err = iGainObservers.Find(&aObserver);
	if(err == KErrNotFound)
		{
		err = iGainObservers.Append(&aObserver);
		}
	else
		{
		err = KErrAlreadyExists;
		}
	DP0_RET(err, "%d");
	}

// ---------------------------------------------------------------------------
// from class MAudioGainControl
// CAudioGainControl::UnregisterAudioGainControlObserver
// ---------------------------------------------------------------------------
void CAudioGainControl::UnregisterAudioGainControlObserver(MAudioGainControlObserver& aObserver)
	{
	DP_CONTEXT(CAudioGainControl::UnregisterAudioStreamObserver *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	TInt idxOrErr = iGainObservers.Find(&aObserver);
	if( idxOrErr != KErrNotFound )
		{
		iGainObservers.Remove(idxOrErr);
		}
	DP_OUT();
	}

// end of file
