// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef AVIREADER_H
#define AVIREADER_H


#include <mmf/server/mmffile.h>
#include <e32std.h>
#include <e32base.h>
#include <mmf/devvideo/devvideoplay.h>


//Observer class for CAviReader
class MAviReaderObserver
	{
public:
	virtual void AudioBufferFilled() = 0;
	virtual void VideoBufferFilled(TVideoInputBuffer* aBuffer) = 0;
	};


//The main class to read from an.avi file.
class CAviReader:public CBase,public MDataSink
	{
public:
	static CAviReader* NewL(CMMFClip& aClip,MAviReaderObserver& aObserver);
	virtual ~CAviReader();
	TTimeIntervalMicroSeconds Duration() const ;
	void SetPositionL(TInt aPosition);
	void ResetL();
	void SetPlayWindowL(TTimeIntervalMicroSeconds aStart,TTimeIntervalMicroSeconds aEnd);
	void FillVideoBufferL(TVideoInputBuffer* aBuffer);
	void FillAudioBufferL(CMMFBuffer* aBuffer);  	  	
  	void ReadNextFrameL(TUid aMediaType, TInt& aPosition);
  	TBool ReadNextFrameStartL(TInt& aStart);
  	TBool IsValidBlock(TUint aChunkId);
	TBool IsVideoInputEnd();
	TBool IsAudioInputEnd();
	void ReadComplete();
  	void AudioEnabled(TBool& aEnabled);
	void FrameRate(TReal32& aFramesPerSecond);
	void AudioCodec(TFourCC& aCodec);
	TInt SampleRate();
	void VideoMimeType(TDes8& aMimeType);
	void VideoFrameSize(TSize& aSize);
	TInt Channels();
	TInt BitsPerSample();
	TBool IsAudioDataAvailableL(TInt aStartPos);
			
    //MDataSink
	TFourCC SinkDataTypeCode(TMediaId aMediaId);
	void EmptyBufferL(CMMFBuffer* aBuffer, MDataSource* aSupplier, TMediaId aMediaId);
	void BufferFilledL(CMMFBuffer* aBuffer);
	TBool CanCreateSinkBuffer();
	CMMFBuffer* CreateSinkBufferL(TMediaId aMediaId, TBool& aReference);
	void ConstructSinkL( const TDesC8& aInitData);
private:
	
	//Main Avi Header
	struct TAviMainHeader	
		{
		TUint	iMicroSecPerFrame; 
		TUint	iMaxBytesPerSec;
		TUint   iPaddingGranularity;
		TUint	iFlags;
		TUint	iTotalFrames;
		TUint	iInitialFrames;
		TUint	iStreams;	
		TUint	iSuggestedBufferSize;
		TUint	iWidth;
		TUint	iHeight;
		TUint   iReserved[4];
		} iMainHeader;
	
	struct TAviStreamHeader	//represents stream header
		{
		TUint	iFccType;
		TUint	iFccHandler;
		TUint	iFlags;	
		TUint	iReserved;
		TUint	iInitialFrames;
		TUint	iScale;
		TUint	iRate;		
		TUint	iStart;
		TUint	iLength;	
		TUint	iSuggestedBufferSize;
		TUint   iQuality;
		TUint	iSampleSize;
		struct
			{
			TInt16 iLeft;
			TInt16 iTop;
			TInt16 iRight;
			TInt16 iBottom;
			}iRcFrame;
		};
	TAviStreamHeader iStreamHeader[2];
	
    struct TPcmWaveFormat
		{
		struct TWaveFormat
             { 
             TUint16  iFormatTag; 
             TUint16  iNChannels; 
             TUint  iNSamplesPerSec; 
             TUint  iNAvgBytesPerSec; 
             TUint16  iNBlockAlign;
             } iWaveFormat;
        TUint16 iBitsPerSample;
        } iPCMWaveFormat;

    //Video Stream Format


	struct TBitmapInfoHeader
		{
		TUint  iBiSize;
        TUint   iBiWidth;
        TUint   iBiHeight;
        TUint16   iBiPlanes;
        TUint16   iBiBitCount;
		TUint  iBiCompression;
		TUint  iBiSizeImage;
		TUint   iBiXPelsPerMeter;
		TUint   iBiYPelsPerMeter;
		TUint  iBiClrUsed;
		TUint  iBiClrImportant;
		}iBitmapInfoHeader;
	
private:
	TInt ReadFormatL();
  	void ProcessFormatL();
 	TInt FindRiffChunksL();
 	CAviReader(CMMFClip& aClip, MAviReaderObserver& aObserver);
  	void ConstructL();
  	void DoReadL(TInt aReadPosition);
  	void SetMediaEOS(TUid aMediaType);
private:
	MAviReaderObserver& iObserver;
	CMMFClip& iClip;
	CMMFDescriptorBuffer* iSourceBuffer;
	CMMFDescriptorBuffer* iAudioBuffer;
	CMMFDescriptorBuffer* iVideoBuffer;
	TInt iNumberOfStreams;
	TBool iAudioEos;
	TBool iVideoEos;	
	TInt iSourcePos;
	TInt iAudioPos;
	TInt iVideoPos;
	TUint8* iClipPos;
	TBool iAudioEnabled;
	TVideoInputBuffer* iBufferFromDevVideo;
	CMMFBuffer* iBufferFromDevSound;
	TBool iVideoRequestMade;
	TBool iAudioRequestMade;
	TBool iReadCompleted;
	TBool iReadRequestMade;
	TInt iFileSize;
	TInt iAudioIdx;
	TInt iVideoIdx;
	};
#endif
