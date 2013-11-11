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
//



#include "logicalaudiocodec.h"
#include <a3f/audioprocessingunittypeuids.h>

#include <ecom/implementationproxy.h> // For making it ECom plugin
// Map the interface implementation UIDs to implementation factory functions
const TImplementationProxy ImplementationTable[] =
	{
	IMPLEMENTATION_PROXY_ENTRY(KAudioDecoderUid,  CLogicalAudioCodec::NewL),
	IMPLEMENTATION_PROXY_ENTRY(KAudioEncoderUid,  CLogicalAudioCodec::NewL),
	};

// Exported proxy for instantiation method resolution.
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
	return ImplementationTable;
	}

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
CLogicalAudioCodec* CLogicalAudioCodec::NewL(TAny* aParam)
	{
	DP_STATIC_CONTEXT(CLogicalAudioCodec::NewL *CD0*, CtxDevSound, DPLOCAL);
	DP_IN();
	CLogicalAudioCodec* self = new(ELeave)CLogicalAudioCodec(aParam);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	DP0_RET(self, "0x%x");
	}

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------

CLogicalAudioCodec::CLogicalAudioCodec(TAny* aParam) : CAudioProcessingUnit(aParam)
	{
	TRACE_CREATE();
	DP_CONTEXT(CLogicalAudioCodec::CLogicalAudioCodec *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	if(iSettingsObserver)
		{
		iSettingsObserver->ReceiveComponentSettingsChange(iType, ERegisterCodecObserver);
		}
	DP_OUT();
	}

// ---------------------------------------------------------------------------
// Second phase constructor
// ---------------------------------------------------------------------------
void CLogicalAudioCodec::ConstructL()
	{
	DP_CONTEXT(CLogicalAudioCodec::ConstructL *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	DP_OUT();
	}

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
CLogicalAudioCodec::~CLogicalAudioCodec()
	{
	DP_CONTEXT(CLogicalAudioCodec::~CLogicalAudioCodec *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	iAudioCodecObserver.Close();
	DP_OUT();
	}

// ---------------------------------------------------------------------------
// from class MAudioCodec
// CLogicalAudioCodec::SetFormat
// ---------------------------------------------------------------------------
TInt CLogicalAudioCodec::SetFormat(TUid aFormat)
	{
	DP_CONTEXT(CLogicalAudioCodec::SetFormat *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	iFormat = aFormat;
	if(iSettingsObserver)
		{
		iSettingsObserver->ReceiveComponentSettingsChange(iType, EComponentAlterationCodec);
		}
	DP0_RET(KErrNone, "%d");
	}

// ---------------------------------------------------------------------------
// from class MAudioCodec
// CLogicalAudioCodec::SetSampleRate
// ---------------------------------------------------------------------------
TInt CLogicalAudioCodec::SetSampleRate(TInt aSampleRate)
	{
	DP_CONTEXT(CLogicalAudioCodec::SetSampleRate *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	iSampleRateConfig = aSampleRate;
	if(iSettingsObserver)
		{
		iSettingsObserver->ReceiveComponentSettingsChange(iType, EComponentAlterationCodec);
		}
	DP0_RET(KErrNone, "%d");
	}

// ---------------------------------------------------------------------------
// from class MAudioCodec
// CLogicalAudioCodec::SetMode
// ---------------------------------------------------------------------------
TInt CLogicalAudioCodec::SetMode(TUid aMode)
	{
	DP_CONTEXT(CLogicalAudioCodec::SetMode *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	iModeConfig = aMode;
	if(iSettingsObserver)
		{
		iSettingsObserver->ReceiveComponentSettingsChange(iType, EComponentAlterationCodec);
		}
	DP0_RET(KErrNone, "%d");
	}

// ---------------------------------------------------------------------------
// from class MAudioCodec
// CLogicalAudioCodec::GetSupportedSamplesRates
// ---------------------------------------------------------------------------
TInt CLogicalAudioCodec::GetSupportedSamplesRates(RArray<TInt>& aSupportedRates)
	{
	DP_CONTEXT(CLogicalAudioCodec::GetSupportedSamplesRates *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	TInt err(KErrNone);
	if(iAdaptationStream)
		{
		err = iAdaptationStream->GetSupportedSampleRates(aSupportedRates);
		}
	else
		{
		err = KErrNotFound;
		}
	DP0_RET(err, "%d");
	}

// ---------------------------------------------------------------------------
// from class MAudioCodec
// CLogicalAudioCodec::GetSupportedMode
// ---------------------------------------------------------------------------
TInt CLogicalAudioCodec::GetSupportedModes(RArray<TUid>& aSupportedModes)
	{
	DP_CONTEXT(CLogicalAudioCodec::GetSupportedModes *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	TInt err(KErrNone);
	if(iAdaptationStream)
		{
		err = iAdaptationStream->GetSupportedModes(aSupportedModes);
		}
	else
		{
		err = KErrNotFound;
		}
	DP0_RET(err, "%d");
	}
// ---------------------------------------------------------------------------
// from class MAudioProcessingUnit
// CLogicalAudioCodec::IsTypeOf
// ---------------------------------------------------------------------------
TBool CLogicalAudioCodec::IsTypeOf(TUid aTypeId) const
	{
	DP_CONTEXT(CLogicalAudioCodec::IsTypeOf *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	TBool result = EFalse;
	if ( iType == aTypeId || aTypeId == KUidAudioCodec)
		{
		result = ETrue;
		}
	DP0_RET(result, "%d");
	}

// ---------------------------------------------------------------------------
// from class MAudioProcessingUnit
// CLogicalAudioCodec::Interface
// ---------------------------------------------------------------------------
//
TAny* CLogicalAudioCodec::Interface(TUid aType)
	{
	DP_CONTEXT(CLogicalAudioCodec::Interface *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	
	TAny* interface = NULL;
	if (aType == KUidAudioCodec)
		{
		interface = static_cast<MAudioCodec*>(this);
		}
	else if (aType == KUidAudioCodecAdaptationObserver)
		{
		interface = static_cast<MAudioCodecAdaptationObserver*>(this);
		}
	DP0_RET(interface, "0x%08x");
	}

// ---------------------------------------------------------------------------
// from class MAudioCodecAdaptationObserver
// CLogicalAudioCodec::AllBuffersProcessed
// ---------------------------------------------------------------------------
void CLogicalAudioCodec::AllBuffersProcessed()
	{
	DP_CONTEXT(CLogicalAudioCodec::AllBuffersProcessed *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	DP_OUT();
	}

// ---------------------------------------------------------------------------
// from class MAudioCodecAdaptationObserver
// CLogicalAudioCodec::GetSupportedConfigurationComplete
// ---------------------------------------------------------------------------
void CLogicalAudioCodec::GetSupportedAModesComplete(TInt aError)
	{
	DP_CONTEXT(CLogicalAudioCodec::GetSupportedModesComplete *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	TUint count = iAudioCodecObserver.Count();
	for ( TUint idx(0); idx < count; idx++ )
		{
		iAudioCodecObserver[idx]->GetSupportedModesComplete(aError);
		}
	DP_OUT();
	}
	
// ---------------------------------------------------------------------------
// from class MAudioCodecAdaptationObserver
// CLogicalAudioCodec::GetSupportedConfigurationComplete
// ---------------------------------------------------------------------------
void CLogicalAudioCodec::GetSupportedARatesComplete(TInt aError)
	{
	DP_CONTEXT(CLogicalAudioCodec::GetSupportedRatesComplete *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	TUint count = iAudioCodecObserver.Count();
	for ( TUint idx(0); idx < count; idx++ )
		{
		iAudioCodecObserver[idx]->GetSupportedSampleRatesComplete(aError);
		}
	DP_OUT();
	}

// ---------------------------------------------------------------------------
// from class MAudioCodecAdaptationObserver
// CLogicalAudioCodec::ProcessingUnitError
// ---------------------------------------------------------------------------
void CLogicalAudioCodec::ProcessingUnitError(TInt aError)
	{
	DP_CONTEXT(CLogicalAudioCodec::ProcessingUnitError *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();

	TUint count = iAudioProcessingUnitObservers.Count();
	MAudioProcessingUnit* pUnit = static_cast<MAudioProcessingUnit*>(this);
	for(TUint index=0; index < count; index++)
		{
		iAudioProcessingUnitObservers[index]->ProcessingUnitError(pUnit, aError);
		}
	DP_OUT();
	}

// ---------------------------------------------------------------------------
// CLogicalAudioCodec::RegisterAudioCodecObserver
// ---------------------------------------------------------------------------
TInt CLogicalAudioCodec::RegisterAudioCodecObserver(MAudioCodecObserver& aObserver)
	{
	DP_CONTEXT(CLogicalAudioCodec::RegisterAudioCodecObserver *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	TInt err = iAudioCodecObserver.Find(&aObserver);
	if(err == KErrNotFound)
		{
		err = iAudioCodecObserver.Append(&aObserver);
		}
	else
		{
		err = KErrAlreadyExists;
		}
	DP0_RET(err, "%d");
	}

// ---------------------------------------------------------------------------
// CLogicalAudioCodec::UnregisterAudioCodecObserver
// ---------------------------------------------------------------------------
void CLogicalAudioCodec::UnregisterAudioCodecObserver(MAudioCodecObserver& aObserver)
	{
	DP_CONTEXT(CLogicalAudioCodec::UnregisterAudioCodecObserver *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	TInt idxOrErr = iAudioCodecObserver.Find(&aObserver);
	if( idxOrErr != KErrNotFound )
		{
		iAudioCodecObserver.Remove(idxOrErr);
		}
	DP_OUT();
	}

// End of file
