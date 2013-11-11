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
// mmfhwdeviceadapter.h
// 
//

/**
 @file
 @publishedPartner
 @prototype
 @see CMMFHwDevice
 @see MMdfInputPortObserver
 @see MMdfOutputPortObserver
 @see MMdfProcessingUnitObserver
 @see MMdfHwDeviceSetup
*/

#ifndef HWDEVICEADAPTER_H
#define HWDEVICEADAPTER_H

#include <mdf/mdfinputport.h>
#include <mdf/mdfoutputport.h>
#include <mdf/mdfprocessingunit.h>
#include <mdf/mdfpuloader.h>
#include <mmf/server/mmfhwdevice.h>
#include <mmf/server/mmfdatabuffer.h>
#include <mmf/server/mmfhwdevicesetup.h>
#include <mmf/common/mmfutilities.h>

class CMMFBuffer;

/*
Hardware Device Adapter class that uses Processing Units (and their associated Ports) in
order to decode and encode audio data.
*/
class CMdfHwDeviceAdapter : public CMMFHwDevice,
				    		public MMdfInputPortObserver,
				    		public MMdfOutputPortObserver,
				    		public MMdfProcessingUnitObserver,
				    		public MMdfHwDeviceSetup
	{
public:	
	/*
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
	static CMdfHwDeviceAdapter* NewL();
	
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
	~CMdfHwDeviceAdapter();
	
	// from MMdfInputPortObserver
	void MipoWriteDataComplete(const MMdfInputPort* aPu, CMMFBuffer* aBuffer, TInt aErrorCode);
	void MipoDisconnectTunnelComplete(const MMdfInputPort* aPu, TInt aErrorCode);
	void MipoRestartTunnelComplete(const MMdfInputPort* aPu, TInt aErrorCode);
	
	// from MMdfOutputPortObserver
	void MopoReadDataComplete(const MMdfOutputPort* aPu, CMMFBuffer* aBuffer, TInt aErrorCode);
	void MopoDisconnectTunnelComplete(const MMdfOutputPort* aPu, TInt aErrorCode);
	void MopoRestartTunnelComplete(const MMdfOutputPort* aPu, TInt aErrorCode);
	
	// from MMdfProcessingUnitObserver
	void InitializeComplete(const CMdfProcessingUnit* aPu, TInt aErrorCode);
	void ExecuteComplete(const CMdfProcessingUnit* aPu, TInt aErrorCode);
	
	// from MMdfHwDeviceSetup	
	void SetDataTypesL(TFourCC aSrcType, TFourCC aDestType);
	
	// CMdfHwDeviceAdapter
	void GetState(THwDevAdapterState& aState) const;
	
protected:
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
	
private:
	CMdfHwDeviceAdapter();
	void ConstructL();
	TInt CreateBuffers();
	TInt StartEncode();
	TInt StartDecode();
	TInt InitializeEncode();
	TInt InitializeDecode();
	TInt StartExecuting();
	void StopHwDevice(TInt error);
private:
	enum TPUType
		{
		EPcmPu,
		ESourceSinkPu
		};
	CMdfPuLoader* iPuLoader;
	TUid iPuLoaderDtorKey;
	MMdfInputPort* iCodecInputPort;
	MMdfOutputPort* iCodecOutputPort;
	MMdfInputPort* iSinkInputPort;
	MMdfOutputPort* iSourceOutputPort;
	CMdfProcessingUnit* iCodecPU;
	CMdfProcessingUnit* iAudioDevicePU;
	THwDevAdapterState iState;
	TBool iPCMPUCallbackComplete;
	TBool iSourceSinkPUCallbackComplete;
	TBool iSourceSinkPuMopoStopCompleted;
	TBool iPCMPuMopoStopCompleted;
	TBool iPCMPuMipoStopCompleted;
	TBool iStopping;
	TInt iInitError;
	TInt iExecuteError;
	CMMFBuffer* iInputBuffer;
	CMMFBuffer* iOutputBuffer;
	TInt iInputPortBufferData;	
	TInt iOutputPortBufferData;
	CActiveSchedulerWait* iActiveWait;
	TDeviceFunc iFuncCmd;
	TFourCC iFirstFourCC;
	TFourCC iSecondFourCC;
	};
	
#endif // HWDEVICEADAPTER_H
