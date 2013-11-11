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
// Observer class for CAviWriter
// 
//

#ifndef AVIWRITER_H
#define AVIWRITER_H

#include <mmf/server/mmffile.h>
#include <e32std.h>
#include <e32base.h>
#include <mmf/devvideo/devvideorecord.h>

const TInt KIdx1EntrySize = 16;

class MAviWriterObserver
	{
public:
	virtual void AudioWritten(TBool abVal)=0;
	virtual void BufferEmptied() = 0;
	};

//The main class to write to an.avi file.
class CAviWriter:public CBase,public MDataSource
	{
public:
	static CAviWriter* NewL(CMMFClip& aClip, MAviWriterObserver& aObserver);
	virtual ~CAviWriter();	
	void Reset();
	void SetMaximumClipSizeL(TInt aFileSize);
	void GetRecordTimeAvailableL(TTimeIntervalMicroSeconds& aTime);
	void WriteVideoDataL(TVideoOutputBuffer* aBuffer);
	void WriteAudioDataL(CMMFBuffer* aBuffer);
	void SetDefaultHeaderParametersL();
	void SetVideoFrameRate(TReal32 aFramesPerSecond);
	void SetVideoFrameSize(TSize aFrameSize);
	void SetAudioEnabledL(TBool aAudioEnabled);
	void SetPositionL(TTimeIntervalMicroSeconds aPosition);
	TTimeIntervalMicroSeconds Duration();
	TInt GetAudioBitRate();
	void SetAudioBitRateL(TInt& aBitRate);
		
	//from MDataSource
	void FillBufferL(CMMFBuffer* aBuffer, MDataSink* aConsumer, TMediaId aMediaId);
	void BufferEmptiedL(CMMFBuffer* aBuffer);
	TBool CanCreateSourceBuffer();
	CMMFBuffer* CreateSourceBufferL(TMediaId aMediaId, TBool &aReference);
	TFourCC SourceDataTypeCode(TMediaId aMediaId);
	void ConstructSourceL(const TDesC8& aInitData);
	void UpdateHeadersL();
	
private: 	

	void ConstructL();	
	CAviWriter(CMMFClip& aClip, MAviWriterObserver& aObserver);
	TUint HdrlLen();
	void BeforeDataWriteL(TUint8 aMediaIdx);
	void AfterDataWriteL(TUint8 aMediaIdx,TUint aDataLen);
	void OnRecordCompleteL();
	TUint WriteJunk(TUint aBytes);
	void WriteHeaderL();
	void SetNumberOfStreamsL();
	void SetDefaultAudioHeaders();
	void SetDefaultVideoHeaders();
	TInt ReadFormatL();
		
private:
	MAviWriterObserver& iObserver;
  	CMMFClip* iClip;
  	CMMFDescriptorBuffer* iSourceBuffer;
  	CMMFDescriptorBuffer* iReadBuffer;
  	
  	//Identifies if this is a new 'rec'  
  	TBool iBnewRec;
	
	//Updates the size of 'Movi' at the end.
	TInt iMoviPos;
	
	//Reflects size of the movi chunk
	TUint iMoviSize;
	TInt iMaxClipSize;
  	TUint iOffset;
  	TInt iCurrentClipPosition;
  	TInt iDataWritten[2];
  	TInt iRecLen;
  	TUint8* iBufPtr;
	RBuf8 iBufIdx1;
	TUint8 iBufIdx1Entry[KIdx1EntrySize];

  	//Total number of Frames in the file
  	TInt iFrameCount;			
  	TBool iClipAlreadyExists;
  	TBool iAudioEnabled;
  	
	//Main Avi header
	struct TAviMainHeader	
		{
		TUint	iMicroSecPerFrame; 
		TUint	iMaxBytesPerSec;
		TUint	iPaddingGranularity;
		TUint	iFlags;
		TUint	iTotalFrames;
		TUint	iInitialFrames;
		TUint	iStreams;	
		TUint	iSuggestedBufferSize;
		TUint	iWidth;
		TUint	iHeight;
		TUint	iReserved[4];
		} iMainHeader;

	//Stream header applicable to both audio and video	
	struct TAviStreamHeader	
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
		TUint	iQuality;
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

	//Audio Stream format
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
		} iPcmWaveFormat;
		

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
		} iBitmapInfoHeader;
	};

#endif
