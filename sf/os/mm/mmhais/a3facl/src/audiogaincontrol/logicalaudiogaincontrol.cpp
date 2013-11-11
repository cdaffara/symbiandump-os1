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
// Name        : audiogaincontrol.cpp
// Part of     : ACL Logical layer
//



#include "logicalaudiogaincontrol.h"
#include <a3f/audioprocessingunittypeuids.h>
#include <ecom/implementationproxy.h> // For making it ECom plugin

// ======== LOCAL FUNCTIONS ========
// __________________________________________________________________________
// Exported proxy for instantiation method resolution
// Define the interface UIDs

// Moved from header 
const TInt KDefaultMaxRampTime = 10;

const TImplementationProxy ImplementationTable[] = 
	{
	IMPLEMENTATION_PROXY_ENTRY(KAudioGainControlUid, CLogicalAudioGainControl::NewL)
	};

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
	return ImplementationTable;
	}

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CLogicalAudioGainControl::CLogicalAudioGainControl(TAny* aParam) : CAudioProcessingUnit(aParam)
	{
	TRACE_CREATE();
	DP_CONTEXT(CLogicalAudioGainControl::CLogicalAudioGainControl *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	
	iMaxRampTime = KDefaultMaxRampTime;
	
	DP_OUT();
	}

// ---------------------------------------------------------------------------
// Factory method
// ---------------------------------------------------------------------------
//
CLogicalAudioGainControl* CLogicalAudioGainControl::NewL(TAny* aParam)
	{
	DP_STATIC_CONTEXT(CLogicalAudioGainControl::NewL *CD0*, CtxDevSound, DPLOCAL);
	DP_IN();
	CLogicalAudioGainControl* self = new(ELeave)CLogicalAudioGainControl(aParam);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	DP0_RET(self, "0x%x");
	}

// ---------------------------------------------------------------------------
// Second phase constructor
// ---------------------------------------------------------------------------
//
void CLogicalAudioGainControl::ConstructL()
	{
	DP_CONTEXT(CLogicalAudioGainControl::ConstructL *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	TAudioChannelGain left;
	TAudioChannelGain right;
	left.iLocation = TAudioChannelGain::ELeft;
	right.iLocation = TAudioChannelGain::ERight;
	iDesiredChannels.AppendL(left);
	iDesiredChannels.AppendL(right);
	DP_OUT();
	}

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CLogicalAudioGainControl::~CLogicalAudioGainControl()
	{
	DP_CONTEXT(CLogicalAudioGainControl::~CLogicalAudioGainControl *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	iActiveChannels.Close();
	iDesiredChannels.Close();
	iGainObservers.Close();
	DP_OUT();
	}

// ---------------------------------------------------------------------------
// from class MAudioGainControl
// CLogicalAudioGainControl::GetMaxGain
// ---------------------------------------------------------------------------
//
TInt CLogicalAudioGainControl::GetMaxGain(TInt& aMaxGain) const
	{
	DP_CONTEXT(CLogicalAudioGainControl::GetMaxGain *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	// TODO: Fix this when the MMRC server is available
	// This adaptationGain pointer will be replaced by the a pointer gotten from 
	// the "control mechanism" provided by the real MMRC server
	TInt err = KErrNotReady;
	if(iAdaptationGain)
		{
		err = iAdaptationGain->GetMaxGain(aMaxGain);
		}
	DP0_RET(err, "%d");
	}

// ---------------------------------------------------------------------------
// from class MAudioGainControl
// CLogicalAudioGainControl::GetMaxRampTime
// ---------------------------------------------------------------------------
//
TInt CLogicalAudioGainControl::GetMaxRampTime(TTimeIntervalMicroSeconds& aMaxRampTime) const
	{
	DP_CONTEXT(CLogicalAudioGainControl::GetMaxRampTime *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	TInt err = KErrNone;
	aMaxRampTime = KDefaultMaxRampTime;
	if(iAdaptationGain)
		{
		err = iAdaptationGain->GetMaxRampTime(aMaxRampTime);
		}
	DP0_RET(err, "%d");
	} 

// ---------------------------------------------------------------------------
// from class MAudioGainControl
// CLogicalAudioGainControl::GetGain
// ---------------------------------------------------------------------------
//
TInt CLogicalAudioGainControl::GetGain(RArray<TAudioChannelGain>& aChannels) const
	{
	DP_CONTEXT(CLogicalAudioGainControl::GetGain *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	TInt err = KErrNone;
	TInt count = iDesiredChannels.Count();
	aChannels.Reset();
	for (TUint i(0); i < count; i++)
		{
		TInt err = aChannels.Append(iDesiredChannels[i]);
		}
	DP0_RET(err, "%d");
	}

// ---------------------------------------------------------------------------
// from class MAudioGainControl
// CLogicalAudioGainControl::SetGain
// ---------------------------------------------------------------------------
//
TInt CLogicalAudioGainControl::SetGain(RArray<TAudioChannelGain>& aChannels, TUid aRampOperation, const TTimeIntervalMicroSeconds& aRampDuration)
	{
	DP_CONTEXT(CLogicalAudioGainControl::SetGain *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	TInt err = KErrNone;

	iDesiredRampTime = static_cast<TInt>(aRampDuration.Int64());
	iDesiredRampOperation = aRampOperation;

	// The gain values are clippped to MaxGain. 
	TInt a3fMaxGain = 0;
	if(iAdaptationGain)
		{
		err = iAdaptationGain->GetMaxGain(a3fMaxGain);
		}

	if(err == KErrNone)
		{
		iDesiredChannels.Reset();
		TUint count = aChannels.Count();
		TAudioChannelGain gainChannel;
		for (TUint i(0); i < count; i++)
			{
			gainChannel = aChannels[i];
			// 
			if(gainChannel.iGain > a3fMaxGain)
				{
				gainChannel.iGain = a3fMaxGain;
				}
			else if(gainChannel.iGain < 0)
				{
				err = KErrArgument;
				break;
				}
			
			err = iDesiredChannels.Append( gainChannel );
			if(err != KErrNone)
				{
				break;
				}
			}
		}

	if(err == KErrNone)
		{
		if(iSettingsObserver)
			{
			iSettingsObserver->ReceiveComponentSettingsChange(iType, EComponentAlterationGain);
			}
		}

	DP0_RET(err, "%d");
	}

// ---------------------------------------------------------------------------
// from class MAudioGainControl
// CLogicalAudioGainControl::SetGain
// ---------------------------------------------------------------------------
//
TInt CLogicalAudioGainControl::SetGain(RArray<TAudioChannelGain>& aChannels)
	{
	DP_CONTEXT(CLogicalAudioGainControl::SetGain *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	TInt err = KErrNone;
	err = SetGain(aChannels, KNullUid, 0);
	DP0_RET(err, "%d");
	}

// ---------------------------------------------------------------------------
// from class MAudioGainControl
// CLogicalAudioGainControl::RegisterAudioGainControlObserver
// ---------------------------------------------------------------------------
//
TInt CLogicalAudioGainControl::RegisterAudioGainControlObserver(MAudioGainControlObserver& aObserver)
	{
	DP_CONTEXT(CLogicalAudioGainControl::RegisterAudioGainControlObserver *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	TInt err = iGainObservers.Find(&aObserver);
	if( err != KErrNotFound )
		{
		err = KErrAlreadyExists;
		}
	else
		{
		err = iGainObservers.Append(&aObserver);
		}
	DP0_RET(err, "%d");
	}

// ---------------------------------------------------------------------------
// from class MAudioGainControl
// CLogicalAudioGainControl::UnregisterAudioGainControlObserver
// ---------------------------------------------------------------------------
//
void CLogicalAudioGainControl::UnregisterAudioGainControlObserver(MAudioGainControlObserver& aObserver)
	{
	DP_CONTEXT(CLogicalAudioGainControl::UnregisterAudioGainControlObserver *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	TInt idxOrErr = iGainObservers.Find(&aObserver);
	if( idxOrErr != KErrNotFound )
		{
		iGainObservers.Remove(idxOrErr);
		}
	DP_OUT();
	}

// ---------------------------------------------------------------------------
// from class MAudioProcessingUnit
// CLogicalAudioGainControl::IsTypeOf
// ---------------------------------------------------------------------------
TBool CLogicalAudioGainControl::IsTypeOf(TUid aTypeId) const
	{
	DP_CONTEXT(CLogicalAudioGainControl::IsTypeOf *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	TBool result = EFalse;
	if (iType == aTypeId)
		{
		result = ETrue;
		}
	DP0_RET(result, "%d");
	}

// ---------------------------------------------------------------------------
// from class MAudioProcessingUnit
// CLogicalAudioGainControl::Interface
// ---------------------------------------------------------------------------
TAny* CLogicalAudioGainControl::Interface(TUid aType)
	{
	DP_CONTEXT(CLogicalAudioGainControl::GetInterface *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	MAudioGainControl* ptr = NULL;
	if(aType==KUidAudioGainControl)
		{
		ptr = this;
		}
	DP_OUT();
	return ptr;
	}

// ---------------------------------------------------------------------------
// from class MAudioGainControlObserver
// CLogicalAudioGainControl::MaxRampTimeChanged
// ---------------------------------------------------------------------------
void CLogicalAudioGainControl::MaxRampTimeChanged(MAudioGainControl& /*aGain*/)
	{
	DP_CONTEXT(CLogicalAudioGainControl::MaxRampTimeChanged *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	TUint count = iGainObservers.Count();
	for ( TUint idx(0); idx < count; idx++ )
		{
		iGainObservers[idx]->MaxRampTimeChanged(*this);
		}
	DP_OUT();
	}

// ---------------------------------------------------------------------------
// from class MAudioGainControlObserver
// CLogicalAudioGainControl::MaxGainChanged
// ---------------------------------------------------------------------------
void CLogicalAudioGainControl::MaxGainChanged(MAudioGainControl& /*aGain*/)
	{
	DP_CONTEXT(CLogicalAudioGainControl::MaxGainChanged *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	TUint count = iGainObservers.Count();
	for ( TUint idx(0); idx < count; idx++ )
		{
		iGainObservers[idx]->MaxGainChanged(*this);
		}
	DP_OUT();
	}

// ---------------------------------------------------------------------------
// from class MAudioGainControlObserver
// CLogicalAudioGainControl::GainChanged
// ---------------------------------------------------------------------------
void CLogicalAudioGainControl::GainChanged(MAudioGainControl& /*aGain*/, TInt aError)
	{
	DP_CONTEXT(CLogicalAudioGainControl::GainChanged *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	TUint count = iDesiredChannels.Count();
	iActiveChannels.Reset();
	for(TUint i=0; i<count; i++)
		{
		iActiveChannels.Append(iDesiredChannels[i]);
		}
	count = iGainObservers.Count();
	for ( TUint idx(0); idx < count; idx++ )
		{
		iGainObservers[idx]->GainChanged(*this, aError);
		}
	DP_OUT();
	}
