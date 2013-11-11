/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef AUDIOSTREAM_H
#define AUDIOSTREAM_H

#include <e32base.h>
#include <f32file.h>
#include <a3f/a3f_trace_utils.h>
#include <a3f/a3fbase.h>
#include "audiostreamadaptation.h"
#include "maudiocodecadaptationobserver.h"
#include "mconfigurationhelper.h"
#include "mstreambuffercontrol.h"

class CAudioCodec;
class CAudioStream;
class CAudioDeviceSource;
class CAudioDeviceSink;
class CBufferSink;
class CBufferSource;
class CAudioGainControl;
class CFourCCConvertor;
class MAudioStreamAdaptationObserver;
class MInputPort;
class MOutputPort;
class MAudioCodecObserver;


#include "mlogicalchain.h"

class CLogicalAudioStream;

/////
 // This is the implementation of audio stream adaptation.
 ///

class CAudioStream : 	public CAudioStreamAdaptation,
						public MAudioCodecAdaptationObserver,
						public MConfigurationHelper, 
						public MStreamBufferControl, 
						public MFlushHandlerObserver
	{
public:

	/////
	// Create a new instance.
	//
	// @param aStream the stream implementation. Ownership of the pointer is transferred to the create CAudioStream object instance.
	// @return CAudioStreamComponent* a pointer to the created instance.
	///
	static CAudioStream* NewL();

	/////
	// Destructor.
	//
	// Deletes all objects and releases all resource owned by this instance.
	///
	virtual ~CAudioStream();

	// From CAudioStreamAdaptation
	TInt RegisterAudioStreamObserver(MAudioStreamAdaptationObserver& aObserver);
	void UnregisterAudioStreamObserver(MAudioStreamAdaptationObserver& aObserver);
	void UnregisterAllAudioStreamObserver();
	void SetFourCC(const CFourCCConvertor& aFourCCConvertor);

	TInt Message(MLogicalChain& aCurrentChain, MLogicalChain& aDesiredChain, MAudioContext& aContext, TInt aFlags);

	// from class MAudioCodecAdaptationObserver
	void AllBuffersProcessed();
	void ProcessingUnitError(TInt aError);
	TInt RegisterAudioCodecObserver(MAudioCodecObserver& aObserver);
	void UnregisterAudioCodecObserver(MAudioCodecObserver& aObserver);
	void GetSupportedAModesComplete(TInt aError);
	void GetSupportedARatesComplete(TInt aError);

	// from base clase MStreamBufferControl
	TInt FlushBuffers();

	// from class MFlushHandlerObserver
	void FlushComplete(TInt aError);
protected:
	// from base clase MConfigurationhelper
	TInt GetSupportedModes(RArray<TUid>& aSupportedModes);
	TInt GetSupportedSampleRates(RArray<TInt>& aSupportedRates);

private:
	TInt CreateDataPath();

	TInt DemolishDataPath();

	TInt ChangeState(TAudioState aPreviousState, TAudioState aTargetState);

	TAny* GetComponent(TUid aType);

	TInt CreatePhysicalComponents(MLogicalChain& aDesiredChain);

	void DeletePhysicalComponents();

	void ConstructL();

	CAudioStream();

private:
	// Flags
	TBool				iIsCodecConfig;

	// Components
	MLogicalChain*		iDesiredChain;

	MLogicalChain*		iCurrentChain;

	MAudioContext* 		iAudioContext;

	CAudioCodec*		iCodec;

	CBufferSource*		iBufferSource;

	CBufferSink*		iBufferSink;

	CAudioDeviceSource*	iDeviceSource;

	CAudioDeviceSink*	iDeviceSink;

	CAudioGainControl*	iGainControl;

	// Not owned
	CFourCCConvertor*	iFourCCConvertor;

	TInt				iSampleRateConfig;

	TUid				iModeConfig;

	TAudioState			iCurrentStreamState;

	TAudioState			iDesiredStreamState;

	// Observers
	RPointerArray<MAudioStreamAdaptationObserver> iAudioStreamObservers;

	RPointerArray<MAudioCodecObserver>	iAudioCodecObservers;

	MOutputPort*		iOutputport;	// remove ports when changing to Uninitialized

	MInputPort*			iInputport;

	TTimeIntervalMicroSeconds iTimeProcessed;
	};

#endif // AUDIOSTREAM_H__
