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

#ifndef AUDIODEVICE_H
#define AUDIODEVICE_H

#include <e32msgqueue.h>
#include <mdf/mdfprocessingunit.h>
#include <mdf/mdfinputport.h>
#include <mdf/mdfoutputport.h>
#include <mmf/server/mmfhwdevicesetup.h>
#include <mdf/mdfcommon.h>

#ifdef SYMBIAN_MDF_SHAREDCHUNK_SOUNDDRIVER
	#include "mdasoundadapter.h"
#else
	#include <mdasound.h>
#endif

class CMMFBuffer;

/* 
Audio Sink/Source processing unit
*/
class CAudioDevice : public CMdfProcessingUnit			   
	{
public:	
	/* 
	Audio Sink/Source input port
	*/
	class CInputPort: public CActive, 
					  public MMdfInputPort, 
					  public MPlayCustomInterface
		{
	public:	
		static CInputPort* NewL(CAudioDevice& aParent);
		
		// from MMdfInputPort
		TInt MipConfigure(const TPuConfig&  aConfiguration);
		TInt MipGetConfig(TPuConfig& aConfigurationSetup);
		void MipInitialize();
		void MipSetObserver(const MMdfInputPortObserver& aInputPortObserver);
		CMMFBuffer* MipCreateBuffer(TInt aBufferSize);
		TInt MipUseBuffer(CMMFBuffer& aBuffer);
		TInt MipFreeBuffer(CMMFBuffer* aBuffer);
		TInt MipTunnelRequest(const MMdfOutputPort& aOutputPortToBeConnectedTo,
			TTunnelFlags& aTunnelFlags, TSupplierType& aSupplierType);
		void MipWriteData(CMMFBuffer& aBuffer);
		void MipDisconnectTunnel();
		void MipRestartTunnel();
		TBool MipIsTunnelled() const;
		TInt MipIndex() const;
		TUint32 MipBufferSize() const;
		TInt MipCreateCustomInterface(TUid aUid);
		TAny* MipCustomInterface(TUid aUid);
		
		// from MPlayCustomInterface
		void SetVolume(TUint aVolume);
		TUint Volume();
		TUint BytesPlayed();
		void SetVolumeRamp(const TTimeIntervalMicroSeconds& aRampDuration);
		TTimeIntervalMicroSeconds& VolumeRamp();
		TInt SampleRate();
		TInt Channels();
				
		// from CActive
		void RunL();
		void DoCancel();
		
		// CInputPort
		void Execute();
		void Pause();
		void Stop();
		~CInputPort();
		
	private:
		explicit CInputPort(CAudioDevice& aParent);
		void ConstructL();
	private:
		CMMFBuffer* 			iCurrentBuffer;
		CAudioDevice& 			iParent;
		MMdfOutputPort*			iPortConnectedTo;
		RPointerArray<CMMFBuffer> 	iBuffers;
		MMdfInputPortObserver* 		iObserver;
		TInt 					iSampleRate;
		TInt 					iChannels;
		TBool 				iInterleaved;	
		TUint 				iVolume;
		TTimeIntervalMicroSeconds 	iRampDuration;
		TUint 				iBytesPlayed;
		TBool 				iPaused;
		TBool 				iStopped;
		TUint 				iBufferSize;
		};
	/* 
	Audio Sink/Source output port
	*/	
	class COutputPort:  public CActive, 
						public MMdfOutputPort,
						public MRecordCustomInterface
		{
	public:	
		static COutputPort* NewL(CAudioDevice& aParent);
		
		// from MMdfOutputPort
		TInt MopConfigure(const TPuConfig&  aConfiguration);
		TInt MopGetConfig(TPuConfig& aConfigurationSetup);
		void MopInitialize();
		void MopSetObserver(const MMdfOutputPortObserver& aOutputPortObserver);
		CMMFBuffer* MopCreateBuffer(TInt aBufferSize);
		TInt MopUseBuffer(CMMFBuffer& aBuffer);
		TInt MopFreeBuffer(CMMFBuffer* aBuffer);
		TInt MopTunnelRequest(const MMdfInputPort& aInputPortToBeConnectedTo,
			TTunnelFlags& aTunnelFlags, TSupplierType& aSupplierType);
		void MopReadData(CMMFBuffer& aBuffer);
		void MopDisconnectTunnel();
		void MopRestartTunnel();
		TBool MopIsTunnelled() const;
		TInt MopIndex() const;
		TUint32 MopBufferSize() const;
		TInt MopCreateCustomInterface(TUid aUid);
		TAny* MopCustomInterface(TUid aUid);
		
		// from MRecordCustomInterface
		void SetGain(TUint aGain);
		TUint Gain();
		TUint BytesRecorded();
		TInt SampleRate();
		TInt Channels();
		
		// from CActive		
		void RunL();
		void DoCancel();
		
		// COutputPort
		~COutputPort();
		void Execute();
		void Pause();
		void Stop();

	private:
		explicit COutputPort(CAudioDevice& aParent);
		void ConstructL();
	private:
		CMMFBuffer* 			iCurrentBuffer;
		CAudioDevice& 			iParent;			
		RPointerArray<CMMFBuffer> 	iBuffers;
		MMdfOutputPortObserver* 	iObserver;
		MMdfInputPort*			iPortConnectedTo;
		TInt 					iSampleRate;
		TInt 					iChannels;
		TBool 				iInterleaved;
		TUint 				iGain;
		TUint 				iBytesRecorded;
		TBool 				iPaused;
		TUint 				iBufferSize;
		};
public:
	static CAudioDevice* NewL();
	// from CMdfProcessingUnit
	TInt Create(const MMdfProcessingUnitObserver& aProcessingUnitObserver);
	TInt GetInputPorts(RPointerArray<MMdfInputPort>& aComponentInputPorts);
	TInt GetOutputPorts(RPointerArray<MMdfOutputPort>& aComponentOutputPorts);
	TInt Configure(const TPuConfig& aConfigurationSetup);
	TInt GetConfig(TPuConfig& aConfigurationSetup);
	void Initialize();
	void Execute (); 
	TInt Pause ();
	void Stop ();
	TProcessingUnitState State();
	TInt CreateCustomInterface(TUid aUid);
	TAny* CustomInterface(TUid aUid);
	
	// CAudioDevice
	RMdaDevSound& SoundDevice();
	MMdfProcessingUnitObserver* Observer();
	~CAudioDevice();
	
private:
	CAudioDevice();	
	void ConstructL();
private:
	CInputPort* 			iInputPort;
	COutputPort* 			iOutputPort;
	RMdaDevSound			iSoundDevice;
	MMdfProcessingUnitObserver*	iObserver;
	TProcessingUnitState 		iState;
	};
	
#endif // AUDIODEVICE_H
