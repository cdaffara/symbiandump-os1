//audiocodec.cpp

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
#include <a3f/a3ffourcclookup.h>
#include <a3f/audioprocessingunittypeuids.h>
#include "maudiocodecadaptationobserver.h"
#include "audiocodec.h"
#include "resourcedata.h"

// THIS WILL REMAIN UNTIL THE REQUIREMENT WILL BE DEFINED
// THIS IMPLEMENTATION WILL WORK ONLY FOR HW DEVICES IMPLEMNETED AS SOFTWARE CODEC WRAPPERS
#define INCLUDE_HWDEVICES_AS_SWCODEC_WRAPPERS

#ifdef INCLUDE_HWDEVICES_AS_SWCODEC_WRAPPERS
#include <mmf/server/mmfswcodecwrappercustominterfacesuids.hrh>
#endif

const TInt KMicroSecsInOneSec = 1000000;
const TInt KHwDeviceDefaultDataLength= 9;
const TUint KDefaultMaxGain = 255; // TODO should discover at run time but this is value of current Symbian sound driver adaptor

//Values to find the request ask by the client
const TInt KRequestSampleRate = 0x0001;
const TInt KRequestMode = 0x0002;
const TAudioModeTableEntry KModeTableLookup[] = {
							{ EMMFMono, {KA3FModeMonoValue} },
							{ EMMFStereo, {KA3FModeStereoNonInterleavedValue} },
							};


// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
CAudioCodec::CAudioCodec(TUid aTypeId, const CFourCCConvertor& aFourCCConvertor)
	: CActive(EPriorityStandard),
	iLastBuffer(EFalse),
	iHwDeviceState(EIdle),
	iLastBufferAck(EFalse), 
	iRampOperation(KNullUid)
	{
	TRACE_CREATE();
	DP_CONTEXT(CAudioCodec::CAudioCodec *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	if(aTypeId==KUidAudioEncoder)
		{
		iMode = EEncode;
		}
	else if(aTypeId==KUidAudioDecoder)
		{
		iMode = EDecode;
		}
	iFourCCConvertor = static_cast<CFourCCConvertor*>( const_cast<CFourCCConvertor*>(&aFourCCConvertor) );

	iHwDeviceInitArgs.iEapStreamId = 0;
	iHwDeviceInitArgs.iPolicyId = 0;
	DP_OUT();
	}

// ---------------------------------------------------------------------------
// Factory method
// ---------------------------------------------------------------------------
EXPORT_C CAudioCodec* CAudioCodec::NewL(TUid aTypeId, const CFourCCConvertor& aFourCCConvertor)
	{
	DP_STATIC_CONTEXT(CAudioCodec::NewL *CD0*, CtxDevSound, DPLOCAL);
	DP_IN();
	CAudioCodec* self = new(ELeave)CAudioCodec(aTypeId, aFourCCConvertor);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	DP0_RET(self, "0x%x");
	}

// ---------------------------------------------------------------------------
// Second phase constructor
// ---------------------------------------------------------------------------
void CAudioCodec::ConstructL()
	{
	DP_CONTEXT(CAudioCodec::ConstructL *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	CActiveScheduler::Add(this);
	DP_OUT();
	}

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
CAudioCodec::~CAudioCodec()
	{
	DP_CONTEXT(CAudioCodec::~CAudioCodec *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	iAudioCodecObservers.Close();
	if (iHwDevice)
		{
		if (!iHwDeviceState == EStopped)
			{
			DP0(DLINFO, "StopAndDeleteCodec");
			iHwDevice->StopAndDeleteCodec();
			}
		delete iHwDevice;
		iHwDevice = NULL;
		}
	Cancel();
	DP_OUT();
	}

// ---------------------------------------------------------------------------
// CAudioCodec::SetFormat
// ---------------------------------------------------------------------------
EXPORT_C TInt CAudioCodec::SetFormat(TUid aFormat)
	{
	DP_CONTEXT(CAudioCodec::SetFormat *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	iFormat = aFormat;
	DP0_RET(KErrNone, "%d");
	}

// ---------------------------------------------------------------------------
// CAudioCodec::Initialize
// ---------------------------------------------------------------------------
EXPORT_C TInt CAudioCodec::Initialize()
	{
	DP_CONTEXT(CAudioCodec::Initialize *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	TInt err(KErrNone);
	TFourCC aFourCC(0);
	err = iFourCCConvertor->FormatToFourCC(iFormat,aFourCC);
	if(err == KErrNotFound)
		{
		err = KErrNotSupported;
		}
	if(err == KErrNone)
		{
		TRAP(err,  FourCCToHwDeviceUidL(aFourCC, iMode, iDeviceUid) );
		}
	if (err == KErrNone)
		{
		err = LoadHwDevice();
		}
		
#ifdef INCLUDE_HWDEVICES_AS_SWCODEC_WRAPPERS
	if (err == KErrNone)
		{
		TUid interfaceUid = {0};
		TAny* interface = NULL;
		if (iMode == EDecode)
			{
			interfaceUid.iUid = KMmfPlaySettingsCustomInterface;
			}
		else if(iMode == EEncode)
			{
			interfaceUid.iUid = KMmfRecordSettingsCustomInterface;
			}
		
		// This call at the sw codec wrapper creates the custom interfaces for playing and record
		if(iHwDevice)
			{
			interface = iHwDevice->CustomInterface(interfaceUid);
			if (iMode == EDecode)
				{
				iPlayCustomInterface = static_cast<MPlayCustomInterface*>(interface);
				}
			else if(iMode == EEncode)
				{
				iRecordCustomInterface = static_cast<MRecordCustomInterface*>(interface);
				}
			}
		}

#endif
	if(err == KErrNone)
		{
		err = InitHwDevice();
		}
	DP0_RET(err, "%d");
	}
	
// ---------------------------------------------------------------------------
// CAudioCodec::Load
// ---------------------------------------------------------------------------
EXPORT_C TInt CAudioCodec::Load(TInt aSampleRate, TUid aMode)
	{
	DP_CONTEXT(CAudioCodec::Load *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	iSampleRateConfig = aSampleRate;
	iModeConfig = aMode;
	TInt err(KErrNone);
	err = SetHwDeviceConfig();
	DP0_RET(err, "%d");
	}

// ---------------------------------------------------------------------------
// CAudioCodec::Start
// ---------------------------------------------------------------------------
EXPORT_C TInt CAudioCodec::Start()
	{
	DP_CONTEXT(CAudioCodec::Start *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	TInt err(KErrNone);
	iLastBuffer = EFalse;
	
	// Needed since ToneHwDevice only exchange one buffer with its client
	// Resume shouldn't clear the last buffer flag 
	// since this wont' result on a new buffer request
	if(iHwDeviceState == EPaused && iDeviceUid.iUid == KUidToneHwDevice)
		{
		iLastBuffer = ETrue;
		}

	if (iHwDevice)
		{
		PRF(PRF_ID, PRF_START, PRF_TIME, AA_DS_StartHwDevice, "");
		if (iMode == EDecode)
			{
			// PLAY
			err = iHwDevice->Start(EDevDecode, EDevOutFlow);
			DP1(DLINFO,"hwdevice decode start: error %d",err);
			iIgnoreUnderflowInterface = reinterpret_cast<MIgnoreUnderflowEventsCustomInterface*>(iHwDevice->CustomInterface(KIgnoreUnderflowCustomInterfaceTypeUid));	
			if(iIgnoreUnderflowInterface)
				{
				iIgnoreUnderflowInterface->IgnoreUnderflowEvents();
				}
			}
		else if (iMode == EEncode)
			{
			// RECORD
			err = iHwDevice->Start(EDevEncode, EDevInFlow);
			DP1(DLINFO,"hwdevice encode start: error %d",err);
			}
		else
			{
			err = KErrNotSupported;
			DP1(DLINFO,"CAudioCodec::StartHwDevice - Unknown mode %d", iMode);
			}
		PRF(PRF_ID, PRF_STOP, PRF_TIME, AA_DS_StartHwDevice, "");
		}
	else
		{
		err = KErrHardwareNotAvailable;
		}

	if (err == KErrNone)
		{
		iHwDeviceState = ERunning;
		}
	iLastBufferAck = EFalse;	
	DP0_RET(err, "%d");
	}

// ---------------------------------------------------------------------------
// CAudioCodec::Pause
// ---------------------------------------------------------------------------
EXPORT_C TInt CAudioCodec::Pause()
	{
	DP_CONTEXT(CAudioCodec::Pause *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	TInt err(KErrNone);
	if (iHwDevice)
		{
		DP0(DLINFO,"Pause hwdevice");
		err = iHwDevice->Pause();
		}
	iHwDeviceState = EPaused;
	DP0_RET(err, "%d");
	}

// ---------------------------------------------------------------------------
// CAudioCodec::Stop
// ---------------------------------------------------------------------------
EXPORT_C TInt CAudioCodec::Stop()
	{
	DP_CONTEXT(CAudioCodec::Stop *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	TInt err(KErrNone);
	if (iHwDevice && !(iHwDeviceState == EStopped)) 
		{
		DP0(DLINFO,"Stop hwdevice");
		UpdateBytesPlayed();
		err = iHwDevice->Stop();
		iHwDeviceState = EStopped;
		}
	iLastBufferAck = EFalse;
	DP0_RET(err, "%d");
	}

// ---------------------------------------------------------------------------
// CAudioCodec::GetSupportedSampleRates
// ---------------------------------------------------------------------------
TInt CAudioCodec::GetSupportedSampleRates(RArray<TInt>& aSupportedRates)
	{
	DP_CONTEXT(CAudioCodec::GetSupportedSampleRates *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	TInt err(KErrNone);
	aSupportedRates.Reset();
	// Ask for rates from resource files
	err = ReadResourceRates(aSupportedRates);

	DP0_RET(err, "%d");
	}

// ---------------------------------------------------------------------------
// CAudioCodec::GetSupportedModes
// ---------------------------------------------------------------------------
TInt CAudioCodec::GetSupportedModes(RArray<TUid>& aSupportedModes)
	{
	DP_CONTEXT(CAudioCodec::GetSupportedModes *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	TInt err(KErrNone);
	aSupportedModes.Reset();
	// Ask for modes from resource files
	err = ReadResourceModes(aSupportedModes);

	DP0_RET(err, "%d");
	}

// ---------------------------------------------------------------------------
// CAudioCodec::GetCustomInterface
// ---------------------------------------------------------------------------
TInt CAudioCodec::RequestCustomInterface(TUid aUid, TAny*& aPtr)
	{
	DP_CONTEXT(CAudioCodec::GetCustomInterface *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	TInt err = KErrNone;
	if(iHwDevice)
		{
		aPtr = iHwDevice->CustomInterface(aUid);
		}
	else
		{
		err = KErrNotReady;
		}
	DP0_RET(err, "%d");
	}

// ---------------------------------------------------------------------------
// CAudioCodec::RegisterObserver
// ---------------------------------------------------------------------------
TInt CAudioCodec::RegisterObserver(MCustomInterfaceSupportObserver& aObserver)
	{
	DP_CONTEXT(CAudioCodec::RegisterObserver *CD1*, CtxDevSound, DPLOCAL);
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
// CAudioCodec::UnRegisterObserver
// ---------------------------------------------------------------------------
void CAudioCodec::UnRegisterObserver(MCustomInterfaceSupportObserver& aObserver)
	{
	DP_CONTEXT(CAudioCodec::UnRegisterObserver *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	TInt idxOrErr = iCISupportObservers.Find(&aObserver);
	if( idxOrErr != KErrNotFound )
		{
		iCISupportObservers.Remove(idxOrErr);
		}
	DP_OUT();
	}

// ---------------------------------------------------------------------------
// CAudioCodec::LoadHwDevice
// ---------------------------------------------------------------------------
TInt CAudioCodec::LoadHwDevice()
	{
	DP_CONTEXT(CAudioCodec::LoadHwDevice *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	TInt err(KErrNone);

	if(iDeviceUid==KNullUid)
		{
		DP0_RET(KErrNotSupported, "%d");
		}

	TRAP(err, iHwDevice = CMMFHwDevice::NewL(iDeviceUid));
	if(err == KErrNotFound)
		{
		err = KErrNotSupported;
		}
	DP0_RET(err, "%d");
	}

// ---------------------------------------------------------------------------
// CAudioCodec::InitHwDevice
// ---------------------------------------------------------------------------
TInt CAudioCodec::InitHwDevice()
	{
	DP_CONTEXT(CAudioCodec::InitHwDevice *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	TInt err(KErrNone);
	DP1(DLINFO, "eapStreamId %d", iHwDeviceInitArgs.iEapStreamId);
	DP1(DLINFO, "policyId %d", iHwDeviceInitArgs.iPolicyId);

	iHwDeviceInitParams.iHwDeviceObserver = this;
	iHwDeviceInitParams.iHwDeviceInitArgsPtr.Set((TUint8*)&(iHwDeviceInitArgs),
														sizeof(THwDeviceInitArgs),
														sizeof(THwDeviceInitArgs));
	if (iHwDevice)
		{
		err = iHwDevice->Init(iHwDeviceInitParams);
		}
	else
		{
		err = KErrHardwareNotAvailable;
		}
	DP0_RET(err, "%d");
	}

// ---------------------------------------------------------------------------
// CAudioCodec::SetHwDeviceConfig
// ---------------------------------------------------------------------------
TInt CAudioCodec::SetHwDeviceConfig()
	{
	DP_CONTEXT(CAudioCodec::SetHwDeviceConfig *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	TInt err(KErrNone);

	//TODO Look for another way to set those parameters
	TTaskConfig taskconfig;
	taskconfig.iUid = KUidRefDevSoundTaskConfig;
	taskconfig.iStereoMode = ResolveMode(iModeConfig);
	taskconfig.iRate = iSampleRateConfig;

	DP1(DLINFO, "taskconfig.iRate %d", taskconfig.iRate);
	DP1(DLINFO, "taskconfig.iStereoMode %d", taskconfig.iStereoMode);
	DP1(DLINFO, "taskconfig.iUid %d", taskconfig.iUid);

	if (err == KErrNone && iHwDevice)
		{
		err = iHwDevice->SetConfig(taskconfig);
		}
	else
		{
		err = KErrHardwareNotAvailable;
		}
	DP0_RET(err, "%d");
	}

// ---------------------------------------------------------------------------
// CAudioCodec::ReadResourceRates
// ---------------------------------------------------------------------------
TInt CAudioCodec::ReadResourceRates(RArray<TInt>& aRates)
	{
	DP_CONTEXT(CAudioCodec::ReadResourceRates*CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	TInt err(KErrNone);

	CResourceData* resource = NULL;
	TRAP(err, resource = CResourceData::NewL(iDeviceUid));
	if (err == KErrNone && resource)
		{
		err = resource->GetSSampleRates(iMode,aRates);
		delete resource;
		}

	DP0_RET(err, "%d");
	}

// ---------------------------------------------------------------------------
// CAudioCodec::ReadResourceModes
// ---------------------------------------------------------------------------
TInt CAudioCodec::ReadResourceModes(RArray<TUid>& aModes)
	{
	DP_CONTEXT(CAudioCodec::ReadResourceModes*CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	TInt err(KErrNone);

	CResourceData* resource = NULL;
	TRAP(err, resource = CResourceData::NewL(iDeviceUid));
	if (err == KErrNone && resource)
		{
		err = resource->GetSModes(iMode,aModes);
		delete resource;
		}

	DP0_RET(err, "%d");
	}

// ---------------------------------------------------------------------------
// from class MAudioComponent
// CAudioCodec::GetOutputPort
// ---------------------------------------------------------------------------
TInt CAudioCodec::GetOutputPort(MOutputPort*& aOutputPort)
	{
	DP_CONTEXT(CAudioCodec::GetOutputPort *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	aOutputPort = this;
	DP0_RET(KErrNone, "%d");
	}

// ---------------------------------------------------------------------------
// from class MAudioComponent
// CAudioCodec::GetInputPort
// ---------------------------------------------------------------------------
TInt CAudioCodec::GetInputPort(MInputPort*& aInputPort)
	{
	DP_CONTEXT(CAudioCodec::GetInputPort *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	aInputPort = this;
	DP0_RET(KErrNone, "%d");
	}

// ---------------------------------------------------------------------------
// from class MAudioComponent
// CAudioCodec::RegisterAudioComponentObserver
// ---------------------------------------------------------------------------
EXPORT_C TInt CAudioCodec::RegisterAudioCodecObserver(MAudioCodecAdaptationObserver& aObserver)
	{
	DP_CONTEXT(CAudioCodec::RegisterAudioCodecObserver *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	TInt err = iAudioCodecObservers.Find(&aObserver);
	if(err == KErrNotFound)
		{
		iAudioCodecObservers.Append(&aObserver);
		err = KErrNone;
		}
	else
		{
		err = KErrAlreadyExists;
		}
	DP0_RET(err,"%d");
	}

// ---------------------------------------------------------------------------
// from class MAudioComponent
// CAudioCodec::UnregisterAudioComponentObserver
// ---------------------------------------------------------------------------
EXPORT_C void CAudioCodec::UnregisterAudioCodecObserver(MAudioCodecAdaptationObserver& aObserver)
	{
	DP_CONTEXT(CAudioCodec::UnregisterAudioCodecObserver *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	TInt idxOrErr = iAudioCodecObservers.Find(&aObserver);
	if( idxOrErr != KErrNotFound )
		{
		iAudioCodecObservers.Remove(idxOrErr);
		}
	DP_OUT();
	}

// ---------------------------------------------------------------------------
// from class MInputPort
// CAudioCodec::EmptyBuffer
// ---------------------------------------------------------------------------
TInt CAudioCodec::EmptyBuffer(CMMFBuffer* /*aBuffer*/, MOutputPort* /*aSupplier*/)
	{
	DP_CONTEXT(CAudioCodec::EmptyBuffer *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	DP0_RET(KErrNotReady, "%d");
	}

// ---------------------------------------------------------------------------
// from class MInputPort
// CAudioCodec::BufferFilled
// ---------------------------------------------------------------------------
TInt CAudioCodec::BufferFilled(CMMFBuffer* aBuffer)
	{
	DP_CONTEXT(CAudioCodec::BufferFilled *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	PRF(PRF_ID, PRF_STOP, PRF_LOAD, AA_DS_Fill, "");
	TInt err(KErrNone);
	if(aBuffer->LastBuffer())
		{
		iLastBuffer = ETrue;
		}
	if(iHwDevice)
		{
		err = iHwDevice->ThisHwBufferFilled(*aBuffer);
		}
	else
		{
		err = KErrNotFound;
		}
	DP0_RET(err,"%d");
	}

// ---------------------------------------------------------------------------
// from class MInputPort
// CAudioCodec::SetOutput
// ---------------------------------------------------------------------------
TInt CAudioCodec::SetOutput(MOutputPort* aOutput)
	{
	DP_CONTEXT(CAudioCodec::SetOutput *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	TInt err = KErrNone;
	
	iOutputPort = aOutput;
	
	DP0_RET(err, "%d");
	}

// ---------------------------------------------------------------------------
// from class MInputPort
// CAudioCodec::RemoveOutput
// ---------------------------------------------------------------------------
TInt CAudioCodec::RemoveOutput(MOutputPort* /*aOutput*/)
	{
	DP_CONTEXT(CAudioCodec::RemoveOutput *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	DP0_RET(KErrNone, "%d");
	}

// ---------------------------------------------------------------------------
// from class MOutputPort
// CAudioCodec::FillBuffer
// ---------------------------------------------------------------------------
TInt CAudioCodec::FillBuffer(CMMFBuffer* /*aBuffer*/, MInputPort* /*aConsumer*/)
	{
	DP_CONTEXT(CAudioCodec::FillBuffer *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	DP0_RET(KErrNotReady, "%d");
	}

// ---------------------------------------------------------------------------
// from class MOutputPort
// CAudioCodec::BufferEmptied
// ---------------------------------------------------------------------------
TInt CAudioCodec::BufferEmptied(CMMFBuffer* aBuffer)
	{
	DP_CONTEXT(CAudioCodec::BufferEmptied *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	TInt err(KErrNone);
	
	if (!aBuffer)
		{
		err = KErrNotFound;
		DP0_RET(err, "%d");
		}

	if (iLastBufferAck) //if the hwdevice return a buffer with the lastbuffer flag set, rather than returning a non-empty buffer
		{				//in this case we generate an empty buffer on behalf of the adaptation
		TInt count = iAudioCodecObservers.Count();
		for ( TInt i(0); i < count; i++ ) // causes a ProcessingFinished() to be called on the stream
			{
			iAudioCodecObservers[i]->AllBuffersProcessed();
			}
		iLastBufferAck = EFalse;
		}
	else 
		{
		if(aBuffer->LastBuffer())
			{
			iLastBuffer = ETrue;
			}
		if(iHwDevice)
			{ 
			err = iHwDevice->ThisHwBufferEmptied(*aBuffer);
			}
		else
			{
			err = KErrNotFound;
			}
		}
	DP0_RET(err,"%d");
	}

// ---------------------------------------------------------------------------
// from class MOutputPort
// CAudioCodec::SetInput
// ---------------------------------------------------------------------------
TInt CAudioCodec::SetInput(MInputPort* aInput)
	{
	DP_CONTEXT(CAudioCodec::SetInput *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	TInt err = KErrNone;
	
	iInputPort = aInput;
	
	DP0_RET(err, "%d");
	}

// ---------------------------------------------------------------------------
// from class MOutputPort
// CAudioCodec::RemoveInput
// ---------------------------------------------------------------------------
TInt CAudioCodec::RemoveInput(MInputPort* /*aInput*/)
	{
	return KErrNone;
	}

// ---------------------------------------------------------------------------
// from class MOutputPort
// CAudioCodec::FlushBuffer
// ---------------------------------------------------------------------------
TInt CAudioCodec::FlushBuffer(MFlushHandlerObserver* aFlushObserver)
	{
	TInt err = KErrNone;
	TAny* ptr = NULL;

	// Flush low layer buffers
	if(iHwDevice)
		{
		ptr = iHwDevice->CustomInterface(TUid::Uid(KMmfUidEmptyBuffersCustomInterface));
		MMMFDevSoundEmptyBuffers* emptybuffers = static_cast<MMMFDevSoundEmptyBuffers*>(ptr);
		if (emptybuffers)
			{
			// Due to defect DEF127468, this do nothing
			err = emptybuffers->EmptyBuffers();
			}
		else
			{
			err = KErrNotSupported;
			}
		}
	else
		{
		err = KErrNotReady;
		}

	// Flush source through an asynchronous request
	if(err == KErrNone)
		{
		err = iOutputPort->FlushBuffer(aFlushObserver);
		}
	return err;
	}


// ---------------------------------------------------------------------------
// from class MMMFHwDeviceObserver
// CAudioCodec::FillThisHwBuffer
// ---------------------------------------------------------------------------
TInt CAudioCodec::FillThisHwBuffer(CMMFBuffer& aHwBuffer)
	{
	DP_CONTEXT(CAudioCodec::FillThisHwBuffer *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	PRF(PRF_ID, PRF_START, PRF_LOAD, AA_DS_Fill, "");

	TInt err(KErrNone);
	iAdaptationBuffer = static_cast<CMMFDataBuffer*> (&aHwBuffer);

	ASSERT(iAdaptationBuffer);
	if (iLastBuffer)
		{
		iAdaptationBuffer->Data().SetLength(0);
		BufferFilled(iAdaptationBuffer);
		}
	else
		{
		TRAP(err,iAdaptationBuffer->SetRequestSizeL(iAdaptationBuffer->Data().MaxLength() ));
		if(err == KErrNone)
			{
			if(iOutputPort)
				{
				err = iOutputPort->FillBuffer(&aHwBuffer, this);
				}
			else
				{
				err = KErrNotFound;
				}
			}
		}
	DP0_RET(err,"%d");
	}

// ---------------------------------------------------------------------------
// from class MMMFHwDeviceObserver
// CAudioCodec::EmptyThisHwBuffer
// ---------------------------------------------------------------------------
TInt CAudioCodec::EmptyThisHwBuffer(CMMFBuffer& aHwBuffer)
	{
	DP_CONTEXT(CAudioCodec::EmptyThisHwBuffer *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();

	TInt err(KErrNone);
	iAdaptationBuffer = static_cast<CMMFDataBuffer*> (&aHwBuffer);
	ASSERT(iAdaptationBuffer);
	if(iHwDeviceState == EPaused)
		{
		if (iAdaptationBuffer->Data().Length() == 0) // empty buffer
			{
			TInt count = iAudioCodecObservers.Count();
			for ( TInt i(0); i < count; i++ ) // causes a ProcessingFinished() to be called on the stream
				{
				iAudioCodecObservers[i]->AllBuffersProcessed();
				}
			}
		else
			{
			if(aHwBuffer.LastBuffer())
				{
				aHwBuffer.SetLastBuffer(EFalse);	// a buffer coming from hardware device should never have last buffer set...
				iLastBufferAck = ETrue;
				}
			
			TRAP(err,iAdaptationBuffer->SetRequestSizeL(iAdaptationBuffer->Data().Length()));
			if(err == KErrNone)
				{
				if(iInputPort)
					{
					err = iInputPort->EmptyBuffer(&aHwBuffer, this);
					}
				else
					{
					err = KErrNotFound;
					}
				}			
			}
		}
	else
		{
		TRAP(err,iAdaptationBuffer->SetRequestSizeL(iAdaptationBuffer->Data().Length()));
		if(err == KErrNone)
			{
			if(iInputPort)
				{
				err = iInputPort->EmptyBuffer(&aHwBuffer, this);
				}
			else
				{
				err = KErrNotFound;
				}
			}
		}
	DP0_RET(err,"%d");
	}

// ---------------------------------------------------------------------------
// from class MMMFHwDeviceObserver
// CAudioCodec::MsgFromHwDevice
// ---------------------------------------------------------------------------
TInt CAudioCodec::MsgFromHwDevice(TUid aMessageType, const TDesC8& aMsg)
	{
	DP_CONTEXT(CAudioCodec::MsgFromHwDevice *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	TBuf<50> formattedNumber;
	formattedNumber.Format(_L("MessageType 0x%08x"), aMessageType.iUid);
	TBuf<50> buf;
	buf.Copy(aMsg);
	buf.Append(formattedNumber);
	if(aMessageType.iUid == KMmfHwDeviceObserverUpdateBytesPlayed)
		{
		//This is used by sw codec wrapper to request a bytes played update
		//Bytes played won't be updated in Stopped() or Error() on sw codec wrapper
		//As the sound device is closed. Non sw codec wrapper Hw device plugins
		//Can get the bytes updated on Stopped() and/or Error()
		UpdateBytesPlayed();
		}
	DP1(DLINFO, "DeviceMessage is %S",&buf);
	DP0_RET(KErrNone, "%d");
	}

// ---------------------------------------------------------------------------
// from class MMMFHwDeviceObserver
// CAudioCodec::Stopped
// ---------------------------------------------------------------------------
void CAudioCodec::Stopped()
	{
	DP_CONTEXT(CAudioCodec::Stopped *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	//For sw codec wrappers hw devices bytes played are updated in MsgFromHwDevice
	//But non sw codec wrappers hw devices may do it differently
	//Also don't know if non sw codec wrappers hw devices will call Stopped or Error first
	UpdateBytesPlayed();
	DP_OUT();
	}

// ---------------------------------------------------------------------------
// from class MMMFHwDeviceObserver
// CAudioCodec::Error
// ---------------------------------------------------------------------------
//
void CAudioCodec::Error(TInt aError)
	{
	DP_CONTEXT(CAudioCodec::Error *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();

	TTimeIntervalMicroSeconds currentPosition;
	TInt err = KErrNone;

#ifdef _DEBUG
	RDebug::Print(_L("iDeviceUid=0x%x  aError=%d iLastBuffer=%d"), iDeviceUid.iUid, aError, iLastBuffer);
#endif 
	// The most problable receiver is the logicalaudiocodec which should notify to the client through MAudioProcessingUnit
	TUint count = iAudioCodecObservers.Count();
	
	// Notify only for the
	if (aError == KErrUnderflow && iMode == EDecode ) 
		{
		// Notify the observers
		if(iLastBuffer)
			{
			for ( TUint i(0); i < count; i++ )
				{
				iAudioCodecObservers[i]->AllBuffersProcessed();
				}
			}
		else
			{		
			// Re-start decoding

			//For sw codec wrappers hw devices bytes played are updated in MsgFromHwDevice
			//But non sw codec wrappers hw devices may do it differently
			//Also don't know if non sw codec wrappers hw devices will call Stopped or Error first
			UpdateBytesPlayed();

			err = GetControlPosition(currentPosition);
			if(err != KErrNone)
				{
				// Ignore safely
				}
			
#ifdef _DEBUG
	RDebug::Print(_L("Restart decoding"));
#endif 			
					
			Start();
			}
		}
	else if (aError == KErrOverflow && iMode == EEncode )
		{
		if (!iLastBuffer  && iHwDeviceState == ERunning)
			{
			// Re-start encoding
			err = GetControlPosition(currentPosition);
			if(err != KErrNone)
				{
				// Ignore safely
				}
			Start();
			}
		else
			{
				if (iDeviceUid.iUid != KUidToneHwDevice) 
					{
#ifdef _DEBUG
	RDebug::Print(_L("Is this is not needed for other encoders but toneHwDevice"));
#endif 
					for ( TUint i(0); i < count; i++ )
						{
						iAudioCodecObservers[i]->AllBuffersProcessed();
						}
					}
			}
		}
	else
		{
		for ( TUint i(0); i < count; i++ )
			{
			iAudioCodecObservers[i]->ProcessingUnitError(aError);
			}
		}
	DP_OUT();
	}

// HELPER CLASS
// ---------------------------------------------------------------------------
// from class MGainHelper
// CAudioCodec::GetGain
// ---------------------------------------------------------------------------
TInt CAudioCodec::GetGain(TInt& aGain) const
	{
	DP_CONTEXT(CAudioCodec::GetGain *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	TInt err = KErrNone;
	if (iMode == EDecode  && iPlayCustomInterface)
		{
		aGain = iPlayCustomInterface->Volume();
		}
	else if( iMode == EEncode  && iRecordCustomInterface) 
		{
		aGain = iRecordCustomInterface->Gain();
		}
	DP0_RET(err, "%d");
	}

// ---------------------------------------------------------------------------
// from class MGainHelper
// CAudioCodec::SetGain
// ---------------------------------------------------------------------------
TInt CAudioCodec::SetGain(RArray<TAudioChannelGain>& aChannels)
	{
	DP_CONTEXT(CAudioCodec::SetGain *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	TInt err = KErrNone;

	// Current adaptation doesn't support multichannel
	// Use average
	TUint count = aChannels.Count();
	TInt gain = 0;
	if (count)
		{
		TInt totalGain = 0;
		for (TUint i(0); i < count; i++)
			{
			totalGain  =+ aChannels[i].iGain;
			}
		gain = totalGain / count;
		}

	if (gain > KDefaultMaxGain)
		{
		gain = KDefaultMaxGain;
		}
	else if (gain < 0)
		{
		gain = 0;
		}

	if (iMode == EDecode  && iPlayCustomInterface)
		{
		iPlayCustomInterface->SetVolume(gain);
		}
	else if( iMode == EEncode  && iRecordCustomInterface) 
		{
		iRecordCustomInterface->SetGain(gain);
		}
	
	DP0_RET(err, "%d");
		}


TInt CAudioCodec::ConfigureRamp(TUid aRampOperation, const TTimeIntervalMicroSeconds& aRampDuration)
	{
	DP_CONTEXT(CAudioCodec::ConfigureRamp *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	TInt err = KErrNone;

	if (aRampOperation == KUidGainSawTooth)
		{
		if (iMode == EDecode  && iPlayCustomInterface)
			{
			iPlayCustomInterface->SetVolumeRamp(aRampDuration);
			}
		}
	else if (iRampOperation == KNullUid)
		{
		if (iMode == EDecode  && iPlayCustomInterface)
			{
			iPlayCustomInterface->SetVolumeRamp(0);
			}
		}
	else
		{
		err = KErrA3fUnsupportedRamp;
		}

	DP0_RET(err, "%d");
	}

// ---------------------------------------------------------------------------
// from class MPositionControl
// CAudioCodec::GetControlPosition
// ---------------------------------------------------------------------------
TInt CAudioCodec::GetControlPosition(TTimeIntervalMicroSeconds& aPosition)
	{
	DP_CONTEXT(CAudioCodec::GetControlPosition *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	TInt err = KErrNone;
	TInt sampleRateValue = iSampleRateConfig;

	// At this adaptation EMMFSoundEncoding16BitPCM encoding is assumed
	// Due RMdaDevSound which is always pcm16 each sample is 2 bytes
	TInt bytesPerAudioSample = 2;

	// Here is secure to convert to TInt a TAudioMode since the values 
	// are chossen according to the value they represent.
	TInt numberOfChannels = ResolveMode(iModeConfig);

	TInt64 samples = 0;
	if (err == KErrNone)
		{
		if( iMode == EDecode && iPlayCustomInterface)
			{
			TInt64 bytesPlayed = iPlayCustomInterface->BytesPlayed();
			if (bytesPlayed)
				{
				iPlayedBytesCount = bytesPlayed;
				}

			samples = iPlayedBytesCount;

			if(numberOfChannels > 1)
				{
				samples /= numberOfChannels;
				}

			if(bytesPerAudioSample > 1)
				{
				samples /= bytesPerAudioSample;
				}
			}
		else if( iMode == EEncode && iRecordCustomInterface)
			{
			samples = iRecordCustomInterface->BytesRecorded();
			if(numberOfChannels > 1)
				{
				samples /= numberOfChannels;
				}
			if(bytesPerAudioSample > 1)
				{
				samples /= bytesPerAudioSample;
				}
			}
		}

	if(sampleRateValue)
		{
		iPosition = (TInt64(samples) * KMicroSecsInOneSec / sampleRateValue);
		}
	aPosition = iPosition;
	DP1(DLINFO,"GetControlPosition Samples = %d", samples);
#ifdef _DEBUG
	RDebug::Print(_L("GetControlPosition Position=%d"), iPosition);
#endif 
	DP0_RET(err, "%d");
	}

// ---------------------------------------------------------------------------
// CAudioCodec::ResetControlPosition
// ---------------------------------------------------------------------------
void CAudioCodec::ResetControlPosition()
	{
	DP_CONTEXT(CAudioCodec::ResetControlPosition *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	iPosition = 0;
	iPlayedBytesCount = 0;
	DP_OUT();
	}

// ---------------------------------------------------------------------------
// CAudioCodec::FourCCToHwDeviceUid
// ---------------------------------------------------------------------------
void CAudioCodec::FourCCToHwDeviceUidL(TFourCC aFourCC, TMode aMode, TUid &aHWDev)
	{
	DP_CONTEXT(CAudioCodec::FourCCToHwDeviceUid *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
			
	//check argument precondition for aState
	if ((aMode != EDecode) && (aMode != EEncode))
		{
		User::Leave(KErrArgument);
		}

	// Array to return hw device plugin resource info(place on cleanupstack
	// _after_ ListImplementationsL() )
	RImplInfoPtrArray plugInArray;
	TUid KUidMmfHWPluginInterfaceCodec = {KMmfUidPluginInterfaceHwDevice};

	// ListImplementationsL leaves if it cannot find anything so trap the error
	TRAPD(err, REComSession::ListImplementationsL(KUidMmfHWPluginInterfaceCodec,
													plugInArray));
	CleanupResetAndDestroyPushL(plugInArray);	

	TUint numberOfHwDevicePlugins = plugInArray.Count();

	// if no errors and have hwdevice plugin resource entries then scan entries
	// matching on a datatype of pcm16 as the destination datatype for play and
	// the source datatype for record. If a match is found and isn't already in
	// the list of supported data types, then add it to the list
	if ((err == KErrNone) && (numberOfHwDevicePlugins))
		{
		CImplementationInformation* hwDeviceResourceEntry = NULL;
		_LIT8(KPCM16FourCCString, " P16");
		TBufC8<KFOURCCLENGTH> fourCCStringPCM16(KPCM16FourCCString);
		TPtr8 fourCCPtrPCM16 = fourCCStringPCM16.Des();

		// check each resource entry for dst 4CC = P16 for play and
		// src 4CC = P16 for record
		for (TUint hwDeviceEntry = 0;
				hwDeviceEntry < numberOfHwDevicePlugins;
				hwDeviceEntry++)
			{
			hwDeviceResourceEntry = plugInArray[hwDeviceEntry];

			if (IsDataTypeMatch(hwDeviceResourceEntry, fourCCPtrPCM16, aMode))
				{
				// resource entry data field has dest/src datatype ' P16'
				// i.e. pcm16 for play/record
				TPtrC8 fourCCPtr(0,0);

				if (aMode == EDecode)
					{
					// datatype supported 4CC is left 4 chars
					fourCCPtr.Set(
					hwDeviceResourceEntry->DataType().Left(KFOURCCLENGTH));
					}
				else if (aMode == EEncode)
					{
					// datatype supported 4CC is right 4 chars
					fourCCPtr.Set(
					hwDeviceResourceEntry->DataType().Right(KFOURCCLENGTH));
					}

				TFourCC fourCCEntry(fourCCPtr);

				// New Mapping
				if (fourCCEntry == aFourCC)
					{
					// get the Uid
					aHWDev = hwDeviceResourceEntry->ImplementationUid();
					break;
					}
				}
			}
			
		// Verify there is a HwDeviceUid
		if(aHWDev.iUid == 0)
			{
			User::Leave(KErrNotSupported);
			}
		}
	else
		{
		// if an error occured and not KErrNotFound then must be a 'real' error
		// e.g. KErrNoMemory
		if ((err != KErrNotFound) && (err != KErrNone))
			{
			User::Leave(err);
			}
		}

	CleanupStack::PopAndDestroy(&plugInArray);
	DP_OUT();
	}

// ---------------------------------------------------------------------------
// CAudioCodec::IsDataTypeMatch
// ---------------------------------------------------------------------------
TBool CAudioCodec::IsDataTypeMatch(CImplementationInformation* aHwDeviceResourceEntry,
	const TDesC8& aHwMatchFourCC, TMode aState)
	{
	DP_CONTEXT(CAudioCodec::IsDataTypeMatch *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	TBool match = EFalse;

	// Check for resource entry lenght since for HwDeviceAdaptor it is just "*"
	if (aHwDeviceResourceEntry->DataType().Length()>=KHwDeviceDefaultDataLength)
		{
		if (aState == EDecode)
			{
			//play need to match with the right four characters
			match = (!(aHwMatchFourCC.Match(
				aHwDeviceResourceEntry->DataType().Right(KFOURCCLENGTH)) ==
				KErrNotFound));
			}
		else if (aState == EEncode)
			{
			//record need to match with the left four characters
			match =
				(!(aHwMatchFourCC.Match(
				aHwDeviceResourceEntry->DataType().Left(KFOURCCLENGTH)) ==
				KErrNotFound));
			}
		}
	else
		{
#ifdef _DEBUG
		RDebug::Print(_L("HwDeviceEntry %S"), &aHwDeviceResourceEntry->DataType());
#endif			
		}

	DP0_RET(match, "0x%x");
	}

// ---------------------------------------------------------------------------
// CAudioCodec::ResolveMode
// ---------------------------------------------------------------------------
TInt CAudioCodec::ResolveMode(TUid aMode)
	{
	DP_CONTEXT(CAudioCodec::ResolveMode *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	TInt result = 0;

	//Mapping an uid mode to an int channel
	for (TUint i=0; i<=KMaxModeIndex; i++)
		{
		if(KModeTableLookup[i].iAudioMode == aMode)
			{
			result = KModeTableLookup[i].iAudioModeValue;
			break;
			}
		}
	DP0_RET(result, "%d");
	}

// ---------------------------------------------------------------------------
// CAudioCodec::SupportedRates
// ---------------------------------------------------------------------------
EXPORT_C TInt CAudioCodec::SupportedRates(RArray<TInt>& aSupportedRates)
	{
	DP_CONTEXT(CAudioCodec::Service *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	iRequest |= KRequestSampleRate;

	//Get request capabilities
	iErrorRates = GetSupportedSampleRates(aSupportedRates);
	
	// Simulate an asyncronous response
	if (!IsActive())
		{
		TRequestStatus* status = &iStatus;
		User::RequestComplete(status, KErrNone);
		SetActive();
		}
	DP0_RET(iErrorRates, "%d");
	}

// ---------------------------------------------------------------------------
// CAudioCodec::SupportedModes
// ---------------------------------------------------------------------------
EXPORT_C TInt CAudioCodec::SupportedModes(RArray<TUid>& aSupportedModes)
	{
	DP_CONTEXT(CAudioCodec::Service *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	iRequest |= KRequestMode;

	iErrorModes = GetSupportedModes(aSupportedModes);

	// Simulate an asyncronous response
	if (!IsActive())
		{
		TRequestStatus* status = &iStatus;
		User::RequestComplete(status, KErrNone);
		SetActive();
		}
	DP0_RET(iErrorModes, "%d");
	}


//From CActive
// ---------------------------------------------------------------------------
// CAudioCodec::RunL
// ---------------------------------------------------------------------------
void CAudioCodec::RunL()
	{
	DP_CONTEXT(CAudioCodec::RunL *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();

	// Send the callback for the request operation
	if (iRequest & KRequestMode)
		{
		TUint count = iAudioCodecObservers.Count();
		for ( TUint i(0); i < count; i++ )
			{
			iAudioCodecObservers[i]->GetSupportedAModesComplete(iErrorModes);
			}

		// Reset the request flag
		iRequest &= ~KRequestMode;
		}

	// Send the callback for the request operation
	if (iRequest & KRequestSampleRate)
		{
		TUint count = iAudioCodecObservers.Count();
		for ( TUint i(0); i < count; i++ )
			{
			iAudioCodecObservers[i]->GetSupportedARatesComplete(iErrorRates);
			}

		// Reset the request flag
		iRequest &= ~KRequestSampleRate;
		}

	DP_OUT();
	}

// ---------------------------------------------------------------------------
// CAudioCodec::RunError
// ---------------------------------------------------------------------------
TInt CAudioCodec::RunError(TInt aError)
	{
	DP_CONTEXT(CAudioCodec::RunError *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	DP0_RET(aError, "%d");
	}

// ---------------------------------------------------------------------------
// CAudioCodec::DoCancel
// ---------------------------------------------------------------------------
void CAudioCodec::DoCancel()
	{
	DP_CONTEXT(CAudioCodec::DoCancel *CD1*, CtxDevSound, DPLOCAL);
	DP_IN();
	iRequest = 0;
	DP_OUT();
	}

// end of file
