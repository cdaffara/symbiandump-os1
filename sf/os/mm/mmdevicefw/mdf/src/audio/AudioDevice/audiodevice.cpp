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

#include <ecom/implementationproxy.h>
#include <mmf/server/mmfdatabuffer.h>
#include "audiodevice.hrh"
#include "audiodevice.h"
#include <e32debug.h>
#include <mdf/mdfpuconfig.h>
#include <mda/common/audio.h>

// we need 16k to hold a pcm packet
const TInt KBufferSize = 16384;

const TInt KDefaultSampleRate = 8000;
const TInt KDefaultNumberChannels = 1;
_LIT(KAudioDevicePanic, "CAudioDevice Panic");

enum TAudioDevicePanics
	{
	EObserverNotSet, 
	EInvalidUsage
	};
	
const TInt KInputPortIndex = 0;
const TInt KOutputPortIndex = 1;
	
// ------------------------------------------------------------------------------------------	
// CAudioDevice::CInput port class implementation

	
CAudioDevice::CInputPort::CInputPort(CAudioDevice& aParent) 
	: CActive(EPriorityNormal),
	  iParent(aParent),
	  iSampleRate(KDefaultSampleRate),
	  iChannels(KDefaultNumberChannels),
	  iBufferSize(KBufferSize)
	{
	}
		
CAudioDevice::CInputPort::~CInputPort()
	{
	Cancel();
	iBuffers.Close();
	}

CAudioDevice::CInputPort* CAudioDevice::CInputPort::NewL(CAudioDevice& aParent)
	{
	CInputPort* self = new (ELeave) CInputPort(aParent);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

void CAudioDevice::CInputPort::ConstructL()
	{
	CActiveScheduler::Add(this);	
	}

TInt CAudioDevice::CInputPort::MipConfigure(const TPuConfig& aConfig)
	{
	if (aConfig.Uid() == TUid::Uid(KUidTTaskConfig))
		{
		const TTaskConfig* config = TPuTaskConfig::GetStructure(aConfig);	
	
		iSampleRate = config->iRate;
		iChannels = (config->iStereoMode & ETaskMono)? 1 : 2;
		iInterleaved = (config->iStereoMode & ETaskInterleaved)?ETrue : EFalse;
		return KErrNone;
		}
	
	if (aConfig.Uid() == TUid::Uid(KUidTTaskConfig2))
		{
		const TTaskConfig2* config = TPuTaskConfig2::GetStructure(aConfig);	
	
		iSampleRate = config->iRate;
		iChannels = config->iNumberOfChannels;
		iInterleaved = (config->iStereoMode & ETaskInterleaved)?ETrue : EFalse;
		return KErrNone;
		}	
		
	return KErrNotSupported;
	}

TInt CAudioDevice::CInputPort::MipGetConfig(TPuConfig& /*aConfig*/)
	{
	return KErrNotSupported;
	}

TInt CAudioDevice::CInputPort::MipTunnelRequest(const MMdfOutputPort& aOutputPortToBeConnectedTo,
			TTunnelFlags& /*aTunnelFlags*/, TSupplierType& /*aSupplierType*/)
	{
	if ((iParent.State()!=EProcessingUnitLoaded) && (!iStopped))
		{
		// invalid state
		return EInvalidState;
		}
	
	if (iPortConnectedTo)
		{
		// the port is already connected, return an error
		return EPortAlreadyTunnelled;
		}
	iPortConnectedTo = const_cast<MMdfOutputPort*>(&aOutputPortToBeConnectedTo);
	return KErrNone;
	}

void CAudioDevice::CInputPort::MipWriteData(CMMFBuffer& aInputBuffer)
	{
	TInt err = iBuffers.Append(&aInputBuffer);
	if (err == KErrNone)
		{
		if (iParent.State() == EProcessingUnitExecuting && !IsActive())
			{
			SetActive();
			TRequestStatus* status = &iStatus;
			User::RequestComplete(status, KErrNone);
			}
		}
	else
		{
		if (iObserver)
			{
			iObserver->MipoWriteDataComplete(this, &aInputBuffer, err);
			}
		}
	}

void CAudioDevice::CInputPort::Execute()
	{
	if (!IsActive() && iBuffers.Count()>0)
		{
		SetActive();
		TRequestStatus* status = &iStatus;
		User::RequestComplete(status, KErrNone);
		}
	}

TBool CAudioDevice::CInputPort::MipIsTunnelled() const
	{
	if (iPortConnectedTo)
		{
		return ETrue;
		}
	else
		{
		return EFalse;			
		}
	}

TInt CAudioDevice::CInputPort::MipIndex() const
	{
	return KInputPortIndex;
	}
	
CMMFBuffer* CAudioDevice::CInputPort::MipCreateBuffer(TInt /*aBufferSize*/)
	{
	__ASSERT_ALWAYS(EFalse, User::Panic(KAudioDevicePanic, EInvalidUsage));
	return NULL;
	}

TInt CAudioDevice::CInputPort::MipUseBuffer(CMMFBuffer& /*aBuffer*/)
	{
	return KErrNone;
	}

TInt CAudioDevice::CInputPort::MipFreeBuffer(CMMFBuffer* /*aBuffer*/)
	{
	return KErrNone;
	}
	
void CAudioDevice::CInputPort::MipDisconnectTunnel()
	{
	}

void CAudioDevice::CInputPort::MipRestartTunnel()
	{
	}

TUint32 CAudioDevice::CInputPort::MipBufferSize() const
	{
	return iBufferSize;
	}

void CAudioDevice::CInputPort::Pause()
	{
	if (iParent.SoundDevice().Handle())
		{
		iParent.SoundDevice().PausePlayBuffer();
		}
	}
	
void CAudioDevice::CInputPort::Stop()
	{
	Cancel();	
	}
	
TInt CAudioDevice::CInputPort::MipCreateCustomInterface(TUid aUid)
	{
	if (aUid.iUid == KMmfPlaySettingsCustomInterface)
		{
		return KErrNone;
		}
	return KErrNotSupported;	
	}

TAny* CAudioDevice::CInputPort::MipCustomInterface(TUid aUid)
	{
	if (aUid.iUid == KMmfPlaySettingsCustomInterface)
		{
		return static_cast<MPlayCustomInterface*>(this);
		}
	return NULL;
	}
	
void CAudioDevice::CInputPort::SetVolume(TUint aVolume)
	{
	iVolume = aVolume;
	}

TUint CAudioDevice::CInputPort::Volume()
	{
	return iVolume;
	}

TUint CAudioDevice::CInputPort::BytesPlayed()
	{
	return iBytesPlayed;
	}

void CAudioDevice::CInputPort::SetVolumeRamp(const TTimeIntervalMicroSeconds& aRampDuration)
	{
	iRampDuration = aRampDuration;
	}

TTimeIntervalMicroSeconds& CAudioDevice::CInputPort::VolumeRamp()
	{
	return iRampDuration;
	}

void CAudioDevice::CInputPort::RunL()
	{
	if (iCurrentBuffer != NULL)
		{
		// If we've been signalled with a buffer, callback that we've completed the writing of the
		// buffer
		if (iObserver)
			{		
			iObserver->MipoWriteDataComplete(this, iCurrentBuffer, iStatus.Int());
			if (iCurrentBuffer->LastBuffer())
				{
				iParent.Observer()->ExecuteComplete(&iParent, KErrUnderflow);
				iParent.SoundDevice().Close();
				}
			}
		iCurrentBuffer = NULL;
		}
	// only process the next buffer if there is no error
	// error callbacks were handled in the previous block
	if (iStatus == KErrNone)
		{
		if (iBuffers.Count()>0)
			{
			iCurrentBuffer = iBuffers[0];
			iBuffers.Remove(0);
			
			if (CMMFBuffer::IsSupportedDataBuffer(iCurrentBuffer->Type()))
				{
 				TDes8& aBufferDes = (static_cast<CMMFDataBuffer*>(iCurrentBuffer))->Data();
 				iStatus = KRequestPending;
				iParent.SoundDevice().PlayData(iStatus, aBufferDes);
				SetActive();
				}
			}
		}
	}

void CAudioDevice::CInputPort::DoCancel()
	{
	if (iParent.SoundDevice().Handle())
		{
		iParent.SoundDevice().CancelPlayData();
		iParent.SoundDevice().FlushPlayBuffer();
		}
	}
	
// CAudioDevice::CInput port class implementation
CAudioDevice::COutputPort::COutputPort(CAudioDevice& aParent) 
	: CActive(EPriorityNormal),
	iParent(aParent),
	iSampleRate(KDefaultSampleRate),
	iChannels(KDefaultNumberChannels),
	iBufferSize(KBufferSize)
	{	
	}

TInt CAudioDevice::CInputPort::SampleRate()
	{
	return iSampleRate;
	}
	
TInt CAudioDevice::CInputPort::Channels()
	{
	return iChannels;
	}

void CAudioDevice::CInputPort::MipInitialize()
	{
	// Nothing to do
	}
	
CAudioDevice::COutputPort* CAudioDevice::COutputPort::NewL(CAudioDevice& aParent)
	{
	COutputPort* self = new (ELeave) COutputPort(aParent);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

void CAudioDevice::COutputPort::ConstructL()
	{
	CActiveScheduler::Add(this);	
	}

TInt CAudioDevice::COutputPort::MopConfigure(const TPuConfig& aConfig)
	{
	if (aConfig.Uid() == TUid::Uid(KUidTTaskConfig))
		{
		const TTaskConfig* config = TPuTaskConfig::GetStructure(aConfig);	
	
		iSampleRate = config->iRate;
		iChannels = (config->iStereoMode & ETaskMono)? 1 : 2;
		iInterleaved = (config->iStereoMode & ETaskInterleaved)?ETrue : EFalse;
		return KErrNone;
		}
		
	if (aConfig.Uid() == TUid::Uid(KUidTTaskConfig2))
		{
		const TTaskConfig2* config = TPuTaskConfig2::GetStructure(aConfig);	
	
		iSampleRate = config->iRate;
		iChannels = config->iNumberOfChannels;
		iInterleaved = (config->iStereoMode & ETaskInterleaved)?ETrue : EFalse;
		return KErrNone;
		}	
	return KErrNotSupported;
	}
	
TInt CAudioDevice::COutputPort::MopGetConfig(TPuConfig& /*aConfig*/)
	{
	return KErrNotSupported;
	}

void CAudioDevice::COutputPort::MopInitialize()
	{
	// Nothing to do
	}

TInt CAudioDevice::COutputPort::MopTunnelRequest(const MMdfInputPort& /*aInputPortToBeConnectedTo*/,
	TTunnelFlags& /*aTunnelFlags*/, TSupplierType& /*aSupplierType*/)
	{
	return KErrNone;
	}

void CAudioDevice::COutputPort::MopReadData(CMMFBuffer& aOutputBuffer)
	{
	TInt err = iBuffers.Append(&aOutputBuffer);
	if (err == KErrNone)
		{
		if ((iParent.State() == EProcessingUnitExecuting || iParent.State() == EProcessingUnitPaused) && !IsActive())
			{
			SetActive();
			TRequestStatus* status = &iStatus;
			User::RequestComplete(status, KErrNone);	
			}		
		}
	else
		{
		if (iObserver)
			{
			iObserver->MopoReadDataComplete(this, &aOutputBuffer, err);
			}
		}
	}

TBool CAudioDevice::COutputPort::MopIsTunnelled() const
	{
	return EFalse;	
	}

TInt CAudioDevice::COutputPort::MopIndex() const
	{
	return KOutputPortIndex;
	}
	
CMMFBuffer* CAudioDevice::COutputPort::MopCreateBuffer(TInt /*aBufferSize*/)
	{
	return NULL;
	}
	
TInt CAudioDevice::COutputPort::MopUseBuffer(CMMFBuffer& /*aBuffer*/) 
	{
	return KErrNone;
	}

TInt CAudioDevice::COutputPort::MopFreeBuffer(CMMFBuffer* /*aBuffer*/) 
	{
	return KErrNone;
	}

void CAudioDevice::COutputPort::Execute()
	{
	if (!IsActive() && iBuffers.Count()>0)
		{
		SetActive();
		TRequestStatus* status = &iStatus;
		User::RequestComplete(status, KErrNone);
		}
	}
	
void CAudioDevice::COutputPort::Pause()
	{
	if(iParent.SoundDevice().Handle())
		{
		iParent.SoundDevice().FlushRecordBuffer();
		}	
	}
	
void CAudioDevice::COutputPort::Stop()
	{
	Cancel();
	}

void CAudioDevice::COutputPort::MopDisconnectTunnel()
	{
	// Find the last buffer in the array and set it as the 'LastBuffer'
	if(iBuffers.Count() > 0)
		{
		(iBuffers[iBuffers.Count() - 1])->SetLastBuffer(ETrue);		
		}
	// Still have to send callback and cancel driver
	iObserver->MopoDisconnectTunnelComplete(this, KErrNone);
	iParent.SoundDevice().CancelRecordData();
	}
	
void CAudioDevice::COutputPort::MopRestartTunnel()
	{
	}
	
TUint32 CAudioDevice::COutputPort::MopBufferSize() const
	{
	return iBufferSize;
	}
	
void CAudioDevice::CInputPort::MipSetObserver(const MMdfInputPortObserver& aObserver)
	{
	iObserver = const_cast<MMdfInputPortObserver*>(&aObserver);
	}

void CAudioDevice::COutputPort::MopSetObserver(const MMdfOutputPortObserver& aObserver)
	{
	iObserver = const_cast<MMdfOutputPortObserver*>(&aObserver);
	}

TInt CAudioDevice::COutputPort::MopCreateCustomInterface(TUid aUid)
	{
	if (aUid.iUid == KMmfRecordSettingsCustomInterface)
		{
		return KErrNone;
		}
	return KErrNotSupported;	
	}
	
TAny* CAudioDevice::COutputPort::MopCustomInterface(TUid aUid)
	{
	if (aUid.iUid == KMmfRecordSettingsCustomInterface)
		{
		return dynamic_cast<MRecordCustomInterface*>(this);
		}

	return NULL;
	}
	
void CAudioDevice::COutputPort::RunL()
	{
	if (iCurrentBuffer != NULL)
		{
		// If we've been signalled with a buffer, callback that we've completed the writing of the
		// buffer
		if (iObserver)
			{		
			TInt length = iCurrentBuffer->BufferSize();
			iBytesRecorded += length;
			if (length<iBufferSize)
				{
				iCurrentBuffer->SetLastBuffer(ETrue);
				iParent.SoundDevice().CancelRecordData(); // Required so that a resume of an encode functions correctly
				}
					
			iObserver->MopoReadDataComplete(this, iCurrentBuffer, iStatus.Int());
			
			}
			iCurrentBuffer = NULL;
		}
		
	if (iStatus == KErrNone)
		{
		if (iBuffers.Count()>0)
			{
			iCurrentBuffer = iBuffers[0];
			iBuffers.Remove(0);
			
			if (CMMFBuffer::IsSupportedDataBuffer(iCurrentBuffer->Type()))
				{
				TDes8& aBufferDes = (static_cast<CMMFDataBuffer*>(iCurrentBuffer))->Data();
				iStatus = KRequestPending;
				iParent.SoundDevice().RecordData(iStatus, aBufferDes);
				SetActive();
				}
			}
		}
	}

void CAudioDevice::COutputPort::DoCancel()
	{
	if (iParent.SoundDevice().Handle())
		{
		iParent.SoundDevice().CancelRecordData();
		iParent.SoundDevice().FlushRecordBuffer();
		}		
	}
	
TInt CAudioDevice::COutputPort::SampleRate()
	{
	return iSampleRate;
	}
	
TInt CAudioDevice::COutputPort::Channels()
	{
	return iChannels;
	}

CAudioDevice::COutputPort::~COutputPort()
	{
	Cancel();
	iBuffers.Close();
	}
	
// from MRecordCustomInterface
void CAudioDevice::COutputPort::SetGain(TUint aGain)
	{
	iGain = aGain;
	}
TUint CAudioDevice::COutputPort::Gain()
	{
	return iGain;
	}
	
TUint CAudioDevice::COutputPort::BytesRecorded()
	{
	return iBytesRecorded;
	}

// ------------------------------------------------------------------------------------------
// CAudioDevice Implementation


CAudioDevice::CAudioDevice()
	{
	}
	
CAudioDevice::~CAudioDevice()
	{
	delete iInputPort;
	delete iOutputPort;
	iSoundDevice.Close();
	}

void CAudioDevice::Execute()
	{
	__ASSERT_ALWAYS(iObserver, User::Panic(KAudioDevicePanic, EObserverNotSet));
	TInt err = KErrNone;
	if(!iSoundDevice.Handle())
		{
		err = iSoundDevice.Open();
		}
	
	RMdaDevSound::TCurrentSoundFormatBuf buf;
	if (err == KErrNone)
		{
		if(iState == EProcessingUnitPaused)
			{
			iSoundDevice.ResumePlaying();
			}
		else
			{
			// Set play format (for input port)
			iSoundDevice.GetPlayFormat(buf);
			buf().iRate = iInputPort->SampleRate();
			buf().iChannels = iInputPort->Channels();
			buf().iBufferSize = KBufferSize;
			buf().iEncoding = RMdaDevSound::EMdaSoundEncoding16BitPCM;
			err = iSoundDevice.SetPlayFormat(buf);
			}
		}
	iState = EProcessingUnitExecuting;	
	if (err == KErrNone)
		{
		// Set record format (for output format)
		iSoundDevice.GetRecordFormat(buf);
		buf().iRate = iOutputPort->SampleRate();
		buf().iChannels = iOutputPort->Channels();
		buf().iBufferSize = KBufferSize;
		buf().iEncoding = RMdaDevSound::EMdaSoundEncoding16BitPCM;
		iSoundDevice.SetRecordFormat(buf);
		iInputPort->Execute();
		iOutputPort->Execute();
		}
	else 
		{
		iObserver->ExecuteComplete(this, err);
		}		
	}

TInt CAudioDevice::Pause()
	{
	iState = EProcessingUnitPaused;
	iInputPort->Pause();
	iOutputPort->Pause();
	return KErrNone;
	}

void CAudioDevice::Stop()
	{
	if(iState == EProcessingUnitExecuting || iState == EProcessingUnitPaused)
		{
		// Cancel and flush the device driver		
		iInputPort->Stop();
		iOutputPort->Stop();
		iState = EProcessingUnitIdle;
		
		// Close the sound device
		iSoundDevice.Close();	
		}		
	}
	
CAudioDevice* CAudioDevice::NewL()
	{
	CAudioDevice* self = new (ELeave) CAudioDevice;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}
	
TInt CAudioDevice::Create(const MMdfProcessingUnitObserver& aObserver)
	{
	iObserver = const_cast<MMdfProcessingUnitObserver*>(&aObserver);
	return KErrNone;
	}
	
TInt CAudioDevice::Configure(const TPuConfig& /*aConfigurationSetup*/)
	{
	return KErrNotSupported;
	}
	
TInt CAudioDevice::GetConfig(TPuConfig& /*aConfigurationSetup*/)
	{
	return KErrNotSupported;
	}

TInt CAudioDevice::GetInputPorts(RPointerArray<MMdfInputPort>& aComponentInputPorts )
	{
	return aComponentInputPorts.Append(iInputPort);
	}

TInt CAudioDevice::GetOutputPorts(RPointerArray<MMdfOutputPort>& aComponentOutputPorts )
	{
	return aComponentOutputPorts.Append(iOutputPort);
	}

void CAudioDevice::ConstructL()
	{
	iInputPort = CInputPort::NewL(*this);
	iOutputPort = COutputPort::NewL(*this);
	iState = EProcessingUnitLoaded;
	}

void CAudioDevice::Initialize()
	{
	__ASSERT_ALWAYS(iObserver, User::Panic(KAudioDevicePanic, EObserverNotSet));
	
	iObserver->InitializeComplete(this, KErrNone);
	iState = EProcessingUnitIdle;
	}

MMdfProcessingUnitObserver* CAudioDevice::Observer()
	{
	return iObserver;	
	}
		
TProcessingUnitState CAudioDevice::State()
	{
	return iState;	
	}
		
RMdaDevSound& CAudioDevice::SoundDevice()
	{
	return iSoundDevice;
	}
	
TAny* CAudioDevice::CustomInterface(TUid /*aUid*/)
	{
	return NULL;
	}
		
TInt CAudioDevice::CreateCustomInterface(TUid /*aUid*/)
	{
	return KErrNotSupported;
	}

// ------------------------------------------------------------------------------------------	
// ECOM Implementation table entry	
	
const TImplementationProxy ImplementationTable[] = 
	{
	IMPLEMENTATION_PROXY_ENTRY(KUidPUAudioDevice,	CAudioDevice::NewL),
	};

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
	return ImplementationTable;
	}
    
