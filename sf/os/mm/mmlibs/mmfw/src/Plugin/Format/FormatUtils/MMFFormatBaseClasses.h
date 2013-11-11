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

#ifndef __MMF_FORMAT_BASECLASSES_H__
#define __MMF_FORMAT_BASECLASSES_H__

#include <mmf/server/mmfformat.h>
#include <mmf/server/mmfdatabuffer.h>
#include <mmf/server/mmfclip.h>
#include <ecom/implementationproxy.h>
#include <mmf/common/mmfutilities.h>

#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <mmf/common/mmfhelper.h>
#endif

const TInt  KDefineIOBufferSize = 512 ;
const TUint KOneSecondInMicroSeconds = 1000000;
const TUint KMono(1);
const TUint KStereo(2);

//this defines the valid sample rates
const TUint KMMFFormatSampleRates[] = { 8000, 11025, 12000, 16000, 22050, 24000, 32000, 44100, 48000, 64000, 88200, 96000};

const TUint KMMFFormatDefaultSampleRate = 8000;
const TUint KMMFFormatDefaultBitsPerSample = 16;
const TUint KMMFFormatDefaultChannels = KMono;



class CMMFFormatRead : public CMMFFormatDecode2, public MAsyncEventHandler
	{
public:

	IMPORT_C virtual ~CMMFFormatRead();

	//from MDataSource
	IMPORT_C virtual void FillBufferL(CMMFBuffer* aBuffer, MDataSink* aConsumer, TMediaId aMediaId);
	IMPORT_C virtual CMMFBuffer* CreateSourceBufferL(TMediaId aMediaId, TBool &aReference); 
	IMPORT_C virtual CMMFBuffer* CreateSourceBufferL(TMediaId aMediaId, CMMFBuffer& aSinkBuffer, TBool &aReference);
	IMPORT_C virtual TFourCC SourceDataTypeCode(TMediaId aMediaId); //returns FourCC code for the mediaId
	IMPORT_C virtual TInt SetSourceDataTypeCode(TFourCC aSourceFourCC, TMediaId aMediaId);
	IMPORT_C virtual TInt SourceThreadLogon(MAsyncEventHandler& aEventHandler);
	IMPORT_C virtual void SourceThreadLogoff();
	IMPORT_C virtual void NegotiateSourceL(MDataSink& aSink);
	IMPORT_C virtual void SourcePrimeL();
	IMPORT_C virtual void SourcePlayL();
	IMPORT_C virtual void SourcePauseL();
	IMPORT_C virtual void SourceStopL();

	//from MDataSink - CMMFFormatDecode is a MDataSink to a CMMFClip
	IMPORT_C virtual void BufferFilledL(CMMFBuffer* aBuffer);


	//From CMMFFormatDecode
	IMPORT_C virtual TUint Streams(TUid aMediaType) const;
	IMPORT_C virtual TTimeIntervalMicroSeconds FrameTimeInterval(TMediaId aMediaType) const;
	IMPORT_C virtual TTimeIntervalMicroSeconds Duration(TMediaId aMediaType) const;
	IMPORT_C virtual TInt SetNumChannels(TUint aChannels);
	IMPORT_C virtual TInt SetSampleRate(TUint aSampleRate);
	inline virtual TUint NumChannels();
	inline virtual TUint SampleRate();
	inline virtual TUint BitRate();
	IMPORT_C virtual void GetSupportedSampleRatesL(RArray<TUint>& aSampleRates);
	IMPORT_C virtual void GetSupportedNumChannelsL(RArray<TUint>& aNumChannels);
	IMPORT_C virtual void SetPositionL(const TTimeIntervalMicroSeconds& aPosition);
	IMPORT_C virtual TTimeIntervalMicroSeconds PositionL();

	//From CMMFFormatDecode2
	IMPORT_C virtual TAny* CustomInterface(TUid iInterfaceId);
	
	// from MAsyncEventHandler
	IMPORT_C virtual TInt SendEventToClient(const TMMFEvent& aEvent);



	//New virtual methods for CMMFFormatRead
	inline virtual TUint MaxNumChannels();
	inline virtual TBool SourceSampleConvert();



protected:
	inline CMMFFormatRead();

	//New virtual methods for CMMFFormatRead
	IMPORT_C virtual void ConstructL(MDataSource* aSource);
	virtual void ProcessFormatHeaderL() =0;
	IMPORT_C virtual void NegotiateSourceBufferL(CMMFBuffer& aBuffer);
	IMPORT_C virtual void CalculateFrameTimeInterval();
	IMPORT_C virtual void CalculateFrameSize(TUint aSuggestedFrameSize = 0);
	inline virtual TInt OKToPlay();

	IMPORT_C CMMFDataBuffer* CreateSourceBufferOfSizeL(TUint aSize);
	IMPORT_C void DoReadL(TInt aReadPosition);



protected:
	enum TFormatState
		{
		EUninitialised,
		EInitialised
		};

protected:
	MDataSource* iClip; //clip containing data. Not owned
	MDataSink* iDataPath; //Not owned
	TFourCC iFourCC;	
	TBool iNeedsSWConversion;
	CMMFChannelAndSampleRateConverterFactory* iChannelAndSampleRateConverterFactory;
	CMMFChannelAndSampleRateConverter* iChannelAndSampleRateConverter; 
	CMMFDataBuffer* iConvertBuffer;
	TUint iConvertBufferSize;
	CMMFBuffer* iBufferToEmpty;	

	TFormatState iState;
	CMMFDataBuffer* iBuffer;
	

	TUint iStartPosition;  //where data is to be read from
	TUint iDataLength;     //the length of the data portion of the file
	TUint iHeaderLength;


	TUint iChannels;
	TUint iSampleRate;
	TUint iBitsPerSample;
	TTimeIntervalMicroSeconds iFrameTimeInterval;

	TUint iFrameSize;
	TUint iFilePosition;	//the current position where a read will occur from
	TUint iClipLength;		//the total length of the file

	//for sample rate conversion at source
	TUint iSinkChannels;
	TUint iSinkSampleRate;
	TFourCC iSinkFourCC;

	MAsyncEventHandler* iAsyncEventHandler; //Not owned
	};


class CMMFFormatWrite : public CMMFFormatEncode, public MAsyncEventHandler
	{
public:
	IMPORT_C virtual ~CMMFFormatWrite();
	IMPORT_C virtual TTimeIntervalMicroSeconds FrameTimeInterval(TMediaId aMediaType) const;
	IMPORT_C virtual TTimeIntervalMicroSeconds Duration(TMediaId aMediaType) const;

	//from MDataSink
	IMPORT_C virtual CMMFBuffer* CreateSinkBufferL(TMediaId aMediaId, TBool &aReference); 
	IMPORT_C virtual TFourCC SinkDataTypeCode(TMediaId aMediaId); //returns FourCC code for the mediaId
	virtual TInt SetSinkDataTypeCode(TFourCC aSinkFourCC, TMediaId aMediaId) = 0; 
	IMPORT_C virtual TInt SinkThreadLogon(MAsyncEventHandler& /*aEventHandler*/);
	IMPORT_C virtual void SinkThreadLogoff();
	virtual void NegotiateL(MDataSource& aSource) = 0;
	IMPORT_C virtual void SinkPrimeL();
	IMPORT_C virtual void SinkPlayL();
	IMPORT_C virtual void SinkPauseL();
	IMPORT_C virtual void SinkStopL();


	//from MDataSink - CMMFFormatDecode is a MDataSink to a CMMFClip
	IMPORT_C virtual void EmptyBufferL(CMMFBuffer* aBuffer, MDataSource* aSupplier, TMediaId aMediaId);
	IMPORT_C virtual void BufferEmptiedL(CMMFBuffer* aBuffer);
	IMPORT_C virtual void SetPositionL(const TTimeIntervalMicroSeconds& aPosition);
	IMPORT_C virtual TTimeIntervalMicroSeconds PositionL();

	IMPORT_C virtual TInt SetNumChannels(TUint aChannels);
	inline virtual TUint NumChannels();

	inline virtual TUint SampleRate();
	IMPORT_C virtual TInt SetSampleRate(TUint aSampleRate);

	inline virtual TUint GetDefaultSampleRate();
	inline virtual TUint BitRate();
	IMPORT_C virtual TInt64 BytesPerSecond()  ;

	IMPORT_C virtual void GetSupportedSampleRatesL(RArray<TUint>& aSampleRates);
	IMPORT_C virtual void GetSupportedNumChannelsL(RArray<TUint>& aNumChannels);
	virtual void GetSupportedDataTypesL(TMediaId aMediaId, RArray<TFourCC>& aDataTypes) = 0;

	IMPORT_C virtual void SetMaximumClipSizeL(TInt aBytes);
	inline virtual TInt MaximumClipSize();

	IMPORT_C virtual void CropL(TTimeIntervalMicroSeconds aPosition, TBool aToEnd );

	// from MAsyncEventHandler
	IMPORT_C virtual TInt SendEventToClient(const TMMFEvent& aEvent);


	//New virtual methods for CMMFFormatWrite
	inline virtual TUint MaxNumChannels();


protected:
	inline CMMFFormatWrite();

	//New virtual methods for CMMFFormatWrite
	IMPORT_C virtual void ConstructL(MDataSink* aSink);
	virtual TInt DetermineIfValidClip() = 0;
	virtual void SetDefaultHeaderParametersL() = 0;
	virtual void ProcessFormatHeaderL() = 0;
	virtual void WriteHeaderL() =0;
	IMPORT_C virtual void CalculateFrameTimeInterval();
	IMPORT_C virtual void CalculateFrameSize(TUint aSuggestedFrameSize = 0);
	IMPORT_C virtual TBool IsMaxClipLengthExceeded( const CMMFBuffer& aBuffer );
	IMPORT_C virtual TBool IsValidSampleRate(TUint aSampleRate );

	//This is called in SinkPrimeL to ensure it is OK to record to the clip.
	//This allows a Client to open a file in the record Utility but then only play it.
	//eg. GSM610 can be opened to playback, but it cannot be appended to.
	inline virtual TInt OKToRecord();



	IMPORT_C CMMFDataBuffer* CreateSinkBufferOfSizeL(TUint aSize);
	IMPORT_C void DoReadL(TInt aReadPosition);
	IMPORT_C void DoWriteL(TInt aWritePosition);


protected:
	enum TFormatState
		{
		EUninitialised,
		EInitialised
		};

protected:
	MDataSink* iClip; //Clip to record to. Not owned
	MDataSource* iDataPath; //Not owned
	TFourCC iFourCC;	

	TFormatState iState;
	CMMFDataBuffer* iBuffer;
	TBool iClipAlreadyExists;
	TBool iFileHasChanged;
	TUint iStartPosition;
	TUint iDataLength;
	TUint iFilePosition;

	TUint iHeaderLength;
	TUint iChannels;
	TUint iSampleRate;
	TUint iSourceChannels;
	TUint iSourceSampleRate;
	TFourCC iSourceFourCC;
	TUint iBitsPerSample;
	TTimeIntervalMicroSeconds iFrameTimeInterval;
	TUint iFrameSize;
	TUint iClipLength;

	TBool iHeaderUpdated;
	TInt iInitialFileSize;

	CMMFChannelAndSampleRateConverterFactory* iChannelAndSampleRateConverterFactory;
	CMMFChannelAndSampleRateConverter* iChannelAndSampleRateConverter; 
	CMMFDataBuffer* iConvertBuffer;
	TUint iConvertBufferSize;
	CMMFBuffer* iBufferToEmpty; //pointer to store buffr to empty - needed if buffer passed to clip is different
	TBool iConverterFrameSizeSet; //if sample rate convertion is used a new frm size needs to be set
	TBool iSourceWillSampleConvert;

	MAsyncEventHandler* iAsyncEventHandler; //Not owned

	TInt iMaximumClipSize;
	};


#include "MMFFormatBaseClasses.inl"

#endif

