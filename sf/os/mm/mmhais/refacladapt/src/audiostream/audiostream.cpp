//audiostream.cpp

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




#include <a3f/a3fbase.h>
#include <a3f/audioprocessingunittypeuids.h>
#include <a3f/a3ffourcclookup.h>

#include "audiostream.h"

// PHYSICAL COMPONENTS
#include "audiocodec.h"
#include "audiostream.h"
#include "buffersource.h"
#include "buffersink.h"
#include "audiodevicesource.h"
#include "audiodevicesink.h"
#include "audiogaincontrol.h"
#include "maudiostreamadaptationobserver.h"

#include <a3f/maudiodatasupplier.h>
#include <a3f/maudiodataconsumer.h>

#include "minputport.h"
#include "moutputport.h"
#include "audiocontext.h"

#include <ecom/implementationproxy.h>



const TInt KSampleRate8000Hz = 8000;

// Map the interface implementation UIDs to implementation factory functions
const TImplementationProxy ImplementationTable[] =
	{
	IMPLEMENTATION_PROXY_ENTRY(0x10283461,  CAudioStream::NewL),
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
CAudioStream::CAudioStream()
	: iCurrentStreamState(EUninitialized), 
	iDesiredStreamState(EUninitialized)
	{
	TRACE_CREATE();
	DP_CONTEXT(CAudioStream::CAudioStream *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	// Some init config values
	iSampleRateConfig = KSampleRate8000Hz;
	iModeConfig = KA3FModeMono;
	
	DP_OUT();
	}

// ---------------------------------------------------------------------------
// Factory method
// ---------------------------------------------------------------------------
//
CAudioStream* CAudioStream::NewL()
	{
	DP_STATIC_CONTEXT(CAudioStream::NewL *CD0*, CtxDevSound, DPLOCAL);
	DP_IN();
	CAudioStream* self = new(ELeave)CAudioStream();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	DP0_RET(self, "0x%x");
	}

// ---------------------------------------------------------------------------
// Second phase constructor
// ---------------------------------------------------------------------------
//
void CAudioStream::ConstructL()
	{
	DP_CONTEXT(CAudioStream::ConstructL *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	DP_OUT();
	}

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CAudioStream::~CAudioStream()
	{
	DP_CONTEXT(CAudioStream::~CAudioStream *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();

	iAudioStreamObservers.Close();
	iAudioCodecObservers.Close();
	// Just for now
	DeletePhysicalComponents();
	REComSession::FinalClose();
	DP_OUT();
	}

// ---------------------------------------------------------------------------
// CAudioStream::RegisterAudioStreamObserver
// ---------------------------------------------------------------------------
//
TInt CAudioStream::RegisterAudioStreamObserver(MAudioStreamAdaptationObserver& aObserver)
	{
	DP_CONTEXT(CAudioStream::RegisterAudioStreamObserver *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	TInt err = iAudioStreamObservers.Find(&aObserver);
	if(err == KErrNotFound)
		{
		err = iAudioStreamObservers.Append(&aObserver);
		}
	else
		{
		err = KErrAlreadyExists;
		}
	DP0_RET(err, "%d");
	}

// ---------------------------------------------------------------------------
// CAudioStream::UnregisterAudioStreamObserver
// ---------------------------------------------------------------------------
//
void CAudioStream::UnregisterAudioStreamObserver(MAudioStreamAdaptationObserver& aObserver)
	{
	DP_CONTEXT(CAudioStream::UnregisterAudioStreamObserver *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	TInt idxOrErr = iAudioStreamObservers.Find(&aObserver);
	if( idxOrErr != KErrNotFound )
		{
		iAudioStreamObservers.Remove(idxOrErr);
		}
	DP_OUT();
	}


// ---------------------------------------------------------------------------
// CAudioStream::SetFourCC
// ---------------------------------------------------------------------------
//
void CAudioStream::SetFourCC(const CFourCCConvertor& aFourCCConvertor)
	{
	DP_CONTEXT(CAudioStream::SetFourCC *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	CFourCCConvertor& ref = const_cast<CFourCCConvertor&>(aFourCCConvertor);
	iFourCCConvertor = static_cast<CFourCCConvertor*>(static_cast<TAny*>(&ref));
	DP_OUT();
	}


// ---------------------------------------------------------------------------
// CAudioStream::UnregisterAudioStreamObserver
// ---------------------------------------------------------------------------
//
void CAudioStream::UnregisterAllAudioStreamObserver()
	{
	DP_CONTEXT(CAudioStream::UnregisterAudioStreamObserver *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	
	iAudioStreamObservers.Reset();
	
	DP_OUT();
	}


// ---------------------------------------------------------------------------
// MMRC extension mechanism
// ---------------------------------------------------------------------------
TAny* CAudioStream::GetComponent(TUid aType)
	{
	TAny* ptr = NULL;
	MLogicalChain* clientDesiredChain = NULL;
	CAudioContext* context = static_cast<CAudioContext*>(iAudioContext);
	if(context)
		{
		clientDesiredChain = context->GetLogicalChain(0);	
		}
	if (clientDesiredChain)
		{
		ptr = clientDesiredChain->GetComponent(aType);
		}
	return ptr;
	}
	
// ---------------------------------------------------------------------------
// CAudioStream::Message
// ---------------------------------------------------------------------------
TInt CAudioStream::Message(MLogicalChain& aCurrentChain, MLogicalChain& aDesiredChain, MAudioContext& aContext, TInt aFlags)
	{
	DP_CONTEXT(CAudioStream::Message *CD0*, CtxDevSound, DPLOCAL);
	DP_IN();
	TInt err = KErrNone;

	iCurrentChain = &aCurrentChain;
	iDesiredChain = &aDesiredChain;

	TUint messageType = aDesiredChain.MessageType();

	iAudioContext = &aContext;

	CAudioContext* audioContext = static_cast<CAudioContext*> (iAudioContext);
	// TO NOTIFY DIRECTLY CONTEXT
	MMultimediaResourceControlObserver* mmrcObserver = static_cast<MMultimediaResourceControlObserver*>(audioContext);

	// Register stream observer
	if (messageType & ERegisterStreamObserver != 0)
		{
		// Use MMRC extension mechanism
		TAny* ptr = GetComponent(KUidAudioStreamAdaptationObserver);	
		MAudioStreamAdaptationObserver* observer = static_cast<MAudioStreamAdaptationObserver*>(ptr);
		if(observer)
			{
			err = RegisterAudioStreamObserver(*observer);	
			}
		}
	
	// Register codec observer
	if (messageType & ERegisterCodecObserver != 0)
		{
		// Use MMRC extension mechanism
		TAny* ptr = GetComponent(KUidAudioCodecObserver);
		MAudioCodecObserver* observer = static_cast<MAudioCodecObserver*>(ptr);
		if (observer) 
			{
			err = RegisterAudioCodecObserver(*observer);	
			}
		}

	// Component creation
	TUint bit = messageType & EComponentCreation;
	if( (err == KErrNone) && (bit != 0) )
		{
		CAudioContext* context = static_cast<CAudioContext*>(iAudioContext);
		MLogicalChain* clientDesiredChain = context->GetLogicalChain(0);	
		err = CreatePhysicalComponents(*clientDesiredChain);

		// For Configuration
		if (err==KErrNone)
			{
			clientDesiredChain->SetAdaptationStream(*this);
			clientDesiredChain->SetStreamBufferControl(*this);
			}
		}
	
	// Component alteration
	// Changes that must be applied before stream state transition
	TBool configCodec = EFalse;
	bit = messageType & EComponentAlterationCodec;
	if( (err == KErrNone) && (bit != 0) )
		{
		// Set Format
		if(iCodec)
			{
			TUid desiredCodecFormat  = iDesiredChain->CodecFormat();
			err = iCodec->SetFormat( desiredCodecFormat );
			} 
		
		if(err == KErrNone)
			{
			//if samplerate or mode has changed, update value and trigger callbacks at appropriate point
			if ((iDesiredChain->GetSampleRate() > 0) && (iDesiredChain->GetSampleRate() != iSampleRateConfig))
				{
				iSampleRateConfig = iDesiredChain->GetSampleRate();
				configCodec = ETrue;
				}
			if ((iDesiredChain->GetMode() != KNullUid) && (iDesiredChain->GetMode() != iModeConfig))
				{
				iModeConfig  = iDesiredChain->GetMode();
				configCodec = ETrue;
				}
			}
		}


	bit = messageType & EComponentAlterationGain;
	if( (err == KErrNone) && (bit != 0) )
		{
		// Apply volume ramp
		// Note that ramp operation and gain now are applied separately because current tone arquitecture 
		// need the volume ramp to be set before start the tonehwdevice 
		TTimeIntervalMicroSeconds currentRampTimeValue = 0; 
		TUid currentRampTimeOperation(KNullUid);
		TTimeIntervalMicroSeconds desiredRampTimeValue = 0; 
		TUid desiredRampTimeOperation(KNullUid);
		iCurrentChain->GetVolumeRampParameters(currentRampTimeOperation, currentRampTimeValue);
		iDesiredChain->GetVolumeRampParameters(desiredRampTimeOperation, desiredRampTimeValue);
		
		if(currentRampTimeOperation != desiredRampTimeOperation ||
		currentRampTimeValue.Int64() != desiredRampTimeValue.Int64() )
			{
			if (iGainControl)
				{
				err = iGainControl->ConfigureRamp(desiredRampTimeOperation, desiredRampTimeValue);
				}
			}
		}

	//Configuration request
	// Stream state
	TBool invokeStateEventCallback = EFalse;
	iDesiredStreamState = iDesiredChain->StreamState();
	if( (err == KErrNone) && (iCurrentStreamState != iDesiredStreamState) )
		{
		err = ChangeState(iCurrentStreamState, iDesiredStreamState);
		if (err == KErrNone)
			{
			iCurrentStreamState = iDesiredStreamState;
			}
		invokeStateEventCallback = ETrue;
		}

	// Component alteration
	// Changes that must be applied after stream state transition
	TBool gainUpdated = EFalse;
	bit = messageType & EComponentAlterationGain;
	if( (err == KErrNone) && (bit != 0) )
		{
		TAny* ptr = GetComponent(KUidAudioGainControl);
		MAudioGainControl* gaincontrol = static_cast<MAudioGainControl*>(ptr);
		if (iGainControl && gaincontrol)
			{
			RArray<TAudioChannelGain> channels;
			TInt err = gaincontrol->GetGain(channels);
			if (channels.Count() != 0 )
				{
				err = iGainControl->SetGain(channels);
				gainUpdated = ETrue;
				}
			channels.Close();
			}
		}
	
	TBool invokeCodecCallbacks = EFalse;
	bit = messageType & EComponentAlterationCodec;
	if ( (err == KErrNone) && (bit != 0) && configCodec && (iCurrentStreamState == EInitialized) )
		{
		//codec loading actually configures sample rate and mode
		ASSERT(iCodec);
		err = iCodec->Load(iSampleRateConfig, iModeConfig);
		iIsCodecConfig = (err == KErrNone);
		invokeCodecCallbacks = ETrue;
		if ( err != KErrNone )
			{
			//get back to previous values in case of error
			iSampleRateConfig = iCurrentChain->GetSampleRate();
			iModeConfig = iCurrentChain->GetMode();
			}
		}

	// Component destruction
	bit = messageType & EComponentDestruction;
	if( (err == KErrNone) && (bit != 0) )
		{
		DeletePhysicalComponents();
		}

	TUint isStopping = aFlags & KServerStopping;
	TUint preemptionRequest = aFlags & KPreemptionRequest;

	// HERE WE CAN GUARANTEE THAT THE REQUEST IS SUCCESFUL
	// Notify context 
	// 1ST CALLBACK
	if(!preemptionRequest) 
		{
		mmrcObserver->ReceiveResourceUpdate(&aDesiredChain, KErrNone);
		}
	else 
		{
		mmrcObserver->ReceivePreemptionUpdate(&aDesiredChain, err);
		}

	// Processing unit callbacks
	// Gain control
	// Note that due to error checking before applying any change 
	// this callback always returned the error obtained by calling SetGain
	// or KErrNone
	if(gainUpdated)
		{
		if (iGainControl)
			{
			iGainControl->IssueGainChangedCallBack(err);
			}
		}

	// Stream 
	if(invokeStateEventCallback)
		{
		invokeStateEventCallback = EFalse;
		for ( TUint idx(0); idx < iAudioStreamObservers.Count(); idx++ )
			{
			if ( !isStopping )
				{
				iAudioStreamObservers[idx]->StateEvent(err, iCurrentStreamState);
				}
			}
		}
	if( invokeCodecCallbacks && (iCurrentStreamState == EInitialized) )
		{
		TInt count = iAudioCodecObservers.Count();
		for ( TInt idx = 0; idx < count; idx++ )
			{
			//TODO replace this functionality with the new mmrc
			iAudioCodecObservers[idx]->SampleRateSet(err);
			iAudioCodecObservers[idx]->ModeSet(err);
			}
		}

	// Now has no effect on context
	// But it's needed to let the MMRC know about the operation being completed
	// and in such way let it to know that 
	for ( TUint idx(0); idx < iAudioStreamObservers.Count(); idx++ )
		{
		if ( !isStopping )
			{
			iAudioStreamObservers[idx]->PhysicalAdaptationEvent(EOperationComplete, err);
			}
		}

	// Don't need to send last callback sync
	// Let MMRC do it

	DP0_RET(err, "%d");
	}

// ---------------------------------------------------------------------------
// CAudioStream::DeletePhysicalComponents
// ---------------------------------------------------------------------------
void CAudioStream::DeletePhysicalComponents()
	{
	DP_CONTEXT(CAudioStream::DeletePhysicalComponents *CD0*, CtxDevSound, DPLOCAL);
	DP_IN();
	if(iBufferSource)
		{
		delete iBufferSource;
		iBufferSource = NULL;
		}
	if(iBufferSink)
		{
		delete iBufferSink;
		iBufferSink = NULL;
		}
	if(iCodec)
		{
		delete iCodec;
		iCodec = NULL;
		iIsCodecConfig = EFalse;
		}
	if(iGainControl)
		{
		delete iGainControl;
		iGainControl = NULL;
		}
	if(iDeviceSource)
		{
		delete iDeviceSource;
		iDeviceSource = NULL;
		}
	if(iDeviceSink)
		{
		delete iDeviceSink;
		iDeviceSink= NULL;
		}
	DP_OUT();
	}


// ---------------------------------------------------------------------------
// CAudioStream::CreatePhysicalComponents
// ---------------------------------------------------------------------------
TInt CAudioStream::CreatePhysicalComponents(MLogicalChain& aDesiredChain)
	{
	DP_CONTEXT(CAudioStream::CreatePhysicalComponents *CD0*, CtxDevSound, DPLOCAL);
	DP_IN();
	
	TInt err = KErrNone;
	TInt units = aDesiredChain.AudioProcessingUnitsCount();
	TInt index=0;
	TUid typeId;

	for (index=0; index < units ; index++)
		{
		typeId = aDesiredChain.AudioProcessingUnitUid(index);

		// By the moment all components 
		if (err == KErrNone)
			{
			if (typeId == KUidAudioDecoder || typeId == KUidAudioEncoder )
				{
				if(!iCodec)
					{
					TRAP(err, iCodec = CAudioCodec::NewL(typeId, *iFourCCConvertor ));
					iIsCodecConfig = EFalse;
					iCodec->RegisterAudioCodecObserver(*this);
					TAny* ptr = GetComponent(KUidAudioCodecAdaptationObserver);
					MAudioCodecAdaptationObserver* observer = static_cast<MAudioCodecAdaptationObserver*>(ptr);
					if(observer)
						{
						iCodec->RegisterAudioCodecObserver(*observer);
						}
					// For HW custom interface
					aDesiredChain.SetCustomInterfaceProvider(*iCodec);
					}
				if(iGainControl)
					{
					iGainControl->SetHelper(*iCodec);
					}
				// This mechanism is temporary and must  be replaced
				// with the Extension mechanism when the MMRC server
				aDesiredChain.SetStreamPositionControl(*iCodec);
				}
			else if (typeId == KUidMmfBufferSource)
				{
				if(!iBufferSource)
					{
					TRAP(err,iBufferSource = CBufferSource::NewL());
					// This mechanism is temporary and must  be replaced
					// with the Extension mechanism when the MMRC server
					aDesiredChain.SetAdaptationSource(*iBufferSource);
					/*
					TAny* ptr = aDesiredChain.GetComponent(KUidMmfBufferSource);
					MMMFAudioDataSupplier* supplier = static_cast<MMMFAudioDataSupplier*>(ptr);
					if(supplier)
						{
						iBufferSource->SetDataSupplier(*supplier);
						}
					*/
					}
				}
			else if (typeId == KUidMmfBufferSink)
				{
				if(!iBufferSink)
					{
					TRAP(err, iBufferSink = CBufferSink::NewL());
					// This mechanism is temporary and must  be replaced
					// with the Extension mechanism when the MMRC server
					aDesiredChain.SetAdaptationSink(*iBufferSink);
					}
				}
			else if (typeId == KUidAudioGainControl)
				{
				if(!iGainControl)
					{
					// This mechanism is temporary and must  be replaced
					// with the Extension mechanism when the MMRC server
					TRAP(err, iGainControl = CAudioGainControl::NewL());
					aDesiredChain.SetAdaptationGainControl(*iGainControl);
					}
				}
			else if (typeId == KUidAudioDeviceSink)
				{
				if(!iDeviceSink)
					{
					TRAP(err, iDeviceSink = CAudioDeviceSink::NewL());
					}
				}
			else if (typeId == KUidAudioDeviceSource)
				{
				if(!iDeviceSource)
					{
					TRAP(err, iDeviceSource = CAudioDeviceSource::NewL());
					}
				}
			else
				{
				err = KErrNotSupported;
				}
			}
		
		// Notify the observers
		for ( TUint idx(0); idx < iAudioStreamObservers.Count(); idx++ )
			{
			iAudioStreamObservers[idx]->AddProcessingUnitComplete(typeId, err);
			}
		}
	DP0_RET(err, "%d");
	}

// ---------------------------------------------------------------------------
// CAudioStream::CreateDataPath
// ---------------------------------------------------------------------------
TInt CAudioStream::CreateDataPath()
	{
	DP_CONTEXT(CAudioStream::CreateDataPath *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	TInt err(KErrNotReady);

	if(iBufferSource && iCodec) // && iSink && iGain)
		{
		// TMode == Decode
		if(KErrNone == iBufferSource->GetOutputPort(iOutputport) && KErrNone == iCodec->GetInputPort(iInputport))
			{
			iOutputport->SetInput(iInputport);
			iInputport->SetOutput(iOutputport);
			err = KErrNone;
			}
		}
	else if(iBufferSink && iCodec) // && iSink && iGain)
		{
		//TMode == Encode
		if(KErrNone == iCodec->GetOutputPort(iOutputport)  && KErrNone == iBufferSink->GetInputPort(iInputport))
			{
			iOutputport->SetInput(iInputport);
			iInputport->SetOutput(iOutputport);
			err = KErrNone;
			}
		}
	DP0_RET(err, "%d");
	}

// ---------------------------------------------------------------------------
// CAudioStream::DemolishDataPath
// ---------------------------------------------------------------------------
//
TInt CAudioStream::DemolishDataPath()
	{
	DP_CONTEXT(CAudioStream::DemolishDataPath *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	iOutputport->RemoveInput(iInputport);
	iInputport->RemoveOutput(iOutputport);
	DP0_RET(KErrNone, "%d");
	}

// ---------------------------------------------------------------------------
// CAudioStream::ChangeState
// ---------------------------------------------------------------------------	
TInt CAudioStream::ChangeState(TAudioState aPreviousState, TAudioState aDesiredState)
	{
	DP_CONTEXT(CAudioStream::ChangeState *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	TInt err(KErrNone);
	iCurrentStreamState = aPreviousState;
	iDesiredStreamState = aDesiredState;

	// Ensure that there is no dereference of a NULL pointer
	ASSERT(iDesiredStreamState < EInitialized || iDesiredStreamState > EActive || iCodec);
	
	switch (iDesiredStreamState)
		{
		case EInitialized:
			{
			if (iCurrentStreamState == EUninitialized) //Initialize
				{
				err = CreateDataPath();
				if(err == KErrNone)
					{
					err = iCodec->Initialize();
					}
				}
			else if( iCurrentStreamState == EIdle ) //codec unload (actually, unconfig)
				{
				iIsCodecConfig = EFalse;
				err = KErrNone;
				}
			// Preemption 
			// This A3F adaptation allows going from Active/Primed directly to initialised
			// otherwise reference MMRC would need to handle those transitions separately 
			else if(iCurrentStreamState == EActive || iCurrentStreamState == EPrimed) 
				{
				// To Idle 
				err = iCodec->Stop();
				// To Initilised
				iIsCodecConfig = EFalse;
				}
				
			if(err == KErrNone)
				{
				iCurrentStreamState = EInitialized;
				}
			break;
			}
		case EIdle:
			{
			if ( (iCurrentStreamState == EInitialized) && !iIsCodecConfig )
				{
				//codec loading actually configures sample rate and mode
				err = iCodec->Load(iSampleRateConfig, iModeConfig);
				iIsCodecConfig = (err == KErrNone);
				}
			else if (iCurrentStreamState == EActive)
				{
				err = iCodec->Stop();
				}
			else if (iCurrentStreamState == EPrimed)
				{
				err = iCodec->Stop();
				}

			if(err == KErrNone)
				{
				iTimeProcessed = 0;
				iCurrentStreamState = EIdle;
				}
			break;
			}
		case EPrimed:
			{
			if (iCurrentStreamState == EIdle)
				{
				DP0(DLINFO,"==============  Stream is going from EIdle -> PRIMED");
				DP0(DLINFO,"Nothing to be done");
				}
			else if (iCurrentStreamState == EActive)
				{
				DP0(DLINFO,"============== Stream is going from EActive -> PRIMED");
				err = iCodec->Pause();
				}
			if(err == KErrNone)
				{
				iCurrentStreamState = EPrimed;
				}
			break;
			}
		case EActive:
			{
			if (iCurrentStreamState == EPrimed) //Activate from Primed
				{
				// Reusing AudioCodec::Start for resuming
				DP0(DLINFO,"==============  Stream is going from EPrimed -> ACTIVE");
				err = iCodec->Start();
				}
			else if(iCurrentStreamState == EIdle) //Activate from Idle
				{
				DP0(DLINFO,"==============  Stream is going from EIdle -> ACTIVATE");
				err = iCodec->Start();
				}

			if(err == KErrNone)
				{
				iCurrentStreamState = EActive;
				}
			break;
			}
		case EUninitialized:
			{
			err = DemolishDataPath();
			if(err == KErrNone)
				{
				iCurrentStreamState = EUninitialized;
				}
			break;
			}
		case EDead:
			{
			err = DemolishDataPath();
			if(err == KErrNone)
				{
				iCurrentStreamState = EDead;
				}
			break;
			}
		default:
			err = KErrNotSupported;
			DP1(DLINFO,"CAudioStream::ChangeState Unknown state! %d", iDesiredStreamState);
			break;
		}
	DP0_RET(err, "%d");
	}

TInt CAudioStream::FlushBuffers()
	{
	DP_CONTEXT(CAudioStream::FlushBuffers *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	
	TInt err = KErrNone;

	MOutputPort* outPort= static_cast<MOutputPort*>(iCodec);
	if(outPort)
		{
		outPort->FlushBuffer(this);
		}
	else
		{
		err = KErrNotReady;
		}
	err = KErrNone;
	DP0_RET(err, "%d");
	}


void CAudioStream::FlushComplete(TInt aError)
	{
	DP_CONTEXT(CAudioStream::FlushComplete *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	TUint count = iAudioStreamObservers.Count();
	for ( TUint i(0); i < count; i++ )
		{
		iAudioStreamObservers[i]->FlushComplete(aError);
		}
	DP_OUT();
	}

void CAudioStream::AllBuffersProcessed()
	{
	DP_CONTEXT(CAudioStream::AllBuffersProcessed *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	TUint count = iAudioStreamObservers.Count();
	for ( TUint i(0); i < count; i++ )
		{
		iAudioStreamObservers[i]->ProcessingFinished();
		}
	DP_OUT();
	}

void CAudioStream::ProcessingUnitError(TInt /*aError*/)
	{
	DP_CONTEXT(CAudioStream::ProcessingUnitError *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	DP_OUT();
	}

// ---------------------------------------------------------------------------
// CAudioStream::RegisterAudioCodecObserver
// ---------------------------------------------------------------------------
TInt CAudioStream::RegisterAudioCodecObserver(MAudioCodecObserver& aObserver)
	{
	DP_CONTEXT(CAudioStream::RegisterAudioCodecObserver *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	TInt err = iAudioCodecObservers.Find(&aObserver);
	if(err == KErrNotFound)
		{
		err = iAudioCodecObservers.Append(&aObserver);
		}
	else
		{
		err = KErrAlreadyExists;
		}
	DP0_RET(err, "%d");
	}

// ---------------------------------------------------------------------------
// CAudioStream::UnregisterAudioCodecObserver
// ---------------------------------------------------------------------------
void CAudioStream::UnregisterAudioCodecObserver(MAudioCodecObserver& aObserver)
	{
	DP_CONTEXT(CAudioStream::UnregisterAudioCodecObserver *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	TInt idxOrErr = iAudioCodecObservers.Find(&aObserver);
	if( idxOrErr != KErrNotFound )
		{
		iAudioCodecObservers.Remove(idxOrErr);
		}
	DP_OUT();
	}

void CAudioStream::GetSupportedAModesComplete(TInt /*aError*/)
	{
	DP_CONTEXT(CAudioStream::GetSupportedAModesComplete *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	DP_OUT();
	}

void CAudioStream::GetSupportedARatesComplete(TInt /*aError*/)
	{
	DP_CONTEXT(CAudioStream::GetSupportedARatesComplete *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	DP_OUT();
	}

// ---------------------------------------------------------------------------
// CAudioStream::GetSupportedSampleRates
// ---------------------------------------------------------------------------
TInt CAudioStream::GetSupportedSampleRates(RArray<TInt>& aSupportedRates)
	{
	TInt err = (KErrNotReady);
	err = iCodec->SupportedRates(aSupportedRates);
	return err;
	}

// ---------------------------------------------------------------------------
// CAudioStream::GetSupportedModes
// ---------------------------------------------------------------------------
TInt CAudioStream::GetSupportedModes(RArray<TUid>& aSupportedModes)
	{
	TInt err = (KErrNotReady);
	err = iCodec->SupportedModes(aSupportedModes);
	return err;
	}

// end of file
