/*
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef AUDIOCODEC_H
#define AUDIOCODEC_H

#include <e32base.h>
#include <mmf/server/mmfhwdevice.h>
#include <mmf/server/mmfhwdevicesetup.h>
#include <mmf/server/sounddevice.h>
#include <a3f/a3f_trace_utils.h>
#include <a3f/a3fbase.h>

#include "hwdeviceimplementationuids.hrh"

#include <mmf/plugin/mmfhwdeviceimplementationuids.hrh>

#include "moutputport.h"
#include "minputport.h"
#include "mgainhelper.h"
#include "mstreampositioncontrol.h"

#include <a3f/msynccisupport.h>
#include <a3f/maudiocodec.h>

#define KFOURCCLENGTH 4


enum TMode
	{
	// Decoder mode
	EDecode = 0x01,
	// And vice versa
	EEncode = 0x02
	};

class TSampleRateTableEntry
	{
public:
	TInt		iSampleRateValue;
	TMMFSampleRate iSampleRate;
	};

class TAudioModeTableEntry
	{
public:
	TMMFMonoStereo	iAudioModeValue;
	TUid 			iAudioMode;
	};

const TInt KMaxModeIndex = 1;



class MAudioCodecAdaptationObserver;
class CFourCCConvertor;

	/////
	//This is the reference implementation of Phyisical AudioCodec
	//@lib audiocodec.lib
	///

// TODO: Check if those interfaces MInputPort, MOutputPort should be replaced by MMMFAudioDataSupplier, MMMFAudioDataConsumer


NONSHARABLE_CLASS(CAudioCodec) :public CActive,
								public MMMFHwDeviceObserver,
								public MInputPort,				// Adaptation interface
								public MOutputPort,				// Adaptation interface
								public MGainHelper,				// Adaptation interface
								public MCustomInterfaceSupport,	// A3F interface
								public MStreamPositionControl	// Adaptation interface
	{
friend class CAudioGainControl;
public:

	/////
	// Create a new instance.
	//
	// @param aTypeId.
	// @return CAudioCodec* a pointer to the created instance.
	///

	IMPORT_C static CAudioCodec* NewL(TUid aTypeId, const CFourCCConvertor& aFourCCConvertor);

	// Check if this would be removed
	/////
	//  Structure to hold hw device init parameters
	///
	class THwDeviceInitArgs
		{
	public:
		TUint8 iPolicyId;
		TInt iEapStreamId;
		};

	/////
	 // Destructor.
	 //
	 // Deletes all objects and releases all resource owned by this instance.
	///
	virtual ~CAudioCodec();


	IMPORT_C TInt SetFormat(TUid aFormat);
	IMPORT_C TInt RegisterAudioCodecObserver(MAudioCodecAdaptationObserver& aObserver);
	IMPORT_C void UnregisterAudioCodecObserver(MAudioCodecAdaptationObserver& aObserver);
	IMPORT_C TInt Initialize();
	IMPORT_C TInt Load(TInt aSampleRate, TUid aMode);
	IMPORT_C TInt Start();
	IMPORT_C TInt Pause();
	IMPORT_C TInt Stop();



	// From MOutputPort
	TInt GetOutputPort(MOutputPort*& aOutputPort);

	// From MInputPort
	TInt GetInputPort(MInputPort*& aInputPort);

	// From MCustomInterfaceSupport
	virtual TInt RequestCustomInterface(TUid aUid, TAny*& aPtr);
	virtual TInt RegisterObserver(MCustomInterfaceSupportObserver& aObserver);
	virtual void UnRegisterObserver(MCustomInterfaceSupportObserver& aObserver);

	// from base class MInputPort
	virtual TInt EmptyBuffer(CMMFBuffer* aBuffer, MOutputPort* aSupplier);
	virtual TInt BufferFilled(CMMFBuffer* aBuffer);
	virtual TInt SetOutput(MOutputPort* aOutput);
	virtual TInt RemoveOutput(MOutputPort* aOutput);

	// from base class MOutputPort
	virtual TInt FillBuffer(CMMFBuffer* aBuffer, MInputPort* aConsumer);
	virtual TInt BufferEmptied(CMMFBuffer* aBuffer);
	virtual TInt SetInput(MInputPort* aInput);
	virtual TInt RemoveInput(MInputPort* aInput);
	virtual TInt FlushBuffer(MFlushHandlerObserver* aFlushObserver);

	// from base class MMMFHwDeviceObserver
	virtual TInt FillThisHwBuffer(CMMFBuffer& aHwBuffer);
	virtual TInt EmptyThisHwBuffer(CMMFBuffer& aHwBuffer);
	virtual TInt MsgFromHwDevice(TUid aMessageType, const TDesC8& aMsg);
	virtual void Stopped();
	virtual void Error(TInt aError);

	// from base class MGainHelper
	TInt GetGain(TInt& aGain) const;
	TInt SetGain(RArray<TAudioChannelGain>& aChannels);
	TInt ConfigureRamp(TUid aRampOperation, const TTimeIntervalMicroSeconds& aRampDuration);

	// from base class MStreamPositionControl
	TInt GetControlPosition(TTimeIntervalMicroSeconds& aPosition);
	void ResetControlPosition();


	TInt ResolveMode(TUid aMode);
	TInt GetSupportedSampleRates(RArray<TInt>& aSupportedRates);
	TInt GetSupportedModes(RArray<TUid>& aSupportedModes);

	//Return the supported values to the client
	IMPORT_C TInt SupportedModes(RArray<TUid>& aSupportedModes);
	IMPORT_C TInt SupportedRates(RArray<TInt>& aSupportedRates);
	
protected:
	// from CActive
	void RunL();
	void DoCancel();
	TInt RunError(TInt aError);

	TInt LoadHwDevice();
	TInt InitHwDevice();
	TInt SetHwDeviceConfig();
	void FourCCToHwDeviceUidL(TFourCC aFourCC, TMode aMode, TUid &aHWDev);

	TBool IsDataTypeMatch(CImplementationInformation* aHwDeviceResourceEntry,
		const TDesC8& aHwMatchFourCC, TMode aState);

	// TODO: This will be replaced, when the new mechanism to get the supported configuration
	// will be defined
	/////
	 // Reads hw device parameters from resource files.
	 // @return error code.
	 ///
	TInt ReadResourceRates(RArray<TInt>& aRates);
	TInt ReadResourceModes(RArray<TUid>& aModes);

	void UpdateBytesPlayed();

private:
	void ConstructL();

	CAudioCodec(TUid aTypeId, const CFourCCConvertor& aFourCCConvertor);

private:
	TUid						iType;
	// Indicates if this is an encoder or decoder
	TMode						iMode;

	// The codec format
	TUid						iFormat;

	// Resolved HwDevice Uid
	TUid						iDeviceUid;

	//

	MPlayCustomInterface*		iPlayCustomInterface;

	MRecordCustomInterface*		iRecordCustomInterface;

	// HwDevice init parameters
	THwDeviceInitParams			iHwDeviceInitParams;
	THwDeviceInitArgs			iHwDeviceInitArgs;

	// CI observers
	RPointerArray<MCustomInterfaceSupportObserver>	iCISupportObservers;

	enum THwDeviceState
		{
		EIdle,
		ERunning,
		EPaused,
		EStopped
		};

	TBool						iLastBuffer;
	THwDeviceState				iHwDeviceState;
	TBool						iLastBufferAck;
	TInt						iRequest;
	TInt						iErrorRates;
	TInt						iErrorModes;
	TInt						iSampleRateConfig;
	TUid						iModeConfig;

	CMMFHwDevice*				iHwDevice;
	CMMFDataBuffer*				iAdaptationBuffer;

	MOutputPort*				iOutputPort;
	MInputPort*					iInputPort;
	TInt64						iPosition;
	TInt64 						iPlayedBytesCount;

	CFourCCConvertor* 			iFourCCConvertor;
	RPointerArray<MAudioCodecAdaptationObserver> iAudioCodecObservers;
	MIgnoreUnderflowEventsCustomInterface* iIgnoreUnderflowInterface;
	TUid						iRampOperation;
	TTimeIntervalMicroSeconds	iRampDuration;
	};

/*
 *	Updates the total bytes played.
 *
 */
inline void CAudioCodec::UpdateBytesPlayed()
	{
	if (iPlayCustomInterface)
		{
		iPlayedBytesCount = iPlayCustomInterface->BytesPlayed();
		}
	}

#endif // AUDIOCODEC_H
