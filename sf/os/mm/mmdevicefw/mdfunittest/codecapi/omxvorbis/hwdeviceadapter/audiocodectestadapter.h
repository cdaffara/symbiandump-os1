/*
* Copyright (c) 2006-2008 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
*
*/


/**
@file
@internalComponent
*/

#ifndef AUDIOCODECTESTADAPTER_H
#define AUDIOCODECTESTADAPTER_H

#include <mmf/server/mmfhwdevice.h>
#include <mdf/mdfinputport.h>
#include <mdf/mdfoutputport.h>
#include <mdf/mdfprocessingunit.h>
#include <mdf/mdfpuloader.h>
#include <mmf/server/mmfdatabuffer.h>
#include <mmf/server/mmfhwdevicesetup.h>
#include <mmf/common/mmfutilities.h>

class CMMFBuffer;

class CMdfHwDeviceCodecTestAdapter :	public CMMFHwDevice,
									 	public MMdfInputPortObserver,
										public MMdfOutputPortObserver,
										public MMdfProcessingUnitObserver,
										public MMdfHwDeviceSetup
	{
public:
	/*
	Hardware Device Adapter panics raised as a result of a programming error.
	*/	
	enum THwDeviceAdapterPanics
		{
		/*
		Raised when the Codec Processing Unit has not been initialised.
		@see CMdfHwDeviceAdapter::CustomInterface
		@see CMdfHwDeviceAdapter::SetDataTypesL
		*/
		EPanicCodecNotSet
		};
	/**
	The current state of the Hardware Device Adapter.
	*/
	enum THwDevAdapterState
		{
		/*
		The PULoader has been loaded.
		*/
		EProcessingUnitLoaderLoaded,
		/*
		The Processing Units have been loaded.
		*/
		EProcessingUnitLoaded,
		/*
		The Processing Units are currently being initialised.
		*/
		EProcessingUnitInitializing,
		/*
		The Processing Units are currently in the idle state.
		*/
		EProcessingUnitIdle,
		/*
		The Processing Units are currently in the executing state.
		*/
		EProcessingUnitExecuting,
		/*
		The Processing Units are currently in the paused state.
		*/
		EProcessingUnitPaused		
		};		
	
public:
	static CMdfHwDeviceCodecTestAdapter* NewL();
	
	// from CMMFHwDevice
	TInt Start(TDeviceFunc aFuncCmd, TDeviceFlow aFlowCmd);
	TInt Stop();
	TInt Pause();
	TInt Init(THwDeviceInitParams& aDevInfo);
	TAny* CustomInterface(TUid aInterfaceId);
	TInt ThisHwBufferFilled(CMMFBuffer& aFillBufferPtr);
	TInt ThisHwBufferEmptied(CMMFBuffer& aEmptyBufferPtr);
	TInt SetConfig(TTaskConfig& aConfig);
	TInt StopAndDeleteCodec();
	TInt DeleteCodec();
	~CMdfHwDeviceCodecTestAdapter();
	
	// from MMdfInputPortObserver
	void MipoWriteDataComplete(const MMdfInputPort* aPu,
		CMMFBuffer* aBuffer, TInt aErrorCode);
	void MipoDisconnectTunnelComplete(const MMdfInputPort* aPu,  TInt aErrorCode);
	void MipoRestartTunnelComplete(const MMdfInputPort* aPu, TInt aErrorCode);
	
	// from MMdfOutputPortObserver
	void MopoReadDataComplete(const MMdfOutputPort* aPu, 
		CMMFBuffer* aBuffer, TInt aErrorCode);
	void MopoDisconnectTunnelComplete(const MMdfOutputPort* aPu, TInt aErrorCode);
	void MopoRestartTunnelComplete(const MMdfOutputPort* aPu, TInt aErrorCode);
	
	// from MMdfProcessingUnitObserver
	void InitializeComplete(const CMdfProcessingUnit* aPu, TInt aErrorCode);
	void ExecuteComplete(const CMdfProcessingUnit* aPu, TInt aErrorCode);
	
	// from MMdfHwDeviceSetup	
	void SetDataTypesL(TFourCC aSrcType, TFourCC aDestType);
	
	void GetState(THwDevAdapterState& aState) const;
	
private:
	CMdfHwDeviceCodecTestAdapter();
	void ConstructL();
	TInt CreateBuffers();
	TInt StartEncode();
	TInt StartDecode();
	TInt InitializeEncodeDecode();
	TInt StartExecuting();
	
	void StopHwDevice(TInt error);

private:
	CMdfPuLoader* iPuLoader;
	TUid iPuLoaderDtorKey;
	MMdfInputPort* iInputPort;
	MMdfOutputPort* iOutputPort;
	
	CMdfProcessingUnit* iCodecPU;
		
	THwDevAdapterState iState;
	TBool iStopping;
	
	TBool iPCMPUCallbackComplete;
	
	TBool iPCMPuMopoStopCompleted;
	TBool iPCMPuMipoStopCompleted;
	
	TInt iExecuteError;
	
	enum TPUType
		{
		EPcmPu
		};

	CMMFBuffer* iInputBuffer;
	CMMFBuffer* iOutputBuffer;
	
	TUint32 iInputPortBufferSize;
	TUint32 iOutputPortBufferSize;
	TInt iInputPortBufferData;	
	TInt iOutputPortBufferData;
	
	CActiveSchedulerWait* iActiveWait;

	TDeviceFunc iFuncCmd;

	TFourCC iFirstFourCC;
	TFourCC iSecondFourCC;
	};
	
#endif // AUDIOCODECTESTADAPTER_H
