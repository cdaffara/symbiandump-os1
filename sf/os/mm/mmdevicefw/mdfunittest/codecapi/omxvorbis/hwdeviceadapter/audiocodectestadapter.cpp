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
// This is a re-implementation of CMdfHwDeviceCodecTestAdapter 
// primarily intended for unit testing codec PUs in PREQ1024.
// It is NOT a subclass, as CMdfHwDeviceCodecTestAdapter itself has no
// virtual or protected methods to override.
// The only difference is that it encodes and decodes from codec
// to codec, unlike CMdfHwDeviceCodecTestAdapter which encodes from sounddev
// (mic) to codec and decodes from codec to sounddev (speaker)
// 
//

/**
 @file
 @internalComponent
*/

#include "audiocodectestadapter.h"
#include <mdf/codecapiuids.hrh>
#include <mdf/mdfpuconfig.h>
// for the bitrate custom interface
#include <mmf/server/devsoundstandardcustominterfaces.h>

#include <e32debug.h>

// #define AUDIOCODECTESTADAPTER_DEBUG	1
#if defined(AUDIOCODECTESTADAPTER_DEBUG)
#define DEBUG_PRINT RDebug::Print
#else
#define DEBUG_PRINT
#endif

// Interface UID for the Processing Unit Loader
const TUid KUidPuLoader = {KUidPuLoaderImplementation};


CMdfHwDeviceCodecTestAdapter::~CMdfHwDeviceCodecTestAdapter()
	{
	Stop();
	// Unload the PUs
	if (iCodecPU)
		{
		iPuLoader->UnloadProcessingUnit(iCodecPU);
		}
		
	// The I/O ports should have been deleted at this point

	delete iInputBuffer;
	delete iOutputBuffer;		
	delete iActiveWait;
	delete iPuLoader;	
	REComSession::DestroyedImplementation(iPuLoaderDtorKey);
	}

CMdfHwDeviceCodecTestAdapter* CMdfHwDeviceCodecTestAdapter::NewL()
	{
	CMdfHwDeviceCodecTestAdapter* self = new (ELeave) CMdfHwDeviceCodecTestAdapter;
	CleanupStack::PushL (self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}
	
CMdfHwDeviceCodecTestAdapter::CMdfHwDeviceCodecTestAdapter()
	{	
	}
	
void CMdfHwDeviceCodecTestAdapter::ConstructL()
	{		
	// Load the PU Loader plugin
	iPuLoader = static_cast<CMdfPuLoader*>
		(REComSession::CreateImplementationL(KUidPuLoader, iPuLoaderDtorKey));
	iActiveWait = new (ELeave) CActiveSchedulerWait;
	iState = EProcessingUnitLoaderLoaded;
	}	
	
TInt CMdfHwDeviceCodecTestAdapter::Start(TDeviceFunc aFuncCmd, TDeviceFlow /*aFlowCmd*/)
	{		
	if (!((aFuncCmd == EDevEncode)|(aFuncCmd == EDevDecode)|(aFuncCmd == EDevNullFunc)))
		{
		return KErrArgument;	
		}
		
	iFuncCmd = aFuncCmd;
	
	// NB: aFlowCmd not used - this is codec-codec processing by default.

	TInt err = KErrNone;
	switch(aFuncCmd)
		{
		case EDevEncode:
			{
			err = StartEncode();
			}
			break;
		case EDevDecode:
			{
			err = StartDecode();
			}
			break;
		case EDevNullFunc:
		default:
			{
			err = KErrNotSupported;	
			}		
			break;
		}

	
	return err;
	}
	
// encode and decode logic is identical for codec-codec processing
	
TInt CMdfHwDeviceCodecTestAdapter::InitializeEncodeDecode()
	{
	DEBUG_PRINT(_L("CMdfHwDeviceCodecTestAdapter::InitializeEncodeDecode"));
	
	ASSERT(iOutputPort);
	
	iInputPortBufferSize = iInputPort->MipBufferSize();	
	TRAPD(err, iInputBuffer = CMMFDescriptorBuffer::NewL(iInputPortBufferSize));
	if(err != KErrNone)
		{
		return err;
		}
	iInputBuffer->SetLastBuffer(EFalse);
	iInputPort->MipUseBuffer(*iInputBuffer);	
	
	iOutputPortBufferSize = iOutputPort->MopBufferSize();			
	TRAP(err, iOutputBuffer = CMMFDescriptorBuffer::NewL(iOutputPortBufferSize));
	if(err != KErrNone)
		{
		return err;
		}	
	iOutputBuffer->SetLastBuffer(EFalse);
	iOutputPort->MopUseBuffer(*iOutputBuffer);		

	// VD: should not move the set up of the state after sending the Initialize() calls???
	iState = EProcessingUnitInitializing;
		
	iCodecPU->Initialize();
	
	iActiveWait->Start();
	return KErrNone;
	}

TInt CMdfHwDeviceCodecTestAdapter::StartEncode()
	{
	DEBUG_PRINT(_L("CMdfHwDeviceCodecTestAdapter::StartEncode"));
	TInt err = KErrNone;
	if (iState == EProcessingUnitLoaded)
		{
		err = InitializeEncodeDecode();
		}
	if (err != KErrNone)
		{
		return err;
		}
			
	return StartExecuting();	
	}
	
TInt CMdfHwDeviceCodecTestAdapter::StartDecode()
	{
	DEBUG_PRINT(_L("CMdfHwDeviceCodecTestAdapter::StartDecode"));
	
	TInt err = KErrNone;
	if (iState == EProcessingUnitLoaded)
		{
		err = InitializeEncodeDecode();
		}
	if (err != KErrNone)
		{
		return err;
		}
			
	return StartExecuting();	
	}

TInt CMdfHwDeviceCodecTestAdapter::StartExecuting()
	{
	DEBUG_PRINT(_L("CMdfHwDeviceCodecTestAdapter::StartExecuting"));
	TInt err = KErrNone;

	iOutputPort->MopReadData(*iOutputBuffer);	
	err = iHwDeviceObserver->FillThisHwBuffer(*iInputBuffer);
	if(err != KErrNone)
		{
		return err;
		}
		
	iState = EProcessingUnitExecuting;
	iCodecPU->Execute();
	
	return KErrNone;
	}
	
TInt CMdfHwDeviceCodecTestAdapter::Stop()
	{	
	if(iState == EProcessingUnitExecuting || iState == EProcessingUnitPaused)
		{			
		iStopping = ETrue; // is used as a guard in ExecuteComplete
				
		if(iCodecPU)
			{
			iCodecPU->Stop();	
			}
				
		iPCMPUCallbackComplete = EFalse;		
				
		iState = EProcessingUnitIdle;
		iStopping = EFalse;	
		}
	return KErrNone;			
	}

TInt CMdfHwDeviceCodecTestAdapter::Pause()
	{
	return iCodecPU->Pause();
	}
	
TInt CMdfHwDeviceCodecTestAdapter::Init(THwDeviceInitParams& aDevInfo)
	{
	if(!iCodecPU)
		{
		return KErrNotSupported;
		}

	// Not currently using any other members of aDevInfo, except the Observer
	if(!aDevInfo.iHwDeviceObserver)
		{
		return KErrArgument;
		}
	iHwDeviceObserver = aDevInfo.iHwDeviceObserver;
	
	// Get ports and set observers
	RPointerArray<MMdfInputPort> inputPorts;
	TInt err = iCodecPU->GetInputPorts(inputPorts);
	if(err != KErrNone)
		{
		return err;
		}	
		
	if (inputPorts.Count()<1)
		{
		return KErrNotFound;
		}
		
	iInputPort = inputPorts[0];
	inputPorts.Close();
	
	iInputPort->MipSetObserver(*this);

	RPointerArray<MMdfOutputPort> outputPorts;
	err = iCodecPU->GetOutputPorts(outputPorts);
	if(err != KErrNone)
		{
		return err;
		}	
		
	if (outputPorts.Count()<1)
		{
		return KErrNotFound;
		}
		
	iOutputPort = outputPorts[0];
	outputPorts.Close();
	iOutputPort->MopSetObserver(*this);
	
	iState = EProcessingUnitLoaded;
		
	return KErrNone;		
	}

TAny* CMdfHwDeviceCodecTestAdapter::CustomInterface(TUid aInterfaceId)
	{
	if (aInterfaceId == KUidHwDeviceSetupInterface)
		{
		return static_cast<MMdfHwDeviceSetup*>(this);
		}
	else if (aInterfaceId.iUid == KMmfPlaySettingsCustomInterface)
		{
		return reinterpret_cast<MPlayCustomInterface*>(iInputPort);
		}
	else if (aInterfaceId.iUid == KMmfRecordSettingsCustomInterface)
		{
		return reinterpret_cast<MRecordCustomInterface*>(iOutputPort);
		}
	// if the PU is an encoder it may have a BitRate custom interface	
	else if (aInterfaceId == KUidCustomInterfaceDevSoundBitRate)
		{
		return static_cast<MMMFDevSoundCustomInterfaceBitRate*>(iCodecPU->CustomInterface(aInterfaceId));
		}
	else	
		{
		return NULL;
		}
	}

TInt CMdfHwDeviceCodecTestAdapter::ThisHwBufferFilled(CMMFBuffer& aFillBufferPtr)
	{		
	DEBUG_PRINT(_L("CMdfHwDeviceCodecTestAdapter::ThisHwBufferFilled"));
	
	aFillBufferPtr.SetStatus(EFull); 
	
	// if the buffer is empty or the last buffer, write it anyway -
	// the stop / error will be generated elsewhere

	iInputPort->MipWriteData(aFillBufferPtr);
	return KErrNone;
	}

TInt CMdfHwDeviceCodecTestAdapter::ThisHwBufferEmptied(CMMFBuffer& /*aEmptyBufferPtr*/)
	{
	DEBUG_PRINT(_L("CMdfHwDeviceCodecTestAdapter::ThisHwBufferEmptied"));
	
	iOutputPort->MopReadData(*iOutputBuffer);
	return KErrNone;
	}
	
TInt CMdfHwDeviceCodecTestAdapter::SetConfig(TTaskConfig& aConfig)
	{
	TInt err = KErrNone;
	// Call to Configure the Codec PU
	TPuTaskConfig config(aConfig);
	err = iInputPort->MipConfigure(config);
	if(err != KErrNone)
		{
		return err;
		}
	err = iOutputPort->MopConfigure(config);
	if(err != KErrNone)
		{
		return err;
		}

//	iState = EProcessingUnitConfigured;
	
	return KErrNone;
	}

TInt CMdfHwDeviceCodecTestAdapter::StopAndDeleteCodec()
	{
	return KErrNone;
	}

TInt CMdfHwDeviceCodecTestAdapter::DeleteCodec()
	{
	return KErrNone;
	}

void CMdfHwDeviceCodecTestAdapter::MipoWriteDataComplete(const MMdfInputPort* aInputPort,
	CMMFBuffer* aBuffer, TInt aErrorCode)
	{
	DEBUG_PRINT(_L("CMdfHwDeviceCodecTestAdapter::MipoWriteDataComplete"));
	
	if (aErrorCode == KErrNone && aInputPort == iInputPort)
		{
		if(aBuffer->LastBuffer())
			{
			if(iFuncCmd == EDevEncode) 
				{
				// we must cancel the PU here if it's an encoder - the decoder
				// will be done elsewhere
				iCodecPU->Stop();
				}
			}
		else
			{
			iHwDeviceObserver->FillThisHwBuffer(*aBuffer);
			}
		}
	else
		{
		StopHwDevice(aErrorCode);
		}		
	}
		
void CMdfHwDeviceCodecTestAdapter::MipoDisconnectTunnelComplete(const MMdfInputPort* aInputPort, 
	TInt aErrorCode)
	{
	// The Inputport of the PcmCodecPu will no longer receive data.
	// Set flag to indicate that the sink outputport has been stopped?
	if(aErrorCode == KErrNone)
		{
		if(aInputPort == iInputPort)
			{
			iPCMPuMipoStopCompleted = ETrue;
			}
		}
	else
		{
		iHwDeviceObserver->Error(aErrorCode);
		}	
	}

void CMdfHwDeviceCodecTestAdapter::MipoRestartTunnelComplete(const MMdfInputPort* /*aInputPort*/,
	TInt /*aErrorCode*/)
	{
	
	}

void CMdfHwDeviceCodecTestAdapter::MopoReadDataComplete(const MMdfOutputPort* aOutputPort, 
	CMMFBuffer* aBuffer, TInt aErrorCode)
	{
	DEBUG_PRINT(_L("CMdfHwDeviceCodecTestAdapter::MopoReadDataComplete"));
	
	if(aErrorCode == KErrNone && aOutputPort == iOutputPort)
		{
		iHwDeviceObserver->EmptyThisHwBuffer(*aBuffer);
		}
	else
		{
		StopHwDevice(aErrorCode);
		}
	}
				
void CMdfHwDeviceCodecTestAdapter::MopoDisconnectTunnelComplete(const MMdfOutputPort* aOutputPort,
	TInt aErrorCode)
	{
	if(aErrorCode == KErrNone)
		{
		if(aOutputPort == iOutputPort)
			{
			iPCMPuMopoStopCompleted = ETrue;			
			}
		}
	else
		{
		iHwDeviceObserver->Error(aErrorCode);
		}
	if(iPCMPuMipoStopCompleted && iPCMPuMopoStopCompleted)
		{
		iState = EProcessingUnitIdle;
		}		
	}
	
void CMdfHwDeviceCodecTestAdapter::MopoRestartTunnelComplete(const MMdfOutputPort* /* aOutputPort */,
	TInt /*aErrorCode*/)
	{
	
	}

void CMdfHwDeviceCodecTestAdapter::InitializeComplete(const CMdfProcessingUnit* aPu, TInt aErrorCode)
	{
	if(aErrorCode != KErrNone)
		{
		iHwDeviceObserver->Error(aErrorCode);
		return;
		}
			
	if(aPu == iCodecPU)
		{
		iPCMPUCallbackComplete = ETrue;
		}
	
	if(iPCMPUCallbackComplete)
		{

		// reset the flags
		iPCMPUCallbackComplete = EFalse;
		
		// PUs initialised OK
		iActiveWait->AsyncStop();

		}
	}

void CMdfHwDeviceCodecTestAdapter::ExecuteComplete(const CMdfProcessingUnit* aPu, TInt aErrorCode)
	{
	if(iStopping)
		{
		return;
		}
	
	if (iExecuteError == KErrNone)
		{
		iExecuteError = aErrorCode;
		}
	 		
	if(aPu == iCodecPU)
		{
		iPCMPUCallbackComplete = ETrue;
		}
	
	if(iExecuteError != KErrNone || (iPCMPUCallbackComplete) )
		{
		if (iState == EProcessingUnitExecuting)
			{
			// stop the hardware device if we are still executing
			StopHwDevice(iExecuteError);		
			iState = EProcessingUnitIdle;
			}
		// reset the flags
		iPCMPUCallbackComplete = EFalse;
		}
	}

void CMdfHwDeviceCodecTestAdapter::SetDataTypesL(TFourCC aSrcType, TFourCC aDestType)
	{
	// Find and load an appropriate Codec
	iCodecPU = iPuLoader->LoadProcessingUnitL(*this, aSrcType, aDestType);
	}


void CMdfHwDeviceCodecTestAdapter::StopHwDevice(TInt error)
	{
	iHwDeviceObserver->Stopped();
	iHwDeviceObserver->Error(error);		
	}

void CMdfHwDeviceCodecTestAdapter::GetState(THwDevAdapterState& aState) const
	{
	aState = iState;	
	}
