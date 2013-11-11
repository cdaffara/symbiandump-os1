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

#ifndef __MMF_VIDEOINPUT_H__
#define __MMF_VIDEOINPUT_H__

// Standard EPOC32 includes required by this header file
#include <e32base.h>
#include <e32std.h>

#include <e32hal.h>
#include <e32svr.h>

// Public Media Server includes
#include <mmf/server/mmfdatasink.h>
#include <mmf/server/mmfdatasource.h>
#include <mmf/server/mmfdatapath.h>
#include <mmf/server/mmfdatasourcesink.hrh>

#include <mmf/server/mmfcodec.h>
#include <mmf/server/mmfdatabuffer.h>
#include <mmf/common/mmfpaniccodes.h>

#include "mmfVideoInputInterfaceUIDs.hrh"
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <mmf/common/mmfhelper.h>
#endif
// DevSound

const TUint KVideoInputDefaultFrameSize = 0x1000;
//#define KVideoInputCanResample		//uncomment this to allow Video Input to sample rate convert
const TUid KUidMmfVideoInput	= {KMmfUidVideoInputInterface};

class CHardwareCodec;

void Panic(TInt aPanicCode);

class MMMFVideoInput : public MDataSource
/*
Interface class to allow dynamic linkage to CMMFVideoInput.
*/
	{
public:
	//factory function
	inline static MMMFVideoInput* NewVideoInputL(TUid aImplementationUid, const TDesC8& aInitData);
	//interface
	virtual void HWFillBufferL(CMMFBuffer* aBuffer, MDataSink* aConsumer) = 0;
protected:
	//constructor
	inline MMMFVideoInput() : MDataSource(KUidMmfVideoInput) {};
	};

inline MMMFVideoInput* MMMFVideoInput::NewVideoInputL( TUid aImplementationUid,  const TDesC8& aInitData )
	{
	//make sure the cast will be ok by checking the uid is what we expect
//	__ASSERT_ALWAYS(aImplementationUid==KUidMmfVideoInput, Panic(EMMFVideoInputPanicBadUID));
	MMMFVideoInput* retPtr = STATIC_CAST(MMMFVideoInput*, MDataSource::NewSourceL(aImplementationUid, aInitData));
	return retPtr;
	}


class CMMFVideoInput : public CBase, public MMMFVideoInput
/**
The interface into DevSound
**/
	{
public:

	static MDataSource* NewSourceL();
	virtual ~CMMFVideoInput();

	//MDataSource mixin implementations
	virtual TFourCC SourceDataTypeCode(TMediaId /*aMediaId*/);
	virtual void FillBufferL(CMMFBuffer* aBuffer, MDataSink* aConsumer,TMediaId /*aMediaId*/);//called by a MDataSink to request buffer fill
	virtual void BufferEmptiedL(CMMFBuffer* aBuffer); //called by MDataSink to pass back emptied buffer to the source
	virtual TBool CanCreateSourceBuffer();
	virtual CMMFBuffer* CreateSourceBufferL(TMediaId /*aMediaId*/);
		//XXX optimal source buffer size creation may depend on the sink buffer & vice versa
	virtual CMMFBuffer* CreateSourceBufferL(TMediaId aMediaId, CMMFBuffer& /*aSinkBuffer*/)
		{return CreateSourceBufferL(aMediaId);};
	virtual CMMFBuffer* CreateSourceBufferL(TMediaId /*aMediaId*/, TBool &aReference);
		//XXX optimal source buffer size creation may depend on the sink buffer & vice versa
	virtual CMMFBuffer* CreateSourceBufferL(TMediaId aMediaId, CMMFBuffer& /*aSinkBuffer*/, TBool &aReference)
		{return CreateSourceBufferL(aMediaId, aReference);};
	virtual TInt SourceThreadLogon(MAsyncEventHandler& aEventHandler);
	virtual void SourceThreadLogoff();
	virtual void SourcePrimeL();
	virtual void SourceStopL();
	virtual void SourcePlayL();
	virtual void SetSourcePrioritySettings(const TMMFPrioritySettings& aPrioritySettings);

	virtual void NegotiateL(MDataSink& aSink);

	// MMMFVideoInput mixin implementations
	TInt BytesPlayed();
	void HWFillBufferL(CMMFBuffer* aBuffer, MDataSink* aConsumer); //called by a FillBufferL if using HW codec

	TRequestStatus iBufferStatus;

protected:
	virtual void ConstructSourceL( const TDesC8& aInitData ) ;

private:
	inline CMMFVideoInput() {}

	MAsyncEventHandler* iEventHandler;



private:

	enum TCodec
		{
		EUseARMCodec=0,
		EUseHWCodec
		};

	enum TState
		{
		EIdle=0,
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
	CHardwareCodec* iHWCodec;
	CMMFBuffer* iBuffer;

	TBool iFirstBufferRead;

	TBool iDevSoundLoaded;
	TBool iStopped;

	TError iError;
	TInt iMMFDevsoundError;
	TCodec iCodecSelect;
	TMMFPrioritySettings iPrioritySettings;

	TUint iSinkSampleRate;
	TUint iSinkChannels;
	TFourCC iSinkFourCC;
	TBool iNeedsSWConversion;
	CMMFChannelAndSampleRateConverterFactory* iChannelAndSampleRateConverterFactory;
	CMMFChannelAndSampleRateConverter* iChannelAndSampleRateConverter;
	CMMFDataBuffer* iConvertBuffer;
	CMMFBuffer* iBufferToEmpty;
	CMMFBuffer* iDevSoundBuf;
	};

#endif
