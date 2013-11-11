// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __MMF_MP3_FORMAT_H__
#define __MMF_MP3_FORMAT_H__

#include <mmf/server/mmfformat.h>
#include <mmf/server/mmfdatabuffer.h>
#include <mmf/server/mmfclip.h>
#include <ecom/implementationproxy.h>
#include <mmf/common/mmfutilities.h>

#define ID3ChunkName(A,B,C) ((C<<16)+(B<<8)+A)
const TInt KID3ChunkNameHdr = ID3ChunkName('I','D','3');
const TUint KID3ChunkHeaderLength = 10;
const TUint KID3ChunkNameOffset = 0;
const TUint KID3ChunkRes1Offset = 3;
const TUint KID3ChunkRes2Offset = 4;
const TUint KID3ChunkLengthOffset = 6;


#define RiffChunkName(A,B,C,D) ((D<<24)+(C<<16)+(B<<8)+A)
const TInt KRiffChunkNameRiff = RiffChunkName('R','I','F','F');
const TInt KRiffChunkNameList = RiffChunkName('L','I','S','T');
const TInt KRiffChunkNameData = RiffChunkName('d','a','t','a');
const TInt KRiffChunkTypeWave = RiffChunkName('W','A','V','E');
const TInt KRiffChunkNameFact = RiffChunkName('f','a','c','t');
const TInt KRiffChunkNameFmt_ = RiffChunkName('f','m','t',' ');

const TUint KMdaWavMaxHeaderSize = 38;
const TUint KMdaWavFactChunkSize = 4;

const TUint KRiffChunkHeaderLength = 8;
const TUint KRiffChunkDataLength = 4;
const TUint KRiffContainerChunkHeaderLength = KRiffChunkHeaderLength+KRiffChunkDataLength;


const TUint KMMFWavFormatTypePcm = 1;
const TUint KMMFWavFormatTypeMP3 = 85;


const TUint KOneSecondInMicroSeconds = 1000000;
const TInt  KDefineIOBufferSize = 0x0200;	//easiest file size to read?
const TUint KMono = 1;
const TUint KStereo = 2;
const TUint KMaxSampleRate = 96000; //set to 96KHz for now

//this defines the valid sample rates for WAV
const TUint KWavSampleRates[] = { 8000, 11025, 16000, 22050,32000, 44100, 48000, 88200, 96000 };

class TMdaRiffChunk
	{
public:
	TInt32 iName;
	TUint32 iLength;
	TInt32 iType;
	TInt iPosition;
	TBool iFound;
	};

class TMdaID3Chunk
	{
public:
	TInt32 iName;		// first 3 bytes of file = "ID3"
	TUint8  iRes1;
	TUint16 iRes2;
	TInt32 iLength;
	TInt iPosition;
	TBool iFound;
	};

class TMdaMP3Header
	{
public:
	TInt iVer;
	TInt iLayer;
	TInt iProtected;

	TInt iBitRate;
	TInt iSampleRate;
	TInt iPadding;
	TInt iPrivate;

	TInt iStereo;
	TInt iModeExt;
	TInt iCopyright;
	TInt iOriginal;
	TInt iEmphasis;

	TBool iFound;
	};

class CMMFWavFormatRead : public CMMFFormatDecode, public MAsyncEventHandler
	{
public:

	static CMMFFormatDecode* NewL(MDataSource* aSource);
	virtual ~CMMFWavFormatRead();
	virtual TUint Streams(TUid aMediaType) const;
	virtual TTimeIntervalMicroSeconds FrameTimeInterval(TMediaId aMediaType) const;
	virtual TTimeIntervalMicroSeconds Duration(TMediaId aMediaType) const;

	//from MDataSource
	virtual void FillBufferL(CMMFBuffer* aBuffer, MDataSink* aConsumer, TMediaId aMediaId);
	virtual CMMFBuffer* CreateSourceBufferL(TMediaId aMediaId, TBool &aReference); 
	virtual CMMFBuffer* CreateSourceBufferL(TMediaId aMediaId, CMMFBuffer& aSinkBuffer, TBool &aReference);
	virtual TFourCC SourceDataTypeCode(TMediaId aMediaId); //returns FourCC code for the mediaId
	virtual TInt SetSourceDataTypeCode(TFourCC aSourceFourCC, TMediaId aMediaId);
	virtual TInt SourceThreadLogon(MAsyncEventHandler& aEventHandler);
	virtual void SourceThreadLogoff();
	virtual void NegotiateSourceL(MDataSink& aSink);
	virtual void SourcePrimeL();
	virtual void SourcePlayL();
	virtual void SourcePauseL();
	virtual void SourceStopL();

	//From CMMFFormatDecode
	virtual TInt SetNumChannels(TUint aChannels);
	virtual TInt SetSampleRate(TUint aSampleRate);

	virtual void BufferFilledL(CMMFBuffer* aBuffer);

	virtual TUint NumChannels() {return iChannels;};
	virtual TUint SampleRate() {return iSampleRate;};
	virtual TUint BitRate() {return iSampleRate * iBitsPerSample;};
	virtual void GetSupportedSampleRatesL(RArray<TUint>& aSampleRates);
	virtual void GetSupportedNumChannelsL(RArray<TUint>& aNumChannels);
	virtual void GetSupportedDataTypesL(TMediaId aMediaId, RArray<TFourCC>& aDataTypes);

	virtual TBool SourceSampleConvert() {return EFalse;};
	virtual void SuggestSourceBufferSize(TUint aSuggestedBufferSize);
	
	//from MAsyncEventHandler
	virtual TInt SendEventToClient(const TMMFEvent& /*aEvent*/) {return KErrNone;}

protected:
	CMMFWavFormatRead() : iState(EUninitialised){};
private:
	void ConstructL(MDataSource* aSource);
	CMMFDataBuffer* CreateSourceBufferOfSizeL(TUint aSize);
	void DoReadL(TInt aReadPosition);
	void FindRiffChunksL();
	void ReadChunk(TMdaRiffChunk* aChunk);
	void ReadChunk(TMdaID3Chunk* aChunk);
	void ReadChunk(TMdaMP3Header* aChunk);
	void AssignChunkTo(TMdaRiffChunk* aAssignedChunk);
	void ProcessFormatHeaderL();
	void NegotiateSourceBufferL(CMMFBuffer& aBuffer);
	void CalculateFrameTimeInterval();
	void CalculateFrameSize(TUint aSuggestedFrameSize = 0);
	void ScanHeaderL(); 
protected:
	enum TFormatState
		{
		EUninitialised,
		EInitialised
		};
protected:
	MDataSource* iClip; //for decode format MDatasource; for encode format MDataSink
	MDataSink* iDataPath; //for decode format MDataSink; for encode format MDataSource
	TFourCC iFourCC;
	CMMFBuffer* iBufferToEmpty;	
private:
	TFormatState iState;
	CMMFDataBuffer* iBuffer;
	const TUint8* iStartPtr;
	TUint iLastReadPosition;
	TBool iHasFactChunk;
	
	TMdaID3Chunk iCurrentID3Chunk;
	TMdaMP3Header iCurrentMP3Header;
	TBool iRawHeader;

	TMdaRiffChunk iCurrent;
	TMdaRiffChunk iFormatChunk;
	TMdaRiffChunk iFactChunk;
	TMdaRiffChunk iDataChunk;
	TUint iRiffChunkLength;
	TBool iFirstTime;
	TBool iDone;
	TUint iStartPosition;
	TUint iDataLength;
	TUint iPos;
	TUint iChannels;
	TUint iSampleRate;
	TUint iAverageBytesPerSecond;	//needed for correct IMA
	TUint iBlockAlign;				//needed for correct IMA
	TUint iSamplesPerBlock;
	TUint iBitsPerSample;
	TTimeIntervalMicroSeconds iFrameTimeInterval;
	TUint iFrameSize;
	TUint iVbrFilePosition;
	TUint iClipLength;
	TUint iCodecId;
	//for sample rate conversion at source
	TUint iSinkChannels;
	TUint iSinkSampleRate;
	TFourCC iSinkFourCC;
	};


#endif

