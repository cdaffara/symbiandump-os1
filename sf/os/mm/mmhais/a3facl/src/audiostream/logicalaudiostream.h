/*
* Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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



#ifndef LOGICALAUDIOSTREAM_H
#define LOGICALAUDIOSTREAM_H

#include <e32base.h>
#include <a3f/a3f_trace_utils.h>
#include <a3f/audioprocessingunittypeuids.h>
#include <a3f/a3fbase.h>

#include <a3f/maudioprocessingunitobserver.h>
#include <a3f/maudiostreamobserver.h>

#include "componentparameters.h"
#include "audiostreammanager.h"

#include <a3f/msynccisupport.h>
#include "../shared/a3fbackdooraccess.h"
#include "../audiocontext/audiocontext.h"

// TO BE REMOVED WHEN MMRC SERVER IS READY
#include "mlogicalchain.h"
#include "maudiostreamadaptationobserver.h"
#include "maudiocodecadaptationobserver.h"

#include <a3f/maudiocodecobserver.h>

#include "mstreambuffercontrol.h"


class MLogicalSettingObserver;
class CLogicalAudioCodec;
class CLogicalAudioGainControl;
class CLogicalBufferSource;
class CLogicalBufferSink;
class CLogicalAudioDeviceSink;
class CLogicalAudioDeviceSource;


/**
This is the implementation of audio stream API.
@see MAudioStream
*/

NONSHARABLE_CLASS(CLogicalAudioStream) :	public CAudioStreamManager,
											public MCustomInterfaceSupport, 
											public MCustomInterfaceSupportObserver, 
											public MAudioStreamAdaptationObserver,
											public MLogicalChain,
											public MAudioCodecObserver,
											public MA3FBackdoorAccessIf
	{
friend class CAudioContext;

public:
	/**
	Destructor.
	
	Deletes all objects and releases all resource owned by this instance.
	*/
	virtual ~CLogicalAudioStream();

	// from MAudioStream
	virtual TInt Initialize();
	virtual TInt Load();
	virtual TInt Activate();
	virtual TInt Stop();
	virtual TInt Unload();
	virtual TInt Uninitialize();
	virtual TInt Prime();
	virtual TInt Flush();

	virtual TInt AddSource(MAudioProcessingUnit* aSource);
	virtual TInt AddSink(MAudioProcessingUnit* aSink);
	virtual TInt AddAudioCodec(MAudioProcessingUnit* aCodec);
	virtual TInt AddGainControl(MAudioProcessingUnit* aGainControl);
	virtual TInt RemoveProcessingUnit(MAudioProcessingUnit* aProcessingUnit);
	virtual TInt ResetStreamTime();
	virtual TInt GetStreamTime(TTimeIntervalMicroSeconds& aStreamTime);
	virtual TAny* Interface(TUid aType);

	// from MCustomInterfaceSupport
	virtual TInt RequestCustomInterface(TUid aUid, TAny*& aPtr);
	virtual TInt RegisterObserver(MCustomInterfaceSupportObserver& aObserver);
	virtual void UnRegisterObserver(MCustomInterfaceSupportObserver& aObserver); 

	// from MCustomInterfaceSupportObserver
	virtual void CustomInterfaceRemoval(TUid aInterfaceUid, TAny* aPtr);

	// from MAudioStreamAdaptationObserver
	void PhysicalAdaptationEvent(TPhysicalEvent aEvent, TInt aError);
	void StateEvent(TInt aReason, TAudioState aNewState);
	void AddProcessingUnitComplete(TUid aType, TInt aError);
	void RemoveProcessingUnitComplete(TUid aType, TInt aError);
	void ProcessingFinished ();
	void FlushComplete (TInt aError);
	
	// from MLogicalChain
	void SetMessageType(TMMRCMessageType aMessageType);
	void ResetMessage();
	TUint MessageType();
	TUid AudioProcessingUnitUid(TInt aIndex);
	TInt AudioProcessingUnitsCount();
	TAudioState StreamState();
	void SetStreamState(TAudioState aState);
	TUid CodecFormat();
	TInt GetSampleRate();
	TUid GetMode();
	TInt Priority();
	void GetVolumeRampParameters(TUid& aRampOperation, TTimeIntervalMicroSeconds& aRampDuration);
	MLogicalChain* CloneL();
	void Release();
	TAny* GetComponent(TUid aType);

	void CopySettings(const MLogicalChain& aChain);
	void SetAdaptationSource(MMMFBufferSource& aSource);
	void SetAdaptationSink(MMMFBufferSink& aSink);
	void SetAdaptationGainControl(MAudioGainControl& aGain);
	void SetStreamPositionControl(MStreamPositionControl& aControl );
	void SetCustomInterfaceProvider(MCustomInterfaceSupport& aControl);
	void SetCodecConfiguration(RArray<TInt> aSampleRate, RArray<TUid> aMode);
	void SetAdaptationStream(MConfigurationHelper& aStream);
	void SetStreamBufferControl(MStreamBufferControl& aStreamBufferControl);

	// from MAudioCodecObserver
	void SampleRateSet(TInt aError);
	void ModeSet(TInt aError);
	void GetSupportedSampleRatesComplete (TInt aError);
	void GetSupportedModesComplete (TInt aError);
	
	// from MA3FBackdoorAccessIf
	MAudioContext* AudioContext();
	MAudioStream* AudioStream();
	MAudioProcessingUnit* ProcessingUnit(TUid aType);

	/**
	Create a new instance.
	
	@param aStream the stream implementation. Ownership of the pointer is transferred to the 
			create CAudioStream object instance.
	@return CAudioStreamComponent* a pointer to the created instance.
	*/
	static CLogicalAudioStream* NewL(TUid aTypeId);
	
	//Get functions for configuration
	void GetSupportedSampleRates(RArray<TInt>& aSupportedRates);
	void GetSupportedModes(RArray<TUid>& aSupportedModes);
	
	virtual void SetParentContext(const CAudioContext& aContext);
	
protected:
	// Internal method
	void MapUidToProcessingUnit(TUid aType, MAudioProcessingUnit*& aInstance);

	void CopyStreamSettings(const MLogicalChain& aChain);
	void CopyCodecSettings(const MLogicalChain& aChain);
	void CopyGainSettings(const MLogicalChain& aChain);

private:

	void ConstructL();
	CLogicalAudioStream();
	TInt DoAddProcessingUnit(MAudioProcessingUnit* aUnit);

private:
	// Time processed
	TTimeIntervalMicroSeconds iTimeProcessed;

	// Audio processing units at this context
	RPointerArray<MAudioProcessingUnit>		iAudioProcessingUnits;
	
	// CI observers
	RPointerArray<MCustomInterfaceSupportObserver>	iCISupportObservers;

	// Stream state
	TAudioState iCurrentState;
	
	TAudioState iRequestState;

	// MLogical chain variables
	MAudioProcessingUnit*		iCodec;

	MAudioProcessingUnit*		iGain;

	// General pointer, useful to manage all the audioprocessingunit
	// never keeps specific information
	MAudioProcessingUnit*		iAudioProcessingUnit;

	// Adaptation interfaces
	MStreamPositionControl*		iPositionControl;
	MStreamBufferControl*		iStreamBufferControl;

	MCustomInterfaceSupport*	iInterfaceProvider;

	// Message type 
	TUint		iMessageType;

	TInt	iContextPriority;
	
	// handle back to parent context
	CAudioContext* iParentContext;
	};
	
#endif // LOGICALAUDIOSTREAM_H
