// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __MMF_AUDIOOUTPUT_H__
#define __MMF_AUDIOOUTPUT_H__

// Standard EPOC32 includes required by this header file
#include <e32base.h>
#include <e32std.h>


#include <mmf/common/mmfpaniccodes.h>

//DevSound
#include <mmf/server/sounddevice.h>

/** 
@publishedAll
@deprecated
*/
const TUint KAudioOutputDefaultFrameSize = 0x1000;


void Panic(TInt aPanicCode);

/**
@publishedAll
@released

Interface class to allow dynamic linkage to CMMFAudioOutput.
*/
class MMMFAudioOutput : public MDataSink
	{
public:
	//factory function
	/**
	Gets a new audio output object.

	@param  aImplementationUid
	        The implementation UID.
	@param  aInitData
	        The initialisation data.

	@return The audio output object.
	*/
	inline static MMMFAudioOutput* NewAudioOutputL(TUid aImplementationUid, const TDesC8& aInitData);
	//interface

	/**
	Returns the sound device.

	Accessor function exposing public CMMFDevsound methods.

	@return	A reference to a CMMFDevSound objector.
	*/
	virtual CMMFDevSound& SoundDevice() = 0;

	/**
	Returns the number of bytes played.

	@return	The number of bytes played. If 16-bit divide this number returned by 2 to get word
	        length.
	*/
	virtual TInt BytesPlayed() = 0;

	/**
	@deprecated

	Gets audio from hardware device abstracted MMFDevsound (not used).

	@param	aBuffer
			The data to read in from a Hardware Device
	@param	aSupplier
			The MDataSink consuming the data contained in aBuffer.
	*/
	virtual void HWEmptyBufferL(CMMFBuffer* aBuffer, MDataSource* aSupplier)=0;//called by a EmptyBufferL if using HW codec
protected:
	//constructor
	/**
	Protected constructor.
	*/
	MMMFAudioOutput() : MDataSink(KUidMmfAudioOutput) {}
	};

inline MMMFAudioOutput* MMMFAudioOutput::NewAudioOutputL( TUid aImplementationUid,  const TDesC8& aInitData )
	{
	//make sure the cast will be ok by checking the uid is what we expect
	__ASSERT_ALWAYS(aImplementationUid==KUidMmfAudioOutput, Panic(EMMFAudioOutputPanicBadUID));
	MMMFAudioOutput* retPtr = STATIC_CAST(MMMFAudioOutput*, MDataSink::NewSinkL(aImplementationUid, aInitData));
	return retPtr;
	}

/**
@publishedAll
@released

The interface into DevSound.

Abstract data sink class providing an interface into hardware sound output.

Uses CMMFDevSound to access such output.
*/
class CMMFChannelAndSampleRateConverterFactory;
class CMMFChannelAndSampleRateConverter;
class CMMFAudioOutput : public CBase, public MMMFAudioOutput, public MDevSoundObserver

	{
public:
	static MDataSink* NewSinkL() ;
	virtual ~CMMFAudioOutput();
	void ConstructL();

	//MDataSink mixin implementations
	virtual TFourCC SinkDataTypeCode(TMediaId aMediaId); //used by data path MDataSource/Sink for codec matching
	virtual void EmptyBufferL(CMMFBuffer* aBuffer, MDataSource* aSupplier, TMediaId aMediaId);//called by a MDataSource to empty a buffer
	virtual void BufferFilledL(CMMFBuffer* aBuffer); //called by MDataSource to pass back full buffer to the sink
	virtual TBool CanCreateSinkBuffer();
	virtual CMMFBuffer* CreateSinkBufferL(TMediaId aMediaId, TBool &aReference);
	virtual TInt SinkThreadLogon(MAsyncEventHandler& aEventHandler);
	virtual void SinkThreadLogoff();
	virtual void SinkPrimeL();
	virtual void SinkPauseL();
	virtual void SinkPlayL();
	virtual void SinkStopL();
	virtual void SetSinkPrioritySettings(const TMMFPrioritySettings& aPrioritySettings);
	virtual void NegotiateL(MDataSource& aSource);
	// MMMFAudioOutput mixin implementations
	TInt BytesPlayed();
	void HWEmptyBufferL(CMMFBuffer* aBuffer, MDataSource* aSupplier);//called by a EmptyBufferL if using HW codec
	CMMFDevSound& SoundDevice();
	virtual TInt SetSinkDataTypeCode(TFourCC aSinkFourCC, TMediaId aMediaId);

	//The following DataType() methods should not be used.
	//They are provided to maintain BC/SC with 7.0s
	virtual void SetDataTypeL(TFourCC aAudioType);
	virtual TFourCC DataType() const;

protected:
	virtual void ConstructSinkL( const TDesC8& aInitData ) ;

private:
	inline CMMFAudioOutput() {}

	void ConfigDevSoundL();
	void LoadL();
	TInt State();

	//MDevSoundObserver mixin implementations	
	virtual void InitializeComplete(TInt aError);
	virtual void ToneFinished(TInt aError); 
	virtual void BufferToBeFilled(CMMFBuffer* aBuffer);
	virtual void PlayError(TInt aError); 
	virtual void BufferToBeEmptied(CMMFBuffer* aBuffer); 
	virtual void RecordError(TInt aError); 
	virtual void ConvertError(TInt aError);
	virtual void DeviceMessage(TUid aMessageType, const TDesC8& aMsg);
	virtual void SendEventToClient(const TMMFEvent& /*aEvent*/);

public:
	virtual TBool IsResumeSupported();

private:
	MAsyncEventHandler* iEventHandler;
	CMMFDevSound* iMMFDevSound;	//this is now private to stop stuff "grabbing it"
	CMMFBuffer* iAudioBuffer;
	TInt iInitializeState;	
	enum TState
		{
		EIdle=0,
		EDevSoundReady,
		EPaused
		};

	enum TError
		{
		ECantCreateBuffer,
		EEmptyBuffer,
		EDevSoundNotLoaded,
		ERecordNotSupported,
		EDevSoundError,
		EUnknown
		};

	MDataSource* iSupplier;
	TState iState;

	TBool iCanSendBuffers;
	TBool iPlayStarted;
	TBool iDevSoundLoaded;

	TError iError;
	TInt iMMFDevsoundError;
	TMMFPrioritySettings iPrioritySettings;

	TUint iSourceSampleRate; 
	TUint iSourceChannels;
	TFourCC iSourceFourCC;
	TFourCC iDataTypeCode;
	CMMFDataBuffer* iDevSoundBuffer;

	TMMFCapabilities iDevSoundConfig;

 	TBool iNeedsSWConversion;
	TUint iSWConvertSampleRate;
	TUint iSWConvertChannels;
 	CMMFChannelAndSampleRateConverterFactory* iChannelAndSampleRateConverterFactory;
 	CMMFChannelAndSampleRateConverter* iChannelAndSampleRateConverter;
 	CMMFDataBuffer* iConvertBuffer;
 	CMMFBuffer* iBufferToEmpty;
 	CActiveSchedulerWait* iActiveSchedulerWait;
	};

#endif
