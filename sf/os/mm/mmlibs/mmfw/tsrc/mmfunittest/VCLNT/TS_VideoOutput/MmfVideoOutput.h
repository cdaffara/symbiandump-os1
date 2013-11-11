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

#ifndef __MMF_VIDEOOUTPUT_H__
#define __MMF_VIDEOOUTPUT_H__

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

#include "mmfVideoOutputInterfaceUIDs.hrh"
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <mmf/common/mmfhelper.h>
#endif

const TUint KVideoOutputDefaultFrameSize = 0x1000;
const TUid KUidMmfVideoOutput	= {KMmfUidVideoOutputInterface};


void Panic(TInt aPanicCode);

class MMMFVideoOutput : public MDataSink
/*
Interface class to allow dynamic linkage to CMMFVideoOutput.
*/
	{
public:
	//factory function
	inline static MMMFVideoOutput* NewVideoOutputL(TUid aImplementationUid, const TDesC8& aInitData);
	//interface
protected:
	//constructor
	MMMFVideoOutput() : MDataSink(KUidMmfVideoOutput) {}
	};

inline MMMFVideoOutput* MMMFVideoOutput::NewVideoOutputL( TUid aImplementationUid,  const TDesC8& aInitData )
	{
	//make sure the cast will be ok by checking the uid is what we expect
//	__ASSERT_ALWAYS(aImplementationUid==KUidMmfVideoOutput, Panic(EMMFVideoOutputPanicBadUID));
	MMMFVideoOutput* retPtr = STATIC_CAST(MMMFVideoOutput*, MDataSink::NewSinkL(aImplementationUid, aInitData));
	return retPtr;
	}

class CHardwareCodec;

class CMMFVideoOutput : public CBase, public MMMFVideoOutput
/**
The interface into DevSound
**/
	{
public:
	static MDataSink* NewSinkL() ;
	virtual ~CMMFVideoOutput();

	//MDataSink mixin implementations
	virtual TFourCC SinkDataTypeCode(TMediaId /*aMediaId*/); //used by data path MDataSource/Sink for codec matching
	virtual void EmptyBufferL(CMMFBuffer* aBuffer, MDataSource* aSupplier, TMediaId /*aMediaId*/);//called by a MDataSource to empty a buffer
	virtual void BufferFilledL(CMMFBuffer* aBuffer); //called by MDataSource to pass back full buffer to the sink
	virtual TBool CanCreateSinkBuffer();
	virtual CMMFBuffer* CreateSinkBufferL(TMediaId /*aMediaId*/, TBool &aReference);
	virtual TInt SinkThreadLogon(MAsyncEventHandler& aEventHandler);
	virtual void SinkThreadLogoff();
	virtual void SinkPrimeL();
	virtual void SinkPauseL();
	virtual void SinkPlayL();
	virtual void SinkStopL();
	virtual void SetSinkPrioritySettings(const TMMFPrioritySettings& aPrioritySettings);
	TInt State();
	virtual void NegotiateL(MDataSource& aSource);


protected:
	virtual void ConstructSinkL( const TDesC8& aInitData ) ;

private:
	inline CMMFVideoOutput() {}

private:

	MAsyncEventHandler* iEventHandler;

	CMMFDataBuffer* iLastVideoBuffer;
	CMMFDataBuffer* iVideoBufferCopy;

	enum TCodec
		{
		EUseARMCodec=0,
		EUseHWCodec
		};

	enum TState
		{
		EIdle=0,
		EDevSoundReady,
		EMMFDevSoundFirstBufferSent,
		EPlaying,
		EPaused,
		EBufferEmpty
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
	CHardwareCodec* iHWCodec;

	TBool iFirstBufferSent;
	TBool iDevSoundLoaded;

	TError iError;

	TCodec iCodecSelect;
	TMMFPrioritySettings iPrioritySettings;

	TUint iSourceSampleRate;
	TUint iSourceChannels;
	TFourCC iSourceFourCC;
	TBool iNeedsSWConversion;
	CMMFChannelAndSampleRateConverterFactory* iChannelAndSampleRateConverterFactory;
	CMMFChannelAndSampleRateConverter* iChannelAndSampleRateConverter;
	CMMFDataBuffer* iConvertBuffer;
	CMMFBuffer* iBufferToEmpty;
	CMMFDataBuffer* iDevSoundBuffer;
	TUint iDestinationSampleRate;
	TUint iDestinationChannels;
	};

#endif
