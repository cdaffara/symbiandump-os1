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

#include "HwDeviceAdapter/mdfhwdeviceadapter.h"
#include <mdf/codecapiuids.hrh>
#include <mdf/mdfpuconfig.h>
#include <mmf/server/devsoundstandardcustominterfaces.h>

// Interface UID for the Sink Processing Unit
const TUid KUidSourceSinkPu = {0x102730BB};
const TInt KZerothPort = 0;

/*
The destructor.  Unloads the Processing Units, deletes the 
Processing Unit Loader and frees any owned buffers.
@see CMMFHwDevice::~CMMFHwDevice()
*/
CMdfHwDeviceAdapter::~CMdfHwDeviceAdapter()
	{
	Stop();
	// Unload the PUs
	if (iCodecPU)
		{
		iPuLoader->UnloadProcessingUnit(iCodecPU);
		}
	if (iAudioDevicePU)
		{
		iPuLoader->UnloadProcessingUnit(iAudioDevicePU);
		}
	
	delete iInputBuffer;
	delete iOutputBuffer;		
	delete iActiveWait;
	delete iPuLoader;	
	REComSession::DestroyedImplementation(iPuLoaderDtorKey);
	}

/*
Creates a new CMdfHwDeviceAdapter object.  The Processing Unit Loader plugin
is also loaded, and so the CMdfHwDeviceAdapter state is set to EProcessingUnitLoaderLoaded.
Will leave with KErrNotFound if it, or the Processing Unit Loader plugin is not found.
@see THwDevAdapterState
*/
CMdfHwDeviceAdapter* CMdfHwDeviceAdapter::NewL()
	{
	CMdfHwDeviceAdapter* self = new (ELeave) CMdfHwDeviceAdapter;
	CleanupStack::PushL (self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

/*
Default constructor.
*/	
CMdfHwDeviceAdapter::CMdfHwDeviceAdapter()
	{	
	}

/*
Loads the Processing Unit Loader plugin.
*/	
void CMdfHwDeviceAdapter::ConstructL()
	{		
	// Load the PU Loader plugin
	iPuLoader = static_cast<CMdfPuLoader*>
		(REComSession::CreateImplementationL(TUid::Uid(KUidPuLoaderImplementation), iPuLoaderDtorKey));
	iActiveWait = new (ELeave) CActiveSchedulerWait;
	iState = EProcessingUnitLoaderLoaded;
	}	

/*
@see CMMFHwDevice::Start()
@see TDeviceFunc
*/
TInt CMdfHwDeviceAdapter::Start(TDeviceFunc aFuncCmd, TDeviceFlow /*aFlowCmd*/)
	{		
	if (!((aFuncCmd == EDevEncode)|(aFuncCmd == EDevDecode)|(aFuncCmd == EDevNullFunc)))
		{
		return KErrArgument;	
		}
		
	iFuncCmd = aFuncCmd;
			
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
			{
			// nothing at the moment, so fall through
			}
			//break;
		default:
			{
			err = KErrNotSupported;	
			}		
			break;
		}

	
	return err;
	}
	
/*
Initialises the encode operation, and set the state of the CMdfHwDeviceAdapter
to EProcessingUnitInitializing.
@return  An error code indicating if the function call was successful.
KErrNone on success, otherwise another of the system-wide error codes.
*/	
TInt CMdfHwDeviceAdapter::InitializeEncode()
	{
	ASSERT(iCodecOutputPort && iSourceOutputPort);
	// Create input buffer that passes data from Source -> Codec PU	
	TRAPD(err, iInputBuffer = CMMFDescriptorBuffer::NewL(iSourceOutputPort->MopBufferSize()));
	if(err != KErrNone)
		{
		return err;
		}
	
	// Create output buffer that passes decoded data from Codec PU -> Out to file	
	TRAP(err, iOutputBuffer = CMMFDescriptorBuffer::NewL(iCodecOutputPort->MopBufferSize()));
	if(err != KErrNone)
		{
		return err;
		}
	
	err = iSourceOutputPort->MopUseBuffer(*iInputBuffer);
	if(err != KErrNone)
		{
		return err;
		}		
	err = iCodecInputPort->MipUseBuffer(*iInputBuffer);
	if(err != KErrNone)
		{
		return err;
		}		
	err = iCodecOutputPort->MopUseBuffer(*iOutputBuffer);
	if(err != KErrNone)
		{
		return err;
		}
			
	// async calls, that callback to InitializeComplete()
	iCodecPU->Initialize();
	iAudioDevicePU->Initialize();
	if (iState == EProcessingUnitLoaded)
		{
		// only wait for the callbacks if we haven't already received them
		iState = EProcessingUnitInitializing;
		iActiveWait->Start();
		}
	return iInitError;
	}
	
/*
Starts the encode operation, providing that the intial state of the
CMdfHwDeviceAdapter is EProcessingUnitLoaded.
@return  An error code indicating if the function call was successful.
KErrNone on success, otherwise another of the system-wide error codes.
*/
TInt CMdfHwDeviceAdapter::StartEncode()
	{
	TInt err = KErrNone;
	if (iState == EProcessingUnitLoaded)
		{
		err = InitializeEncode();
		}
	if (err != KErrNone)
		{
		return err;
		}	
	if (iState == EProcessingUnitPaused)
		{
		// Ensure that the LastBuffer flags are reset.
		iInputBuffer->SetLastBuffer(EFalse);
		iOutputBuffer->SetLastBuffer(EFalse);
		}
					
	return StartExecuting();
	}

/*
Starts the execution of the encode or decode operation, and sets the state
of the CMdfHwDeviceAdapter to EProcessingUnitExecuting.
@return  An error code indicating if the function call was successful.
KErrNone on success, otherwise another of the system-wide error codes.
*/	
TInt CMdfHwDeviceAdapter::StartExecuting()
	{
	TInt err = KErrNone;
	if(iFuncCmd == EDevDecode)
		{
		iCodecOutputPort->MopReadData(*iOutputBuffer);		
		err = iHwDeviceObserver->FillThisHwBuffer(*iInputBuffer);
		if(err != KErrNone)
			{
			return err;
			}
		}
	else // encode
		{
		iSourceOutputPort->MopReadData(*iInputBuffer);
		iCodecOutputPort->MopReadData(*iOutputBuffer);	
		}		
		
	iState = EProcessingUnitExecuting;
		
	iCodecPU->Execute();
	iAudioDevicePU->Execute();
	
	return err;
	}
	
/*
Initialises the decode operation, and set the state of the CMdfHwDeviceAdapter
to EProcessingUnitInitializing.
@return  An error code indicating if the function call was successful.
KErrNone on success, otherwise another of the system-wide error codes.
*/	
TInt CMdfHwDeviceAdapter::InitializeDecode()
	{
	ASSERT(iCodecOutputPort && iSinkInputPort);
	
	TRAPD(err, iInputBuffer = CMMFDescriptorBuffer::NewL(iCodecInputPort->MipBufferSize()));
	if(err != KErrNone)
		{
		return err;
		}
		
	iCodecInputPort->MipUseBuffer(*iInputBuffer);	
	TRAP(err, iOutputBuffer = CMMFDescriptorBuffer::NewL(iCodecOutputPort->MopBufferSize()));
	if(err != KErrNone)
		{
		return err;
		}	

	// Try to set up a tunnelbetween the output port of the PCM Pu 
	// and the input port of the Sink Pu	
	if (iPuLoader->TunnelSetup(*iCodecOutputPort, *iSinkInputPort) != KErrNone)
		{
		iSinkInputPort->MipUseBuffer(*iOutputBuffer);
		}
	iCodecOutputPort->MopUseBuffer(*iOutputBuffer);		
	
	// async calls, that callback to InitializeComplete()
	iCodecPU->Initialize();
	iAudioDevicePU->Initialize();
	iState = EProcessingUnitInitializing;
	iActiveWait->Start();
	return KErrNone;
	}

/*
Starts the decode operation, providing that the intial state of the
CMdfHwDeviceAdapter is EProcessingUnitLoaded.
@return  An error code indicating if the function call was successful.
KErrNone on success, otherwise another of the system-wide error codes.
*/
TInt CMdfHwDeviceAdapter::StartDecode()
	{	
	TInt err = KErrNone;
	if (iState == EProcessingUnitLoaded)
		{
		err = InitializeDecode();
		}
	if (err != KErrNone)
		{
		return err;
		}
	if (iState == EProcessingUnitIdle)
		{
		// Ensure that the LastBuffer flags are reset.
		iInputBuffer->SetLastBuffer(EFalse);
		iOutputBuffer->SetLastBuffer(EFalse);
		}
	return StartExecuting();	
	}

/*
@see CMMFHwDevice::Stop()
*/
TInt CMdfHwDeviceAdapter::Stop()
	{	
	if(iState == EProcessingUnitExecuting || iState == EProcessingUnitPaused)
		{			
		iStopping = ETrue; // is used as a guard in ExecuteComplete
		if(iAudioDevicePU)
			{
			iAudioDevicePU->Stop();	
			}
		if(iCodecPU)
			{
			iCodecPU->Stop();	
			}
				
		iPCMPUCallbackComplete = EFalse;
		iSourceSinkPUCallbackComplete = EFalse;
				
		iState = EProcessingUnitIdle;
		iStopping = EFalse;	
		}
	return KErrNone;		
	}

/*
@see CMMFHwDevice::Pause()
*/
TInt CMdfHwDeviceAdapter::Pause()
	{
	TInt err = KErrNone;
	if(iState != EProcessingUnitPaused)
		{
		if(iAudioDevicePU)
			{	
			err = iAudioDevicePU->Pause();	
			}
		iState = EProcessingUnitPaused;
		}	
	return err;	
	}

/*
@see CMMFHwDevice::Init()
*/
TInt CMdfHwDeviceAdapter::Init(THwDeviceInitParams& aDevInfo)
	{	
	if(!iCodecPU)
		{
		return KErrNotSupported;
		}
		
	// Not currently using any other members of aDevInfo, except the Observer
	// Set observer	
	if(!aDevInfo.iHwDeviceObserver)
		{
		return KErrArgument;
		}
	iHwDeviceObserver = aDevInfo.iHwDeviceObserver;
	
	RPointerArray<MMdfInputPort> inputPorts;
	// Get ports and set observers
	TInt err = iCodecPU->GetInputPorts(inputPorts);		
	if (err == KErrNone && inputPorts.Count() > 0)
		{
		iCodecInputPort = inputPorts[KZerothPort];
		iCodecInputPort->MipSetObserver(*this);
		}		
	inputPorts.Close();	
	if (err != KErrNone)
		{
		return err;
		}		
	if (!iCodecInputPort)
		{
		return KErrNotFound;
		}

	RPointerArray<MMdfOutputPort> outputPorts;
	// Get ports and set observers
	err = iCodecPU->GetOutputPorts(outputPorts);	
	if (err == KErrNone && outputPorts.Count() > 0)
		{
		iCodecOutputPort = outputPorts[KZerothPort];
		iCodecOutputPort->MopSetObserver(*this);
		}		
	outputPorts.Close();	
	if (err != KErrNone)
		{
		return err;
		}
	if (!iCodecOutputPort)
		{
		return KErrNotFound;
		}
	
	// Load SourceSink Pu	
	TRAP(err, iAudioDevicePU = iPuLoader->LoadProcessingUnitL(*this, KUidSourceSinkPu));
	if(err!=KErrNone)
		{
		return err;
		}		

	err = iAudioDevicePU->GetInputPorts(inputPorts);	
	if (err == KErrNone && inputPorts.Count() > 0)
		{
		iSinkInputPort = inputPorts[KZerothPort];
		iSinkInputPort->MipSetObserver(*this);
		}		
	inputPorts.Close();
	if (err != KErrNone)
		{
		return err;
		}
	if (!iSinkInputPort)
		{
		return KErrNotFound;
		}

	
	err = iAudioDevicePU->GetOutputPorts(outputPorts);	
	if (err == KErrNone && outputPorts.Count() > 0)
		{
		iSourceOutputPort = outputPorts[KZerothPort];
		iSourceOutputPort->MopSetObserver(*this);
		}		
	outputPorts.Close();
	if (err != KErrNone)
		{
		return err;
		}
	if (!iCodecOutputPort)
		{
		return KErrNotFound;
		}
	
	iState = EProcessingUnitLoaded;
	return KErrNone;		
	}

/*
@see CMMFHwDevice::CustomInterface()
*/
TAny* CMdfHwDeviceAdapter::CustomInterface(TUid aInterfaceId)
	{
	if (aInterfaceId == KUidHwDeviceSetupInterface)
		{
		MMdfHwDeviceSetup* hwDeviceSetup = this;
		return hwDeviceSetup;
		}
	else if (aInterfaceId.iUid == KMmfPlaySettingsCustomInterface)
		{
		return iSinkInputPort->MipCustomInterface(aInterfaceId);
		}
	else if (aInterfaceId.iUid == KMmfRecordSettingsCustomInterface)
		{
		return iSourceOutputPort->MopCustomInterface(aInterfaceId);
		}
	else if (aInterfaceId == KUidCustomInterfaceDevSoundBitRate)
		{
		return iCodecPU->CustomInterface(aInterfaceId);
		}
	else		
		{
		return NULL;
		}
	}

/*
@see CMMFHwDevice::ThisHwBufferFilled()
*/
TInt CMdfHwDeviceAdapter::ThisHwBufferFilled(CMMFBuffer& aFillBufferPtr)
	{
	aFillBufferPtr.SetStatus(EFull);	
	iCodecInputPort->MipWriteData(aFillBufferPtr);
	return KErrNone;
	}

/*
@see CMMFHwDevice::ThisHwBufferEmptied()
*/
TInt CMdfHwDeviceAdapter::ThisHwBufferEmptied(CMMFBuffer& /*aEmptyBufferPtr*/)
	{
	if (iOutputBuffer->LastBuffer())
		{
		CMMFDataBuffer* buffer = static_cast <CMMFDataBuffer*> (iOutputBuffer);
		buffer->Data().SetLength(0);
		iHwDeviceObserver->EmptyThisHwBuffer(*iOutputBuffer);
		}
	else	
		{
		iCodecOutputPort->MopReadData(*iOutputBuffer);
		}
	return KErrNone;
	}

/*
@see CMMFHwDevice::SetConfig()
*/
TInt CMdfHwDeviceAdapter::SetConfig(TTaskConfig& aConfig)
	{
	TInt err = KErrNone;
	// Call to Configure the Codec PU
	TPuTaskConfig config(aConfig);
	err = iCodecInputPort->MipConfigure(config);
	if(err != KErrNone)
		{
		return err;
		}
	err = iCodecOutputPort->MopConfigure(config);
	if(err != KErrNone)
		{
		return err;
		}

	// configure the audio device
	err = iSinkInputPort->MipConfigure(config);
	if(err != KErrNone)
		{
		return err;
		}
	err = iSourceOutputPort->MopConfigure(config);
	if(err != KErrNone)
		{
		return err;
		}

	return KErrNone;
	}

/*
@see CMMFHwDevice::StopAndDeleteCodec()
*/
TInt CMdfHwDeviceAdapter::StopAndDeleteCodec()
	{	
	TInt stopError = Stop();
	TInt deleteError = DeleteCodec();

	if (stopError != KErrNone)
		{
		return stopError;
		}		
	else
		{
		return deleteError;
		}		
	}

/*
Unloads all Processing Units, and deletes any locally owned buffers.
State is set to EProcessingUnitLoaderLoaded.

@see CMMFHwDevice::DeleteCodec()
*/
TInt CMdfHwDeviceAdapter::DeleteCodec()
	{
	Stop();	
	if (iCodecPU)
		{
		iPuLoader->UnloadProcessingUnit(iCodecPU);		
		}		
	if (iAudioDevicePU)	
		{
		iPuLoader->UnloadProcessingUnit(iAudioDevicePU);		
		}	
	
	// CMdfHwDeviceAdapter does not own the I/O ports, which 
	// have been deleted at this point by UnloadProcessingUnit()
	iCodecInputPort = NULL;
	iCodecOutputPort = NULL;
	iSinkInputPort = NULL;
	iSourceOutputPort = NULL;	
		
	delete iInputBuffer;
	iInputBuffer = NULL;
	delete iOutputBuffer;
	iOutputBuffer = NULL;
	
	iState = EProcessingUnitLoaderLoaded;
	
	return KErrNone;
	}

/*
@see MMdfInputPortObserver::MipoWriteDataComplete()
*/
void CMdfHwDeviceAdapter::MipoWriteDataComplete(const MMdfInputPort* aInputPort,
	CMMFBuffer* aBuffer, TInt aErrorCode)
	{
	if(aErrorCode != KErrNone)
		{
		StopHwDevice(aErrorCode);
		}
	else
		{
		switch(iFuncCmd)
			{
			case EDevEncode:
				{				
				if(aInputPort == iCodecInputPort)
					{
					if(aBuffer->LastBuffer())
						{
						iCodecPU->Stop();
						}
					else
						{
						// if not the last buffer, then pass buffer back to source to get more data				
						iSourceOutputPort->MopReadData(*iInputBuffer);
						}
					}
				}
				break;
			case EDevDecode:
				{
				if(aInputPort == iCodecInputPort)
					{				
					if (!aBuffer->LastBuffer())
						{	
						// JW 22-05-06
						// if the PU is idle (having been Stopped) then
						// FillThisHwBuffer will cause a kern-exec,
						// as the DevSoundSession no longer has a buffer
						if(iState == EProcessingUnitExecuting) 
							{
							TInt err = iHwDeviceObserver->FillThisHwBuffer(*aBuffer);
							if(err != KErrNone)
								{
								StopHwDevice(err);
								}						
							}
						}
					}
				else // aInputPort == iSinkInputPort
					{
					if (!aBuffer->LastBuffer())
						{				
						if(iSinkInputPort->MipIsTunnelled())
							{
							// This callback shouldn't occur since it should be tunnelled with the sink input port
							StopHwDevice(KErrArgument);
							}
						else
							{
							iCodecOutputPort->MopReadData(*iOutputBuffer);
							}
						}
					}
				}
				break;
			case EDevNullFunc:
				// nothing at the moment, so fall through
			default:
				StopHwDevice(KErrNotSupported);	
			}
		}
	}

/*
@see MMdfInputPortObserver::MipoDisconnectTunnelComplete()
*/	
void CMdfHwDeviceAdapter::MipoDisconnectTunnelComplete(const MMdfInputPort* aInputPort, 
	TInt aErrorCode)
	{
	// The Inputport of the PcmCodecPu will no longer receive data.	
	if(aErrorCode == KErrNone)
		{
		if(aInputPort == iCodecInputPort)
			{
			iPCMPuMipoStopCompleted = ETrue;
			}
		else if(aInputPort == iSinkInputPort)
			{
			// This shouldn't be called!
			iHwDeviceObserver->Error(KErrNotFound);		
			}
		}
	else
		{
		iHwDeviceObserver->Error(aErrorCode);
		}	
	}

/*
@see MMdfInputPortObserver::MipoRestartTunnelComplete()
*/
void CMdfHwDeviceAdapter::MipoRestartTunnelComplete(const MMdfInputPort* /*aInputPort*/,
	TInt /*aErrorCode*/)
	{
	
	}
	
/*
@see MMdfOutputPortObserver::MopoReadDataComplete()
*/
void CMdfHwDeviceAdapter::MopoReadDataComplete(const MMdfOutputPort* aOutputPort, 
	CMMFBuffer* aBuffer, TInt aErrorCode)
	{
	if(aErrorCode != KErrNone)
		{
		StopHwDevice(aErrorCode);
		}
	else
		{
		switch(iFuncCmd)
			{
			case EDevEncode:
				{
				if(aOutputPort == iSourceOutputPort)
					{
					iCodecInputPort->MipWriteData(*aBuffer);
					}
				else // aPu == iCodecPU
					{
					TInt err = iHwDeviceObserver->EmptyThisHwBuffer(*aBuffer);
					if(err !=KErrNone)
						{
						StopHwDevice(err);
						}
					}
				}
				break;
			case EDevDecode:
				{
				if(aOutputPort == iCodecOutputPort)
					{
					if(iCodecOutputPort->MopIsTunnelled())
						{
						// This callback shouldn't occur since it should be tunnelled with the sink input port
						StopHwDevice(KErrArgument);
						}
					else
						{
						iSinkInputPort->MipWriteData(*aBuffer);
						}
					}
				}
				break;
			case EDevNullFunc:
				// nothing at the moment, so fall through
			default:
				StopHwDevice(KErrNotSupported);
			}		
		}
	}

/*
@see MMdfOutputPortObserver::MopoDisconnectTunnelComplete()
*/	
void CMdfHwDeviceAdapter::MopoDisconnectTunnelComplete(const MMdfOutputPort* aOutputPort,
	TInt aErrorCode)
	{
	if(!aOutputPort)
		{
		iHwDeviceObserver->Error(KErrArgument);
		return;
		}		
		
	// The last buffer has been set, and called back to MopoReadDataComplete
	if(aErrorCode != KErrNone)
		{
		iHwDeviceObserver->Error(aErrorCode);
		return;
		}	

	if(aOutputPort == iCodecOutputPort)
		{		
		iPCMPuMopoStopCompleted = ETrue;	
		}
	else if(aOutputPort == iSourceOutputPort)
		{
		iSourceSinkPuMopoStopCompleted = ETrue;
		}
	else
		{
		iHwDeviceObserver->Error(KErrArgument);
		return;
		}
			
	if(iPCMPuMipoStopCompleted && iPCMPuMopoStopCompleted && iSourceSinkPuMopoStopCompleted)
		{
		iHwDeviceObserver->Error(KErrNone);
		iState = EProcessingUnitIdle;
		}		
	}

/*
@see MMdfOutputPortObserver::MopoRestartTunnelComplete()
*/	
void CMdfHwDeviceAdapter::MopoRestartTunnelComplete(const MMdfOutputPort* /*aOutputPort*/,
	TInt /*aErrorCode*/)
	{
	
	}

/*
@see MMdfProcessingUnitObserver::InitializeComplete()
*/
void CMdfHwDeviceAdapter::InitializeComplete(const CMdfProcessingUnit* aPu, TInt aErrorCode)
	{
	iInitError = aErrorCode;
	if(aErrorCode != KErrNone)
		{
		// stop waiting on the active scheduler if we were doing so
		if (iState == EProcessingUnitInitializing)
			{
			// change state back to processing unit loaded
			iState = EProcessingUnitLoaded;
			iActiveWait->AsyncStop();
			}
		return;
		}
			
	if(aPu == iCodecPU)
		{
		iPCMPUCallbackComplete = ETrue;
		}
	else if(aPu == iAudioDevicePU)
		{
		iSourceSinkPUCallbackComplete = ETrue;
		}	

	if(iPCMPUCallbackComplete && iSourceSinkPUCallbackComplete)
		{
		// reset the flags
		iPCMPUCallbackComplete = EFalse;
		iSourceSinkPUCallbackComplete = EFalse;
		
		// Both PUs initialised OK
		if (iState == EProcessingUnitInitializing)
			{
			iActiveWait->AsyncStop();
			}
		iState = EProcessingUnitIdle;
		}
	}

/*
@see MMdfProcessingUnitObserver::ExecuteComplete()
*/
void CMdfHwDeviceAdapter::ExecuteComplete(const CMdfProcessingUnit* aPu, TInt aErrorCode)
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
	else if(aPu == iAudioDevicePU)
		{
		iSourceSinkPUCallbackComplete = ETrue;
		}	

	if(iExecuteError != KErrNone || (iPCMPUCallbackComplete && iSourceSinkPUCallbackComplete))
		{
		if (iState == EProcessingUnitExecuting)
			{
			// stop the hardware device if we are still executing
			StopHwDevice(iExecuteError);		
			iState = EProcessingUnitIdle;
			}
		// reset the flags
		iPCMPUCallbackComplete = EFalse;
		iSourceSinkPUCallbackComplete = EFalse;					
		}
	}

/*
@see MMdfHwDeviceSetup::::SetDataTypesL()
*/
void CMdfHwDeviceAdapter::SetDataTypesL(TFourCC aSrcType, TFourCC aDestType)
	{
	// Find and load an appropriate Codec
	iCodecPU = iPuLoader->LoadProcessingUnitL(*this, aSrcType, aDestType);
	}

/*
Called to indicate that the Hardware Device has been stopped, leading
to callbacks to the observer. 
@see MMMFHwDeviceObserver::Stopped()
@see MMMFHwDeviceObserver::Error()
*/
void CMdfHwDeviceAdapter::StopHwDevice(TInt error)
	{
	iHwDeviceObserver->Stopped();
	iHwDeviceObserver->Error(error);		
	}

/*
Returns the state of the Hardware Device Adapter object.
*/
void CMdfHwDeviceAdapter::GetState(THwDevAdapterState& aState) const
	{
	aState = iState;	
	}

