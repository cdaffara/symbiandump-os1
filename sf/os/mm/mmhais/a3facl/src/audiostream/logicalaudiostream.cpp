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



#include "logicalaudiogaincontrol.h"
#include "logicalaudiocodec.h"
#include "logicalbuffersource.h"
#include "logicalbuffersink.h"
#include "logicalaudiodevicesink.h"
#include "logicalaudiodevicesource.h"
#include "logicalaudiostream.h"

#include <mmf/server/sounddevice.h>
#include <a3f/audioprocessingunittypeuids.h>
#include <a3f/maudiocodec.h>
#include <a3f/maudiocontext.h>
#include "audiocontext.h"

// TODO: Remove when the MMRC Extension mechanism is ready
#include "mstreampositioncontrol.h"
#include "mstreampositioncontrol.h"
#include "audioprocessingunit.h"

#include <ecom/implementationproxy.h> // For making it ECom plugin


// Exported proxy for instantiation method resolution
// Define the interface UIDs
const TImplementationProxy ImplementationTable[] =
	{
	IMPLEMENTATION_PROXY_ENTRY(KAudioStreamUid, CLogicalAudioStream::NewL)
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
CLogicalAudioStream::CLogicalAudioStream() 
	: CAudioStreamManager(), 
	iCurrentState(EUninitialized),
	iMessageType(ERegisterStreamObserver)
	{
	TRACE_CREATE();
	DP_CONTEXT(CLogicalAudioStream::CLogicalAudioStream *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	DP_OUT();
	}

// ---------------------------------------------------------------------------
// CLogicalAudioStream::NewL
// ---------------------------------------------------------------------------
CLogicalAudioStream* CLogicalAudioStream::NewL(TUid /*aTypeId*/)
	{
	DP_STATIC_CONTEXT(CLogicalAudioStream::NewL *CD0*, CtxDevSound, DPLOCAL);
	DP_IN();
	CLogicalAudioStream* self = new(ELeave)CLogicalAudioStream();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);  
	DP0_RET(self, "0x%x");
	}

// ---------------------------------------------------------------------------
// Second phase constructor
// ---------------------------------------------------------------------------
void CLogicalAudioStream::ConstructL()
	{
	DP_CONTEXT(CLogicalAudioStream::ConstructL *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	DP_OUT();
	}

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
 CLogicalAudioStream::~CLogicalAudioStream()
	{
	DP_CONTEXT(CLogicalAudioStream::~CLogicalAudioStream *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	iAudioProcessingUnits.Close();
	iCISupportObservers.Close();
	DP_OUT();
	}


// From MAudioStream	
// ---------------------------------------------------------------------------
// CLogicalAudioStream::Uninitialize
// ---------------------------------------------------------------------------
TInt CLogicalAudioStream::Uninitialize()
	{
	DP_CONTEXT(CLogicalAudioStream::Uninitialize *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	if (iCurrentState != EInitialized)
		{
		DP0_RET(KErrNotReady, "%d");
		}
	
	iCurrentState = EUninitialized;	
	DP0_RET(KErrNone, "%d");
	}

// ---------------------------------------------------------------------------
// CLogicalAudioStream::Initialize
// ---------------------------------------------------------------------------
TInt CLogicalAudioStream::Initialize()
	{
	DP_CONTEXT(CLogicalAudioStream::Initialize *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	if (iCurrentState != EUninitialized)
		{
		DP0_RET(KErrNotReady, "%d");
		}
			
	//calling commit?? 
	iCurrentState = EInitialized;	
	DP0_RET(KErrNone, "%d");
	}

// ---------------------------------------------------------------------------
// CLogicalAudioStream::Load
// ---------------------------------------------------------------------------
TInt CLogicalAudioStream::Load()
	{
	DP_CONTEXT(CLogicalAudioStream::Load *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	if (iCurrentState != EInitialized)
		{
		DP0_RET(KErrNotReady, "%d");
		}
			
	iCurrentState = EIdle;
	ResetStreamTime();
	DP0_RET(KErrNone, "%d");
	}
	
// ---------------------------------------------------------------------------
// CLogicalAudioStream::Stop
// ---------------------------------------------------------------------------
TInt CLogicalAudioStream::Stop()
	{
	DP_CONTEXT(CLogicalAudioStream::Stop *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	if (iCurrentState != EActive && iCurrentState != EPrimed) 
		{
		DP0_RET(KErrNotReady, "%d");
		}
	iCurrentState = EIdle;
	DP0_RET(KErrNone, "%d");
	}

// ---------------------------------------------------------------------------
// CLogicalAudioStream::Unload
// ---------------------------------------------------------------------------
TInt CLogicalAudioStream::Unload()
	{
	DP_CONTEXT(CLogicalAudioStream::Unload *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	if (iCurrentState != EIdle)
		{
		DP0_RET(KErrNotReady, "%d");
		}
			
	iCurrentState = EInitialized;
	DP0_RET(KErrNone, "%d");
	}

// ---------------------------------------------------------------------------
// CLogicalAudioStream::Prime
// ---------------------------------------------------------------------------
TInt CLogicalAudioStream::Prime()
	{
	DP_CONTEXT(CLogicalAudioStream::Prime *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	if (iCurrentState != EActive && iCurrentState != EIdle)
		{
		DP0_RET(KErrNotReady, "%d");
		}
			
	iCurrentState = EPrimed;
	DP0_RET(KErrNone, "%d");
	}

// ---------------------------------------------------------------------------
// CLogicalAudioStream::Flush
// ---------------------------------------------------------------------------
TInt CLogicalAudioStream::Flush()
	{
	DP_CONTEXT(CLogicalAudioStream::Flush *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	if (iCurrentState != EIdle && iCurrentState != EPrimed)
		{
		DP0_RET(KErrNotReady, "%d");
		}
	
	if(!iStreamBufferControl)
		{
		DP0_RET(KErrNotReady, "%d");
		}
	//Empty any buffers that have been filled
	iStreamBufferControl->FlushBuffers();

	DP0_RET(KErrNone, "%d");
	}

// ---------------------------------------------------------------------------
// CLogicalAudioStream::Activate
// ---------------------------------------------------------------------------
TInt CLogicalAudioStream::Activate()
	{
	DP_CONTEXT(CLogicalAudioStream::Activate *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	if (iCurrentState != EIdle && iCurrentState != EPrimed) 
		{
		DP0_RET(KErrNotReady, "%d");
		}
			
	//The start-up procedures include requesting permission for audio processing 
	//from audio policy. If permission to start is denied by audio policy, a state 
	//change callback to the current state will occur.
	iCurrentState = EActive;
	DP0_RET(KErrNone, "%d");
	}

// ---------------------------------------------------------------------------
// CLogicalAudioStream::AddSource
// ---------------------------------------------------------------------------
TInt CLogicalAudioStream::AddSource(MAudioProcessingUnit* aSource)
	{
	DP_CONTEXT(CLogicalAudioStream::AddSource *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	TInt err;
	if (aSource->IsTypeOf(KUidMmfBufferSource) || aSource->IsTypeOf(KUidAudioDeviceSource))
		{
		if (iCurrentState == EUninitialized)
			{
			err = DoAddProcessingUnit(aSource);
			}
		else
			{
			err = KErrNotReady;
			}
		}
	else
		{
		err = KErrNotSupported;
		}
	DP0_RET(err, "%d");
	}


// ---------------------------------------------------------------------------
// CLogicalAudioStream::AddSink
// ---------------------------------------------------------------------------
TInt CLogicalAudioStream::AddSink(MAudioProcessingUnit* aSink)
	{
	DP_CONTEXT(CLogicalAudioStream::AddSink *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	TInt err;
	if (aSink->IsTypeOf(KUidMmfBufferSink) || aSink->IsTypeOf(KUidAudioDeviceSink) )
		{
		if (iCurrentState == EUninitialized)
			{
			err = DoAddProcessingUnit(aSink);
			}
		else
			{
			err = KErrNotReady;
			}
		}
	else
		{
		err = KErrNotSupported;
		}
	DP0_RET(err, "%d");
	}

// ---------------------------------------------------------------------------
// CLogicalAudioStream::AddCodec
// ---------------------------------------------------------------------------
TInt CLogicalAudioStream::AddAudioCodec(MAudioProcessingUnit* aCodec)
	{
	DP_CONTEXT(CLogicalAudioStream::AddAudioCodec *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	TInt err;
	if (iCodec != NULL)
		{
		DP0_RET(KErrInUse, "%d");
		}
	
	if (aCodec->IsTypeOf(KUidAudioCodec))
		{
		if (iCurrentState == EUninitialized)
			{
			err = DoAddProcessingUnit(aCodec);
			}
		else
			{
			err = KErrNotReady;
			}
		}
	else
		{
		err = KErrNotSupported;
		}
	DP0_RET(err, "%d");
	}

// ---------------------------------------------------------------------------
// CLogicalAudioStream::AddGainControl
// ---------------------------------------------------------------------------
TInt CLogicalAudioStream::AddGainControl(MAudioProcessingUnit* aGainControl)
	{
	DP_CONTEXT(CLogicalAudioStream::AddGainControl *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	TInt err(KErrNone);
	
	if (iGain != NULL)
		{
		DP0_RET(KErrInUse, "%d");
		}
	
	if (aGainControl->IsTypeOf(KUidAudioGainControl))
		{
		if (iCurrentState == EUninitialized)
			{
			err = DoAddProcessingUnit(aGainControl);
			}
		else
			{
			err = KErrNotReady;
			}
		}
	else
		{
		err = KErrNotSupported;
		}
	DP0_RET(err, "%d");
	}

// ---------------------------------------------------------------------------
// CLogicalAudioStream::RemoveProcessigUnit
// ---------------------------------------------------------------------------
TInt CLogicalAudioStream::RemoveProcessingUnit(MAudioProcessingUnit* aProcessingUnit)
	{
	DP_CONTEXT(CLogicalAudioStream::RemoveProcessingUnit *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();

	if(iCurrentState != EUninitialized)
		{
		DP0_RET(KErrNotReady, "%d");
		}

	TInt err(KErrNotFound);
	if (aProcessingUnit != NULL)
		{
		TAudioComponentId param = aProcessingUnit->InstanceId();
		TUint count= iAudioProcessingUnits.Count();

		if (aProcessingUnit->IsTypeOf(KUidAudioCodec))
			{
			iCodec = NULL;
			}
		else if (aProcessingUnit->IsTypeOf(KUidAudioGainControl))
			{
			iGain = NULL;
			}
		
		for ( TUint i(0); i < count; i++ )
			{
			// find and remove component
			if( iAudioProcessingUnits[i]->InstanceId() == param)
				{
				iAudioProcessingUnits.Remove(i);
				break;
				}
			}
		SetMessageType(EComponentDestruction);
		err = KErrNone;
		}
	DP0_RET(err, "%d");
	}

// ---------------------------------------------------------------------------
// CLogicalAudioStream::ResetStreamTime
// ---------------------------------------------------------------------------
TInt CLogicalAudioStream::ResetStreamTime()
	{
	DP_CONTEXT(CLogicalAudioStream::ResetStreamTime *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	TInt err(KErrNone);
	if (iCurrentState != EIdle)
		{
		DP0_RET(KErrNotReady, "%d");
		}
	if(iPositionControl)
		{
		iPositionControl->ResetControlPosition();
		}
	iTimeProcessed = 0;
	DP0_RET(err, "%d");
	}


// ---------------------------------------------------------------------------
// CLogicalAudioStream::GetStreamTime
// ---------------------------------------------------------------------------
TInt CLogicalAudioStream::GetStreamTime(TTimeIntervalMicroSeconds& aStreamTime)
	{
	DP_CONTEXT(CLogicalAudioStream::GetStreamTime *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	TInt err(KErrNone);

	if(iPositionControl)
		{
		err = iPositionControl->GetControlPosition(aStreamTime);
		if(err == KErrNone)
			{
			iTimeProcessed = aStreamTime;
			}
		}
	else
		{
		aStreamTime = iTimeProcessed;
		}
	DP0_RET(err, "%d");
	}

// ---------------------------------------------------------------------------
// CLogicalAudioStream::Interface
// ---------------------------------------------------------------------------
TAny* CLogicalAudioStream::Interface(TUid aType)
	{
	DP_CONTEXT(CLogicalAudioStream::Interface *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	TAny* interface(NULL);
	if( aType == KUidAudioStream)
		{
		interface = static_cast<MAudioStream*>(this);
		}
	else if( aType == KUidExtensionInferface)
		{
		interface = static_cast<MCustomInterfaceSupport*>(this);
		}
	else if (aType == KUidAudioStreamAdaptationObserver)
		{
		interface = static_cast<MAudioStreamAdaptationObserver*>(this);
		DP0_RET(interface, "0x%x");
		}
	else if (aType == KUidAudioCodecObserver)
		{
		interface = static_cast<MAudioCodecObserver*>(this);
		DP0_RET(interface, "0x%x");
		}

	DP_OUT();
	return interface;
	}

// ---------------------------------------------------------------------------
// From MCustomInterfaceSupport
// CAudioStream::RequestCustomInterface
// ---------------------------------------------------------------------------
TInt CLogicalAudioStream::RequestCustomInterface(TUid aUid, TAny*& aPtr)
	{
	DP_CONTEXT(CLogicalAudioStream::RequestCustomInterface *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	TInt err = KErrNone;
	if (aUid == KA3FBackdoorAccessIfUid)
		{
		MA3FBackdoorAccessIf* self = this;
		aPtr = self;
		}
	else if(iInterfaceProvider)
		{
		err = iInterfaceProvider->RequestCustomInterface(aUid, aPtr);
		if (err != KErrNone)
			{
			aPtr = NULL;
			}
		}
	else
		{
		err = KErrNotReady;
		}
	DP0_RET(err, "%d");
	}

// ---------------------------------------------------------------------------
// From MCustomInterfaceSupport
// CAudioStream::RegisterObserver
// ---------------------------------------------------------------------------
TInt CLogicalAudioStream::RegisterObserver(MCustomInterfaceSupportObserver& aObserver)
	{
	DP_CONTEXT(CLogicalAudioStream::RegisterObserver *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	TInt err = KErrNone;
	err = iCISupportObservers.Find(&aObserver);
	if( err != KErrNotFound )
		{
		err = KErrAlreadyExists;
		}
	else
		{
		err = iCISupportObservers.Append(&aObserver);
		}
	DP0_RET(err, "%d");
	}

// ---------------------------------------------------------------------------
// From MCustomInterfaceSupport
// CAudioStream::UnRegisterObserver
// ---------------------------------------------------------------------------
void CLogicalAudioStream::UnRegisterObserver(MCustomInterfaceSupportObserver& aObserver)
	{
	DP_CONTEXT(CLogicalAudioStream::UnRegisterObserver *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	TInt idxOrErr = iCISupportObservers.Find(&aObserver);
	if( idxOrErr != KErrNotFound )
		{
		iCISupportObservers.Remove(idxOrErr);
		}
	DP_OUT();
	}

// ---------------------------------------------------------------------------
// From MCustomInterfaceSupport
// CAudioStream::CustomInterfaceRemoval
// ---------------------------------------------------------------------------
void CLogicalAudioStream::CustomInterfaceRemoval(TUid aInterfaceUid, TAny* aPtr)
	{
	DP_CONTEXT(CLogicalAudioStream::CustomInterfaceRemoval *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	TUint count = iCISupportObservers.Count();
	for ( TUint idx(0); idx < count; idx++ )
		{
		iCISupportObservers[idx]->CustomInterfaceRemoval(aInterfaceUid, aPtr);
		}
	DP_OUT();
	}

// ---------------------------------------------------------------------------
// From MAudioStreamAdaptationObserver
// CAudioStream::PhysicalAdaptationEvent
// ---------------------------------------------------------------------------
void CLogicalAudioStream::PhysicalAdaptationEvent(TPhysicalEvent /*aEvent*/, TInt /*aError*/)
	{
	DP_CONTEXT(CLogicalAudioStream::PhysicalAdaptationEvent *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	DP_OUT();
	}

// ---------------------------------------------------------------------------
// CAudioStream::StateEvent
// ---------------------------------------------------------------------------
void CLogicalAudioStream::StateEvent(TInt aReason, TAudioState aNewState)
	{
	DP_CONTEXT(CLogicalAudioStream::StateEvent *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
#ifdef _DEBUG	
	RDebug::Print(_L("CLogicalAudioStream::StateEvent Error %d Stay %d"),  aReason, aNewState);
#endif
	TUint count = iAudioStreamObserver.Count();
	for ( TUint idx(0); idx < count; idx++ )
		{
		iAudioStreamObserver[idx]->StateEvent(*this, aReason, aNewState);
		}
	iCurrentState = aNewState;
	DP_OUT();
	}

// ---------------------------------------------------------------------------
// CLogicalAudioStream::AddProcessingUnitComplete
// ---------------------------------------------------------------------------
void CLogicalAudioStream::AddProcessingUnitComplete(TUid aType, TInt aError)
	{
	DP_CONTEXT(CLogicalAudioStream::AddProcessingUnitComplete *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	MAudioProcessingUnit* instance = NULL;
	MapUidToProcessingUnit(aType, instance);
	TUint count = iAudioStreamObserver.Count();
	for ( TUint idx(0); idx < count; idx++ )
		{
		iAudioStreamObserver[idx]->AddProcessingUnitComplete(*this, instance, aError);
		}
	DP_OUT();
	}

// ---------------------------------------------------------------------------
// CLogicalAudioStream::RemoveProcessingUnitComplete
// ---------------------------------------------------------------------------
void CLogicalAudioStream::RemoveProcessingUnitComplete(TUid aType, TInt aError)
	{
	DP_CONTEXT(CLogicalAudioStream::RemoveProcessingUnitComplete *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	MAudioProcessingUnit* instance = NULL;
	MapUidToProcessingUnit(aType, instance);
	TUint count = iAudioStreamObserver.Count();
	for ( TUint idx(0); idx < count; idx++ )
		{
		iAudioStreamObserver[idx]->RemoveProcessingUnitComplete(*this, instance, aError);
		}
	DP_OUT();
	} 

// ---------------------------------------------------------------------------
// From MAudioStreamAdaptationObserver
// CLogicalAudioStream::ProcessingFinished
// ---------------------------------------------------------------------------
void CLogicalAudioStream::ProcessingFinished()
	{
	DP_CONTEXT(CLogicalAudioStream::ProcessingFinished *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	TUint count = iAudioStreamObserver.Count();
	for ( TUint idx(0); idx < count; idx++ )
		{
		iAudioStreamObserver[idx]->ProcessingFinished(*this);
		}
	DP_OUT();
	}

// ---------------------------------------------------------------------------
// CLogicalAudioStream::FlushComplete
// ---------------------------------------------------------------------------
void CLogicalAudioStream::FlushComplete(TInt aError)
	{
	DP_CONTEXT(CLogicalAudioStream::FlushComplete *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	TUint count = iAudioStreamObserver.Count();
	for ( TUint idx(0); idx < count; idx++ )
		{
		iAudioStreamObserver[idx]->FlushComplete(*this, aError);
		}
	DP_OUT();
	}


// ---------------------------------------------------------------------------
// Internal
// CLogicalAudioStream::DoAddProcessingUnit
// ---------------------------------------------------------------------------
TInt CLogicalAudioStream::DoAddProcessingUnit(MAudioProcessingUnit* aProcessingUnit)
	{
	DP_CONTEXT(CLogicalAudioStream::DoAddProcessingUnit *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	TInt err(KErrNone);

	if (aProcessingUnit->IsTypeOf(KUidAudioCodec) )
		{
		// Need for tone handling error
		CAudioProcessingUnit* pUnit = static_cast<CAudioProcessingUnit*>(aProcessingUnit);
		iCodec = static_cast<MAudioProcessingUnit*>(pUnit);
		}
	else if (aProcessingUnit->IsTypeOf(KUidAudioGainControl) )
		{
		CAudioProcessingUnit* pUnit = static_cast<CAudioProcessingUnit*>(aProcessingUnit);
		iGain = static_cast<MAudioProcessingUnit*>(pUnit);
		}

	err = iAudioProcessingUnits.Append(aProcessingUnit);
	SetMessageType(EComponentCreation);

	DP0_RET(err, "%d");
	}

// ---------------------------------------------------------------------------
// Internal
// CLogicalAudioStream::MapUidToProcessingUnit
// ---------------------------------------------------------------------------
void CLogicalAudioStream::MapUidToProcessingUnit(TUid aType, MAudioProcessingUnit*& aInstance)
	{
	DP_CONTEXT(CLogicalAudioStream::MapUidToProcessingUnit *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	TUint count = iAudioProcessingUnits.Count();
	for(TUint i=0; i<count; i++)
		{
		aInstance = static_cast<MAudioProcessingUnit*>(iAudioProcessingUnits[i]);
		if ( aInstance->IsTypeOf(aType)  )
			{
			break;
			}
		aInstance = NULL;
		}
	DP_OUT();
	}

// From MLogicalChain
// ---------------------------------------------------------------------------
// CLogicalAudioStream::SetMessageType
// ---------------------------------------------------------------------------
void CLogicalAudioStream::SetMessageType(TMMRCMessageType aMessageType)
	{
	DP_CONTEXT(CLogicalAudioStream::SetMessageType *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	iMessageType |= aMessageType;
	DP_OUT();
	}

// ---------------------------------------------------------------------------
// CLogicalAudioStream::ResetMessage
// ---------------------------------------------------------------------------
void CLogicalAudioStream::ResetMessage()
	{
	DP_CONTEXT(CLogicalAudioStream::SetMessageType *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	iMessageType = ENoMessage;
	DP_OUT();
	}

// ---------------------------------------------------------------------------
// CLogicalAudioStream::MessageType
// ---------------------------------------------------------------------------
TUint CLogicalAudioStream::MessageType() 
	{
	DP_CONTEXT(CLogicalAudioStream::MessageType *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	DP0_RET(iMessageType, "Message type %d");
	}


// ---------------------------------------------------------------------------
// CLogicalAudioStream::AudioProcessingUnitUid
// ---------------------------------------------------------------------------
TUid CLogicalAudioStream::AudioProcessingUnitUid(TInt aIndex)
	{
	DP_CONTEXT(CLogicalAudioStream::AudioProcessingUnitUid *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	TUid uid = {0};
	MAudioProcessingUnit* pUnit(NULL);
	if (aIndex >= 0)
		{
		pUnit = static_cast<MAudioProcessingUnit*>(iAudioProcessingUnits[aIndex]);
		}

	if (pUnit != NULL)
		{
		if (pUnit->IsTypeOf(KUidAudioDecoder))
			{
			uid = KUidAudioDecoder;
			}
		else if (pUnit->IsTypeOf(KUidAudioEncoder))
			{
			uid = KUidAudioEncoder;
			}			
		else if (pUnit->IsTypeOf(KUidMmfBufferSource))
			{
			uid = KUidMmfBufferSource;
			}
		else if (pUnit->IsTypeOf(KUidAudioGainControl))
			{
			uid = KUidAudioGainControl;
			}
		else if (pUnit->IsTypeOf(KUidAudioDeviceSink))
			{
			uid = KUidAudioDeviceSink;
			}
		else if (pUnit->IsTypeOf(KUidMmfBufferSink))
			{
			uid = KUidMmfBufferSink;
			}
		else if (pUnit->IsTypeOf(KUidAudioDeviceSource))
			{
			uid = KUidAudioDeviceSource;
			}			
		else if (pUnit->IsTypeOf(KUidAudioCodec))
			{
			uid = KUidAudioCodec;
			}
		}
	DP_OUT();
	return uid;
	}

// ---------------------------------------------------------------------------
// CLogicalAudioStream::AudioProcessingUnitsCount
// ---------------------------------------------------------------------------
TInt CLogicalAudioStream::AudioProcessingUnitsCount()
	{
	DP_CONTEXT(CLogicalAudioStream::AudioProcessingUnitsCount *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	DP0_RET(iAudioProcessingUnits.Count(), "%d");
	}

// ---------------------------------------------------------------------------
// CLogicalAudioStream::StreamState
// ---------------------------------------------------------------------------
TAudioState CLogicalAudioStream::StreamState()
	{
	DP_CONTEXT(CLogicalAudioStream::StreamState *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	DP0_RET(iCurrentState, "Stream state %d");
	}

// ---------------------------------------------------------------------------
// From MLogicalChain
// CLogicalAudioStream::SetStreamState
// ---------------------------------------------------------------------------
void CLogicalAudioStream::SetStreamState(TAudioState aAudioState)
	{
	DP_CONTEXT(CLogicalAudioStream::StreamState *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	iCurrentState = aAudioState;
	DP_OUT();
	}

// ---------------------------------------------------------------------------
// From MLogicalChain
// CLogicalAudioStream::CodecFormat
// ---------------------------------------------------------------------------
TUid CLogicalAudioStream::CodecFormat()
	{
	DP_CONTEXT(CLogicalAudioStream::CodecFormat *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	CLogicalAudioCodec* codec = static_cast<CLogicalAudioCodec*>(iCodec);
	DP_OUT();
	TUid tuidnull = {0};
	if(codec)
		{
		return codec->iFormat;
		}
	else
		{
		return tuidnull;
		}
	}

// ---------------------------------------------------------------------------
// From MLogicalChain
// CLogicalAudioStream::GetSampleRate
// ---------------------------------------------------------------------------
TInt CLogicalAudioStream::GetSampleRate()
	{
	DP_CONTEXT(CLogicalAudioStream::GetSampleRate *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	const TUint KDefaultSampleRate = 8000; // Default sample rate
	CLogicalAudioCodec* codec = static_cast<CLogicalAudioCodec*>(iCodec);
	DP_OUT();
	TInt sampleRateValue = KDefaultSampleRate;
	if(codec)
		{
		sampleRateValue = codec->iSampleRateConfig;
		}
	return sampleRateValue;
	}

// ---------------------------------------------------------------------------
// From MLogicalChain
// CLogicalAudioStream::GetMode
// ---------------------------------------------------------------------------
TUid CLogicalAudioStream::GetMode()
	{
	DP_CONTEXT(CLogicalAudioStream::GetMode *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	CLogicalAudioCodec* codec = static_cast<CLogicalAudioCodec*>(iCodec);
	TUid tuidnull = {0};
	DP_OUT();
	if(codec)
		{
		return codec->iModeConfig;
		}
	else
		{
		return tuidnull;
		}
	}


// ---------------------------------------------------------------------------
// From MLogicalChain
// CLogicalAudioStream::Priority
// ---------------------------------------------------------------------------
TInt CLogicalAudioStream::Priority()
	{
	DP_CONTEXT(CLogicalAudioStream::Priority*CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	DP0_RET(iAudioTypeSettings.iPriority, "Priority %d");
	}

// ---------------------------------------------------------------------------
// From MLogicalChain
// CLogicalAudioStream::GetVolumeRampParameters
// ---------------------------------------------------------------------------
void CLogicalAudioStream::GetVolumeRampParameters(TUid& aRampOperation, TTimeIntervalMicroSeconds& aRampDuration)
	{
	DP_CONTEXT(CLogicalAudioStream::GetVolumeRampParameters*CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	CLogicalAudioGainControl* gain = static_cast<CLogicalAudioGainControl*>(iGain);
	if(gain)
		{
		aRampOperation = gain->iDesiredRampOperation;
		aRampDuration = gain->iDesiredRampTime;
		}
	DP_OUT();
	}

// ---------------------------------------------------------------------------
// From MLogicalChain
// CLogicalAudioStream::CopySettings
// @param the logical chain from where the parameter will be copied
// ---------------------------------------------------------------------------
void CLogicalAudioStream::CopySettings(const MLogicalChain& aChain)
	{
	DP_CONTEXT(CLogicalAudioStream::CopySettings *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	CopyStreamSettings(aChain);
	CopyCodecSettings(aChain);
	CopyGainSettings(aChain);
	DP_OUT();
	}

void CLogicalAudioStream::CopyStreamSettings(const MLogicalChain& aChain)
	{
	DP_CONTEXT(CLogicalAudioStream::CopyStreamSettings *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	// LogicalAudioStream settings
	CLogicalAudioStream& srcChain = static_cast<CLogicalAudioStream&>(const_cast<MLogicalChain&>(aChain));
	
	iAudioTypeSettings = srcChain.iAudioTypeSettings;
	iCurrentState = srcChain.iCurrentState;
	iRequestState = srcChain.iRequestState; 
	iMessageType  = srcChain.iMessageType;
	iTimeProcessed = srcChain.iTimeProcessed; 
	DP_OUT();
	}

void CLogicalAudioStream::CopyCodecSettings(const MLogicalChain& aChain)
	{
	DP_CONTEXT(CLogicalAudioStream::CopyCodecSettings *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();

	CLogicalAudioStream& srcChain = static_cast<CLogicalAudioStream&>(const_cast<MLogicalChain&>(aChain));
	// Copy codec settings
	CLogicalAudioCodec* srcCodec = static_cast<CLogicalAudioCodec*>(srcChain.iCodec);
	if(iCodec && srcCodec)
		{
		CLogicalAudioCodec* logicalCodec = static_cast<CLogicalAudioCodec*>(iCodec);
		logicalCodec->iFormat = srcCodec->iFormat;
		logicalCodec->iSampleRateConfig = srcCodec->iSampleRateConfig;
		logicalCodec->iModeConfig = srcCodec->iModeConfig;
		}
	DP_OUT();
	}


void CLogicalAudioStream::CopyGainSettings(const MLogicalChain& aChain)
	{
	DP_CONTEXT(CLogicalAudioStream::CopyGainSettings *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();

	CLogicalAudioStream& srcChain = static_cast<CLogicalAudioStream&>(const_cast<MLogicalChain&>(aChain));
	// Copy gain settings
	CLogicalAudioGainControl* srcGain = static_cast<CLogicalAudioGainControl*>(srcChain.iGain);
	if (iGain && srcGain )
		{
		if(srcGain->iDesiredChannels.Count() > 0)
			{
			CLogicalAudioGainControl* logicalGain = static_cast<CLogicalAudioGainControl*>(iGain);
			TUint count = srcGain->iDesiredChannels.Count();
			for (TUint i(0); i < count; i++)
				{
				logicalGain->iDesiredChannels[i] = srcGain->iDesiredChannels[i];
				}
			logicalGain->iDesiredRampOperation = srcGain->iDesiredRampOperation;
			logicalGain->iDesiredRampTime = srcGain->iDesiredRampTime;
			}
		}
	DP_OUT();
	}

// ---------------------------------------------------------------------------
// From MLogicalChain
// CLogicalAudioStream::CloneL
// ---------------------------------------------------------------------------
MLogicalChain* CLogicalAudioStream::CloneL()
	{
	DP_CONTEXT(CLogicalAudioStream::CloneL *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	TInt err = KErrNone;
	MLogicalChain* newLogicalChain = NULL;

	// Create another audiostream
	CAudioStreamManager *manager = NULL;
	manager = CAudioStreamManager::NewL(KUidAudioStream);
	// Copy stream settings
	CLogicalAudioStream* logicalAudioStream = static_cast<CLogicalAudioStream*>(manager);

	MAudioProcessingUnit* pUnit=NULL;
	TInt count = iAudioProcessingUnits.Count();
	for(TInt i = 0; i < count ; i++)
		{
		// Create processing unit
		TUid type = AudioProcessingUnitUid(i);
		TComponentParameters cParameters;
		cParameters.iTypeUid = type;
		cParameters.iInstanceId = 0;
		cParameters.iContextId = 0;
		cParameters.iSettingsObserver = NULL;
		pUnit = CAudioProcessingUnit::NewL(cParameters);

		// If no error then add to the stream
		if ( (type == KUidAudioDecoder) || (type == KUidAudioEncoder) ) 
			{
			err = logicalAudioStream->AddAudioCodec(pUnit);
			}
		else if (type == KUidMmfBufferSource || (type == KUidAudioDeviceSource) )
			{
			err = logicalAudioStream->AddSource(pUnit);
			}			
		else if (type == KUidAudioGainControl)
			{
			err = logicalAudioStream->AddGainControl(pUnit);
			}
		else if (type == KUidAudioDeviceSink || ( type == KUidMmfBufferSink) )
			{
			err = logicalAudioStream->AddSink(pUnit);
			}
		// TODO: 
		// Check this couldn't be added
		if(err != KErrNone)
			{
			logicalAudioStream->iAudioProcessingUnits.Remove(iAudioProcessingUnits.Count()-1);
			delete pUnit;
			}
		}
	// Cast to MLogicalChain
	newLogicalChain = static_cast<MLogicalChain*>(logicalAudioStream); 
	newLogicalChain->CopySettings(*this);

	DP0_RET(newLogicalChain, "0x%x");
	}

// ---------------------------------------------------------------------------
// From MLogicalChain
// CLogicalAudioStream::Release
// ---------------------------------------------------------------------------
void CLogicalAudioStream::Release()
	{
	DP_CONTEXT(CLogicalAudioStream::Release *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	// ResetAndDestroy
	TInt count = iAudioProcessingUnits.Count();
	TInt i;
	for (i=0; i<count; i++)
		{
		CAudioProcessingUnit* pUnit = static_cast<CAudioProcessingUnit*>(iAudioProcessingUnits[i]);
		delete pUnit;
		}
	iAudioProcessingUnits.Reset();
	DP_OUT();
	delete this;
	}

// ---------------------------------------------------------------------------
// From MLogicalChain
// CLogicalAudioStream::GetComponent
// ---------------------------------------------------------------------------
TAny* CLogicalAudioStream::GetComponent(TUid aType)
	{
	DP_CONTEXT(CLogicalAudioStream::GetComponent *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	TAny* interface = NULL;
	
	// go through this or our subcomponents to see if somebody knows about the interface
	interface = Interface(aType);
	if (interface==NULL)
		{
		TUint count = iAudioProcessingUnits.Count();
		// go through components looking for valid interface
		for ( TInt i=0; i < count; i++ )
			{
			interface = iAudioProcessingUnits[i]->Interface(aType);
			if (interface!=NULL)
				{
				break;
				};
			}
		};


	DP0_RET(interface, "0x%x");
	}



// ---------------------------------------------------------------------------
// From MLogicalChain
// CLogicalAudioStream::SetAdaptationSource
// ---------------------------------------------------------------------------
void CLogicalAudioStream::SetAdaptationSource(MMMFBufferSource& aSource)
	{
	DP_CONTEXT(CLogicalAudioStream::SetAdaptationSource *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	MapUidToProcessingUnit(KUidMmfBufferSource, iAudioProcessingUnit);
	ASSERT(iAudioProcessingUnit);
		
	CAudioProcessingUnit* cUnit = static_cast<CAudioProcessingUnit*>(iAudioProcessingUnit);
	CLogicalBufferSource* bufferSource = (static_cast<CLogicalBufferSource*>(cUnit));	

	bufferSource->iAdaptationBufferSource = &aSource;	
	aSource.SetDataSupplier(*bufferSource);
	
	iAudioProcessingUnit = NULL;
	DP_OUT();
	} 

// ---------------------------------------------------------------------------
// From MLogicalChain
// CLogicalAudioStream::SetAdaptationSink
// ---------------------------------------------------------------------------
void CLogicalAudioStream::SetAdaptationSink(MMMFBufferSink& aSink)
	{
	DP_CONTEXT(CLogicalAudioStream::SetAdaptationSink *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	MapUidToProcessingUnit(KUidMmfBufferSink, iAudioProcessingUnit);
	ASSERT(iAudioProcessingUnit);
		
	CAudioProcessingUnit* cUnit = static_cast<CAudioProcessingUnit*>(iAudioProcessingUnit);
	CLogicalBufferSink* bufferSink = (static_cast<CLogicalBufferSink*>(cUnit));
		
	bufferSink->iAdaptationBufferSink = &aSink;	
	aSink.SetDataConsumer(*bufferSink);

	iAudioProcessingUnit = NULL;
	DP_OUT();
	}

// ---------------------------------------------------------------------------
// From MLogicalChain
// CLogicalAudioStream::SetAdaptationGainControl
// ---------------------------------------------------------------------------
void CLogicalAudioStream::SetAdaptationGainControl(MAudioGainControl& aGain)
	{
	DP_CONTEXT(CLogicalAudioStream::SetAdaptationGainControl *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	ASSERT(iGain);
	CLogicalAudioGainControl* gain = static_cast<CLogicalAudioGainControl*>(iGain);
	gain->iAdaptationGain = &aGain;
	aGain.RegisterAudioGainControlObserver(*gain);
	DP_OUT();
	}



// ---------------------------------------------------------------------------
// From MLogicalChain
// CLogicalAudioStream::SetPositionControl
// ---------------------------------------------------------------------------
void CLogicalAudioStream::SetStreamPositionControl(MStreamPositionControl& aPositionControl)
	{
	DP_CONTEXT(CLogicalAudioStream::SetStreamPositionControl *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	iPositionControl = &aPositionControl;
	DP_OUT();
	}

// ---------------------------------------------------------------------------
// CLogicalAudioStream::SetAdaptationStream
// ---------------------------------------------------------------------------
void CLogicalAudioStream::SetAdaptationStream(MConfigurationHelper& aStream)
	{
	DP_CONTEXT(CLogicalAudioStream::SetAdaptationStream *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	CLogicalAudioCodec* codec = static_cast<CLogicalAudioCodec*>(iCodec);
	if (codec != NULL)
		{
		if (codec->iAdaptationStream == NULL)
			{
			codec->iAdaptationStream = &aStream;
			}
		}
	DP_OUT();
	}

// ---------------------------------------------------------------------------
// CLogicalAudioStream::SetStreamBufferControl
// ---------------------------------------------------------------------------
void CLogicalAudioStream::SetStreamBufferControl(MStreamBufferControl& aStreamBufferControl)
	{
	DP_CONTEXT(CLogicalAudioStream::SetAdaptationStream *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	iStreamBufferControl = &aStreamBufferControl;
	DP_OUT();
	}


// ---------------------------------------------------------------------------
// CLogicalAudioStream::SetCustomInterfaceProvider
// ---------------------------------------------------------------------------
void CLogicalAudioStream::SetCustomInterfaceProvider(MCustomInterfaceSupport& aInterfaceProvider)
	{
	DP_CONTEXT(CLogicalAudioStream::SetCustomInterfaceProvider *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	iInterfaceProvider = &aInterfaceProvider;
	DP_OUT();
	}

// ---------------------------------------------------------------------------
// CLogicalAudioStream::SampleRateSet
// ---------------------------------------------------------------------------
void CLogicalAudioStream::SampleRateSet(TInt aError)
	{
	DP_CONTEXT(CLogicalAudioStream::SampleRateSet *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	CLogicalAudioCodec* codec = static_cast<CLogicalAudioCodec*>(iCodec);
	if(codec)
		{
		TUint count = codec->iAudioCodecObserver.Count();
		for ( TUint idx(0); idx < count; idx++ )
			{
			codec->iAudioCodecObserver[idx]->SampleRateSet(aError);
			}
		}
	DP_OUT();
	}

// ---------------------------------------------------------------------------
// CLogicalAudioStream::ModeSet
// ---------------------------------------------------------------------------
void CLogicalAudioStream::ModeSet(TInt aError)
	{
	DP_CONTEXT(CLogicalAudioStream::SampleRateSet *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	CLogicalAudioCodec* codec = static_cast<CLogicalAudioCodec*>(iCodec);
	if(codec)
		{
		TUint count = codec->iAudioCodecObserver.Count();
		for ( TUint idx(0); idx < count; idx++ )
			{
			codec->iAudioCodecObserver[idx]->ModeSet(aError);
			}
		}
	DP_OUT();
	}

// ---------------------------------------------------------------------------
// CLogicalAudioStream::GetSupportedSampleRatesComplete
// ---------------------------------------------------------------------------
void CLogicalAudioStream::GetSupportedSampleRatesComplete (TInt aError)
	{
	DP_CONTEXT(CLogicalAudioStream::SampleRateSet *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	CLogicalAudioCodec* codec = static_cast<CLogicalAudioCodec*>(iCodec);
	if(codec)
		{
		TUint count = codec->iAudioCodecObserver.Count();
		for ( TUint idx(0); idx < count; idx++ )
			{
			codec->iAudioCodecObserver[idx]->GetSupportedSampleRatesComplete(aError);
			}
		}
	DP_OUT();
	}

// ---------------------------------------------------------------------------
// CLogicalAudioStream::GetSupportedModesComplete
// ---------------------------------------------------------------------------
void CLogicalAudioStream::GetSupportedModesComplete (TInt aError)
	{
	DP_CONTEXT(CLogicalAudioStream::SampleRateSet *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	CLogicalAudioCodec* codec = static_cast<CLogicalAudioCodec*>(iCodec);
	if(codec)
		{
		TUint count = codec->iAudioCodecObserver.Count();
		for ( TUint idx(0); idx < count; idx++ )
			{
			codec->iAudioCodecObserver[idx]->GetSupportedModesComplete(aError);
			}
		}
	DP_OUT();
	}

// ---------------------------------------------------------------------------
// CLogicalAudioStream::SetParentContext
// ---------------------------------------------------------------------------
void CLogicalAudioStream::SetParentContext(const CAudioContext& aContext)
	{
	iParentContext = const_cast<CAudioContext*>(&aContext);
	}


// from MA3FBackdoorAccessIf

MAudioContext* CLogicalAudioStream::AudioContext()
	{
	return iParentContext;
	}
	
MAudioStream* CLogicalAudioStream::AudioStream()
	{
	return this;
	}
	
MAudioProcessingUnit* CLogicalAudioStream::ProcessingUnit(TUid aType)
	{
	// look through our processing units for something of the correct type
	TInt numProcessingUnits = iAudioProcessingUnits.Count();
	for (TInt index=0; index < numProcessingUnits; index++)
		{
		MAudioProcessingUnit* ptr = iAudioProcessingUnits[index];
		if (ptr->IsTypeOf(aType))
			{
			return ptr;
			}
		}
	return NULL;
	}
