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

#ifndef __MMF_AUDIOINPUT_H__
#define __MMF_AUDIOINPUT_H__

// Standard EPOC32 includes required by this header file
#include <e32base.h>
#include <e32std.h>

// Public Media Server includes
#include <mmf/common/mmfpaniccodes.h>

//DevSound
#include <mmf/server/sounddevice.h>

/** 
@publishedAll
@deprecated
*/
const TUint KAudioInputDefaultFrameSize = 0x1000;  //4K
//#define KAudioInputCanResample		//uncomment this to allow Audio Input to sample rate convert 


void Panic(TInt aPanicCode);


/**
@publishedAll
@released

Interface class to allow dynamic linkage to CMMFAudioInput.
*/
class MMMFAudioInput : public MDataSource
	{
public:

	//factory function
	inline static MMMFAudioInput* NewAudioInputL(TUid aImplementationUid, const TDesC8& aInitData);

	//interface
	/**
	Returns the sound device.

	@return A reference to a CMMFDevSound objector. Accessor function exposing public CMMFDevsound methods.
	*/
	virtual CMMFDevSound& SoundDevice() = 0;

	/**
	Returns the number of bytes played.

	@return	The number of bytes played. If 16-bit divide this number returned by 2 to get word length.
	*/
	virtual TInt BytesPlayed() = 0;

	/**
	@deprecated

	Gets audio from hardware device abstracted MMFDevsound (not used).

	@param  aBuffer
	        The data to read in from a Hardware Device
	@param  aConsumer
	        The MDataSink consuming the data contained in aBuffer.
	*/
	virtual void HWFillBufferL(CMMFBuffer* aBuffer, MDataSink* aConsumer) = 0;
protected:
	//constructor
	/**
	Protected constructor.
	*/
	inline MMMFAudioInput() : MDataSource(KUidMmfAudioInput) {};
	};

/**
Allocates and constructs an audio input object. 

Calls MDataSource::NewSourceL().

@param  aImplementationUid
		The Implementation UID.
@param  aInitData
		The initialisation data.

@return A new audio input object.
*/
inline MMMFAudioInput* MMMFAudioInput::NewAudioInputL( TUid aImplementationUid,  const TDesC8& aInitData )
	{
	//make sure the cast will be ok by checking the uid is what we expect
	__ASSERT_ALWAYS(aImplementationUid==KUidMmfAudioInput, Panic(EMMFAudioInputPanicBadUID));
	MMMFAudioInput* retPtr = STATIC_CAST(MMMFAudioInput*, MDataSource::NewSourceL(aImplementationUid, aInitData));
	return retPtr;
	}

/**
@publishedAll
@released

Concrete data source to provide microphone support.

Intended to be used by controller plugin developers for source plugins. Uses CMMFDevSound to access
hardware.
*/
class CMMFAudioInput : public CBase, public MMMFAudioInput, public MDevSoundObserver

	{
public:

	static MDataSource* NewSourceL();
	virtual ~CMMFAudioInput();
	void ConstructL();

	//MDataSource mixin implementations
	virtual TFourCC SourceDataTypeCode(TMediaId aMediaId);
	virtual void FillBufferL(CMMFBuffer* aBuffer, MDataSink* aConsumer,TMediaId aMediaId);//called by a MDataSink to request buffer fill
	virtual void BufferEmptiedL(CMMFBuffer* aBuffer); //called by MDataSink to pass back emptied buffer to the source
	virtual TBool CanCreateSourceBuffer();
	virtual CMMFBuffer* CreateSourceBufferL(TMediaId aMediaId);

	inline virtual CMMFBuffer* CreateSourceBufferL(TMediaId aMediaId, CMMFBuffer& aSinkBuffer);

	virtual CMMFBuffer* CreateSourceBufferL(TMediaId aMediaId, TBool &aReference);

	inline virtual CMMFBuffer* CreateSourceBufferL(TMediaId aMediaId, CMMFBuffer& aSinkBuffer, TBool &aReference);
	virtual TInt SourceThreadLogon(MAsyncEventHandler& aEventHandler);
	virtual void SourceThreadLogoff();
	virtual void SourcePrimeL();
	virtual void SourceStopL();
	virtual void SourcePlayL();
	virtual void SourcePauseL();
	virtual void SetSourcePrioritySettings(const TMMFPrioritySettings& aPrioritySettings);

	virtual void NegotiateSourceL(MDataSink& aSink);

	// MMMFAudioInput mixin implementations
	TInt BytesPlayed();
	void HWFillBufferL(CMMFBuffer* aBuffer, MDataSink* aConsumer); //called by a FillBufferL if using HW codec
	CMMFDevSound& SoundDevice();
	virtual TInt SetSourceDataTypeCode(TFourCC aSourceFourCC, TMediaId aMediaId);

	void LoadL();

	//The following DataType() methods and iBufferStatus should not be used.
	//They are provided to maintain BC/SC with 7.0s
	virtual void SetDataTypeL(TFourCC aAudioType);
	virtual TFourCC DataType() const;
	/**
	Buffer completion status.
	*/
	TRequestStatus iBufferStatus;

protected:
	virtual void ConstructSourceL( const TDesC8& aInitData ) ;

private:
	inline CMMFAudioInput() {}

	MAsyncEventHandler* iEventHandler;
	CMMFDevSound* iMMFDevSound;	//this is now private to stop stuff "grabbing it"
	
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

	void ConfigDevSoundL();
	
private:
	
	enum TCodec
		{
		EUseARMCodec=0,
		EUseHWCodec
		};

	enum TState
		{
		EIdle=0,
		EPaused,
		EDevSoundReady,
		EBufferEmpty
		};

	enum TError
		{
		ECantCreateBuffer,
		EEmptyBuffer,
		EDevSoundNotLoaded,
		EUnknown
		}; 
	

	MDataSink* iConsumer;
	TState iState;

	TBool iFirstBufferRequested;
	TBool iFirstBufferRead;

	TBool iDevSoundLoaded;
	TBool iStopped;

	TError iError;
	TInt iMMFDevsoundError;
	TMMFPrioritySettings iPrioritySettings;

	TUint iSinkSampleRate;
	TUint iSinkChannels;
	TFourCC iSinkFourCC;
	TFourCC iDataTypeCode;
	CMMFBuffer* iDevSoundBuf;

	TBool iPausePending;
	TInt iInitializeState;	
 	CActiveSchedulerWait* iActiveSchedulerWait;
	};

/**
Intended for synchronous usage (returns buffer specific to a Media Type).

@param	aMediaId
        The Media ID.
@param	aSinkBuffer
        Not Used.

@return A pointer to the CMMFBuffer source buffer.
*/
inline CMMFBuffer* CMMFAudioInput::CreateSourceBufferL(TMediaId aMediaId, CMMFBuffer& /*aSinkBuffer*/)
{
	return CreateSourceBufferL(aMediaId);
}

/**
Creates a source buffer.

Intended for asynchronous usage (returns buffer specific to a Media Type).

@param  aMediaId
        The Media ID.
@param  aSinkBuffer
        Not used.
@param	aReference
        A boolean indicating if MDataSource owns the buffer. If EFalse the the caller owns the 
        buffer.

@return The source buffer.
*/
inline CMMFBuffer* CMMFAudioInput::CreateSourceBufferL(TMediaId aMediaId, CMMFBuffer& /*aSinkBuffer*/, TBool &aReference)
{
	return CreateSourceBufferL(aMediaId, aReference);
}

#endif
