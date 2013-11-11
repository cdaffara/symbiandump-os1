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

#include <ecom/ecom.h>
#include <mmf/plugin/mmfformatimplementationuids.hrh>
#include <mmf/common/mmffourcc.h>
#include "formatUtils.h"
#include "mmfmp3format.h"
#include <mmf/server/mmffile.h>

#include "MP3Uids.hrh"



/**
*
* Read16
*  fast read for non-aligned 16-bit data
*/
LOCAL_C TUint16 Read16(const TUint8* aPtr)

    {
	TUint16 ret = *(REINTERPRET_CAST(const TUint16*, aPtr));
	return ret;
    }

/**
* Read32
* @param aPtr
* fast read for non-aligned little-endian 32-bit data
* @xxxx
*/
LOCAL_C TUint32 Read32(const TUint8* aPtr)
    {
    TUint32 x = *aPtr++;
    x |= *aPtr++ << 8;
    x |= *aPtr++ << 16;
    x |= *aPtr++ << 24;
    return x;
    }

/**
* Read24
* @param aPtr
* fast read for non-aligned little-endian 24-bit data
* @xxxx
*/
LOCAL_C TUint32 Read24(const TUint8* aPtr)
    {
    TUint32 x = *aPtr++;
    x |= *aPtr++ << 8;
    x |= *aPtr++ << 16;
    return x;
    }

/**
*
* NewL
* @param aSource
* @return CMMFFormatDecode* 
* 
*/
CMMFFormatDecode* CMMFWavFormatRead::NewL(MDataSource* aSource)
	{
	if ((aSource->DataSourceType()==KUidMmfDescriptorSource)||
			(aSource->DataSourceType()==KUidMmfFileSource))
		{//currently only files and descriptor MDataSources are supported
		CMMFWavFormatRead* self = new(ELeave)CMMFWavFormatRead;
		CleanupStack::PushL(self);
		self->ConstructL(aSource);
		CleanupStack::Pop(self);
		return STATIC_CAST( CMMFFormatDecode*, self );
		}
	else 
		{
		User::Leave(KErrNotSupported);
		return NULL;
		}
	}

/**
*
* ~CMMFWavFormatRead
*
*/
CMMFWavFormatRead::~CMMFWavFormatRead()
	{
	delete iBuffer;
	}
	
/**
*
* ConstructL
* @param aSource
*
*/
void CMMFWavFormatRead::ConstructL(MDataSource* aSource)
	{
	iClip = aSource;
	User::LeaveIfError(iClip->SourceThreadLogon(*this));
	TCleanupItem threadCleanupItem(DoDataSourceThreadLogoff, iClip);
	CleanupStack::PushL(threadCleanupItem);
	
	iClip->SourcePrimeL();
	TCleanupItem srcCleanupItem(DoDataSourceStop, iClip);
	CleanupStack::PushL(srcCleanupItem);
	
	ScanHeaderL(); 
	
	iState = EInitialised;
	
	CleanupStack::Pop(iClip);
	iClip->SourceStopL();
	
	CleanupStack::Pop(iClip);  
	iClip->SourceThreadLogoff();
	}


/**
*
* ScanHeaderL
*
* Method to scan the header of a wav file and sets the audio setting data members
* iSampleRate, iChannels, iClipSize
*
* @leave "This method may leave with one of the system wide error codes"
* PAC
*/
void CMMFWavFormatRead::ScanHeaderL()
	{
	iDone = EFalse;
	iFirstTime = ETrue;
	delete iBuffer;
	iBuffer = NULL;
	iBuffer = CreateSourceBufferOfSizeL(KDefineIOBufferSize);
	DoReadL(0);

	while (!iDone) 
		FindRiffChunksL();

	ProcessFormatHeaderL();
	delete iBuffer;
	iBuffer = NULL;
	}

/**
*
* SourceThreadLogon
* @param aEventHandler
*
*/
TInt CMMFWavFormatRead::SourceThreadLogon(MAsyncEventHandler& aEventHandler)
	{//pass through to source clip
	return(iClip->SourceThreadLogon(aEventHandler));
	}

/**
*
* SourcePrimeL
*
*/
void CMMFWavFormatRead::SourcePrimeL()
	{
	iVbrFilePosition = 0;
	iClip->SourcePrimeL();
	}

/**
*
* SourcePlayL
*
*/
void CMMFWavFormatRead::SourcePlayL()
	{
	iClip->SourcePlayL();
	}

/**
*
* SourcePauseL
*
*/
void CMMFWavFormatRead::SourcePauseL()
	{
	iClip->SourcePauseL();
	}

/**
*
* SourceStopL
*
*/
void CMMFWavFormatRead::SourceStopL()
	{
	iVbrFilePosition = 0;
	iClip->SourceStopL();
	}

/**
*
* SourceThreadLogoff
*
*/
void CMMFWavFormatRead::SourceThreadLogoff()
	{
	iClip->SourceThreadLogoff();
	}

/**
*
* Streams
* @param aMediaType
* @returns TUint
*
*/
TUint CMMFWavFormatRead::Streams(TUid aMediaType) const
	{
	//need to check aMediaType for audio
	if (aMediaType == KUidMediaTypeAudio) 
		return 1; //wav files can only have 1 audio stream
	else
		return 0;
	}

/**
*
* FrameTimeInterval
* @param  aMediaId
* @returns TTimeIntervalMicroSeconds
*
*/
TTimeIntervalMicroSeconds CMMFWavFormatRead::FrameTimeInterval(TMediaId aMediaId) const
	{
	if (aMediaId.iMediaType == KUidMediaTypeAudio) 
		return iFrameTimeInterval;
	else
		return TTimeIntervalMicroSeconds(0);
	}

/**
*
* NegotiateSourceL
* @param aSink
* 
*/
void CMMFWavFormatRead::NegotiateSourceL(MDataSink& /*aSink*/)
	{
	User::Leave(KErrNotSupported);
	}
/**
*
* Duration
* @param aMediaId
* @returns TTimeIntervalMicroSeconds
*
*/

TTimeIntervalMicroSeconds CMMFWavFormatRead::Duration(TMediaId aMediaId) const
	{
	TTimeIntervalMicroSeconds duration(0); //default
	if (aMediaId.iMediaType == KUidMediaTypeAudio) 
		{
		duration = DurationCalculation(iDataLength, iSampleRate, iBitsPerSample, iChannels);
		}

	return duration;
	}
/**
*
* NegotiateSourceBufferL
* @param aSinkBuffer
* 
*/	
void CMMFWavFormatRead::NegotiateSourceBufferL(CMMFBuffer& aSinkBuffer)
	{
	if (!(&aSinkBuffer))	//asychronous buffers may not be initialised
		return;	
	
	// if sink buffer has a fixed size use this to determine source buffer size
	// else if sink buffer size is not specified then set to default buffer size?
	if (aSinkBuffer.Type() == KUidMmfDataBuffer)
		{
		TUint sinkBufferLength = STATIC_CAST(CMMFDataBuffer&, aSinkBuffer).Data().MaxLength();
		CalculateFrameSize(sinkBufferLength); 
		}
	else
		User::Leave(KErrNotSupported);	
	}

/**
*
* CalculateFrameTimeInterval
* 
*/
void CMMFWavFormatRead::CalculateFrameTimeInterval()
	{
	iFrameTimeInterval = FrameTimeIntervalCalculation(iFrameSize, iSampleRate, iBitsPerSample, iChannels);
	}

/**
*
* CalculateFrameSize
* 
*/
void CMMFWavFormatRead::CalculateFrameSize(TUint aSuggestedFrameSize)
	{
	TUint oldFrameSize = iFrameSize;

	if (aSuggestedFrameSize)
		iFrameSize = aSuggestedFrameSize;
	else
		iFrameSize = FrameSizeCalculation(iSampleRate, iBitsPerSample, iChannels);

	if (iFrameSize != oldFrameSize || iFrameTimeInterval.Int64() == 0)
		CalculateFrameTimeInterval();
	}


/**
*
* SetSourceDataTypeCode
* @param aSinkFourCC
* @param aMediaId
* @returns TInt
*
*/
TInt CMMFWavFormatRead::SetSourceDataTypeCode(TFourCC aSinkFourCC, TMediaId aMediaId)
	{
	TInt err = KErrNone;
	if (aMediaId.iMediaType != KUidMediaTypeAudio) 
		err = KErrNotSupported;
	else 
		if (iFourCC != aSinkFourCC) 
			err = KErrAlreadyExists; //can't set fourCC if clip exists - it already has a code
		else
			err = KErrNone;
		
	return err;
	}


/**
* SetNumChannels
* @param aChannels
* @returns TInt
*/
TInt CMMFWavFormatRead::SetNumChannels(TUint aChannels)
	{
	TInt err = KErrNone;
	if (iChannels != aChannels) 
		err = KErrAlreadyExists; //can't set fourCC if clip exists - it already has a code
	return err;
	}

/**
* SetSampleRate
* @param aSampleRate
* @returns TInt
*/
TInt CMMFWavFormatRead::SetSampleRate(TUint aSampleRate)
	{
	TInt err = KErrNone;
	if (iSampleRate != aSampleRate) 
		err = KErrAlreadyExists; //can't set fourCC if clip exists - it already has a code
	return err;
	}

/**
*
* SuggestSourceBufferSize
* @param aSuggestedBufferSize
* 
*/
void CMMFWavFormatRead::SuggestSourceBufferSize(TUint /*aSuggestedBufferSize*/)
	{
	}

//from MDataSource
/**
*
* FillBufferL
* @param aBuffer
* @param aConsumer
* @param aMediaId
*
*/
void CMMFWavFormatRead::FillBufferL(CMMFBuffer* aBuffer, MDataSink* aConsumer, TMediaId aMediaId )
	{
	//since wav always contains linear audio the sink buffer size can set the source buffer size
	//there needs to be a separate buffer negotiating phase but for now put it here
	//the code assumes the sink buffer size is constant
	//the sink buffer size will be used to determine the frame time.
	//check media id &pass onto clip
	if (aMediaId.iMediaType != KUidMediaTypeAudio)
		User::Leave(KErrNotSupported);
	
	iDataPath = aConsumer;	

	//Started playing the data. FrameNumber may not be 1 as position may have been set.
	if (!iVbrFilePosition)
		{
		//use the first buffer to control the framesize
		TUint frameSize;
		if (STATIC_CAST(CMMFDataBuffer*,aBuffer)->RequestSize() <= 0)
			{
			frameSize = STATIC_CAST(CMMFDataBuffer*,aBuffer)->Data().MaxLength();
			}
		else
			{
			frameSize = STATIC_CAST(CMMFDataBuffer*,aBuffer)->RequestSize();
			}

		CalculateFrameSize(frameSize);
		}

	iVbrFilePosition = ((aBuffer->FrameNumber()-1)*iFrameSize)+iStartPosition;

	// WAV files can contain extra chunks after the data, so
	// look at iDataLength (the size of the data not the whole file)
	// and see if we've reached the end. The last buffer flag will be 
	// set when we receive this data.
	if (iVbrFilePosition + iFrameSize > iStartPosition + iDataLength)
		{
		TUint lastBufferLen = (iStartPosition + iDataLength) - iVbrFilePosition;
		STATIC_CAST(CMMFClip*, iClip)->ReadBufferL(lastBufferLen, aBuffer, iVbrFilePosition, this);
		}
	//if RequestSize is set, use API that accepts this
	else if (aBuffer->RequestSize())
		{
		STATIC_CAST(CMMFClip*, iClip)->ReadBufferL(aBuffer->RequestSize(), aBuffer, iVbrFilePosition, this);
		}
	
	else
		STATIC_CAST(CMMFClip*, iClip)->ReadBufferL(aBuffer, iVbrFilePosition, this);
	
	}

/**
*
* CreateSourceBufferOfSizeL
* @param aSize
* @returns CMMFDataBuffer* 
*
*/
CMMFDataBuffer* CMMFWavFormatRead::CreateSourceBufferOfSizeL(TUint aSize)
	{
	//needs to create source buffer
	CMMFDataBuffer* buffer = CMMFDataBuffer::NewL(aSize);
	buffer->Data().FillZ(aSize);
	return buffer;
	}

/**
*
* CreateSourceBufferL
* @param aMediaId
* @param aReference
*
*/
CMMFBuffer* CMMFWavFormatRead::CreateSourceBufferL(TMediaId aMediaId, TBool &aReference)
	{
	if (aMediaId.iMediaType == KUidMediaTypeAudio) 
		{
		aReference = EFalse;
		CalculateFrameSize();
		return CreateSourceBufferOfSizeL(iFrameSize);
		}
	else 
		{
		User::Leave(KErrNotSupported);
		return NULL;
		}
	}

/**
*
* CreateSourceBufferL
* @param aMediaId
* @param aSinkBuffer
* @param aReference
*
*/
CMMFBuffer* CMMFWavFormatRead::CreateSourceBufferL(TMediaId aMediaId, CMMFBuffer& aSinkBuffer, TBool &aReference)
	{
	if (aMediaId.iMediaType == KUidMediaTypeAudio) 
		{
		NegotiateSourceBufferL(aSinkBuffer); //sets frame size to match sink buffer
		return CreateSourceBufferL(aMediaId, aReference);
		}
	else 
		{
		User::Leave(KErrNotSupported);
		return NULL;
		}
	}

/**
*
* SourceDataTypeCode
* @param aMediaId
*
*/
TFourCC CMMFWavFormatRead::SourceDataTypeCode(TMediaId aMediaId)
	{
	if (aMediaId.iMediaType == KUidMediaTypeAudio) 
		return iFourCC;
	else 
		return TFourCC(); //defaults to 'NULL' fourCC
	}
/**
*
* DoReadL
* @param aReadPosition
*
*/
void CMMFWavFormatRead::DoReadL(TInt aReadPosition)
	{
	STATIC_CAST(CMMFClip*, iClip)->ReadBufferL(iBuffer,aReadPosition);
	}

/**
*
* FindRiffChunksL
*
*/
void CMMFWavFormatRead::FindRiffChunksL()
	{
	if (iFirstTime)
		{
		iFirstTime=EFalse;
		iStartPtr=iBuffer->Data().Ptr();
		iPos=0;
		iLastReadPosition=0;
		iDone=EFalse;
		}
	else
		{//We've done another read. If there's < chunk in the buffer then something's wrong
		if (iBuffer->Data().Length() < STATIC_CAST(TInt, KRiffChunkHeaderLength))
			{
			if ((iFormatChunk.iFound) && (iDataChunk.iFound)) 
				{
				iDone = ETrue;
				return;
				}
			else
				{
				User::Leave(KErrCorrupt);
				}
			}
		}
	
	// raw header ?
	const TUint8* aPtr = iStartPtr + 0;
	TInt rawHdr = ((*aPtr) << 8) + *(aPtr+1);
	if ((rawHdr > 0xFFF0) && (rawHdr != 0xFFFF))
		{
		iRawHeader = ETrue;
		iClipLength = KMaxTInt;
		iDone = ETrue;
		}
	else
		{
		//look for an ID3 tag first
		ReadChunk(&iCurrentID3Chunk);
		if (iCurrentID3Chunk.iName == KID3ChunkNameHdr)
			{
			iClipLength = KMaxTInt;
			// skip past 4 bytes header & tag
			iPos+= iCurrentID3Chunk.iPosition + iCurrentID3Chunk.iLength;
			iDone = ETrue;
			}
		}

	while (!iDone)
		{
		TInt advance=0;
		
		ReadChunk(&iCurrent);
		
		if (iCurrent.iName == KRiffChunkNameRiff)//we need to look INSIDE the RIFF chunk
			{
			if(iBuffer->Data().Length() < STATIC_CAST(TInt, KRiffContainerChunkHeaderLength))
				User::Leave(KErrCorrupt);
			iRiffChunkLength=iCurrent.iLength + KRiffChunkHeaderLength;
			iClipLength = iRiffChunkLength;
			advance=KRiffContainerChunkHeaderLength;
			}
		else
			{
			advance=iCurrent.iLength + KRiffChunkHeaderLength;		//... and skip all others
			}
		
		if (iCurrent.iName == KRiffChunkNameFmt_)
			AssignChunkTo(&iFormatChunk);
		
		else if (iCurrent.iName == KRiffChunkNameFact)
			AssignChunkTo(&iFactChunk);
		
		else if (iCurrent.iName == KRiffChunkNameData)
			AssignChunkTo(&iDataChunk);
		
		if (iDataChunk.iFound && iFormatChunk.iFound && iFactChunk.iFound)
			{
			iDone=ETrue;
			}
		else
			{//still have chunks to look for
			iPos+=advance;
			if (iPos & 1)
				iPos++;
			
			if ((TUint)iPos>=(TUint)iRiffChunkLength)
				{
				iDone=ETrue;//end of file
				}
			else
				{//make sure we have at least a chunk's worth left in the buffer
				if ((TUint)(iPos-iLastReadPosition) > 
					(TUint)(iBuffer->Data().Length() -KRiffChunkHeaderLength))
					{
					iLastReadPosition=iPos;
					DoReadL(iLastReadPosition);
					return;
					}	
				}
			}
		}
	

	if (iClipLength == 0)
		{
		User::Leave(KErrNotSupported);
		}
	
	if (!(iDataChunk.iFound && iFormatChunk.iFound) && 
		!iCurrentID3Chunk.iFound &&
		!iRawHeader)
		{
		User::Leave(KErrCorrupt);
		}

	}

/**
*
* AssignChunkTo
* @param aAssignedChunk
*
*/
void CMMFWavFormatRead::AssignChunkTo(TMdaRiffChunk* aAssignedChunk)
	{
	Mem::Copy(REINTERPRET_CAST(TUint8*, aAssignedChunk), REINTERPRET_CAST(TUint8*, &iCurrent),sizeof(TMdaRiffChunk));
	aAssignedChunk->iFound=ETrue;
	}

/**
*
* ReadChunk
* @param aChunk
*
*/
void CMMFWavFormatRead::ReadChunk(TMdaID3Chunk* aChunk)
	{
	Mem::FillZ(REINTERPRET_CAST(TUint8*, aChunk),sizeof(TMdaID3Chunk)); // Zero data
	aChunk->iPosition=iPos + KID3ChunkHeaderLength;
	aChunk->iName = Read24(iStartPtr + iPos - iLastReadPosition + KID3ChunkNameOffset);
	aChunk->iRes1 = *(iStartPtr + iPos - iLastReadPosition + KID3ChunkRes1Offset);
	aChunk->iRes2 = Read16(iStartPtr + iPos - iLastReadPosition + KID3ChunkRes2Offset);

	if (aChunk->iName != KID3ChunkNameHdr)
		return;

	const TUint8* aPtr = iStartPtr + iPos - iLastReadPosition + KID3ChunkLengthOffset;
    TUint32 x = *aPtr++ << 21;
    x |= *aPtr++ << 14;
    x |= *aPtr++ << 7;
    x |= *aPtr++;
	aChunk->iLength = x;

	aChunk->iFound = ETrue;
	}


const TInt KVerTab[4]={3,4,2,1};	// 2.5, reserved, 2, 1
//const TInt freqtab[4]={44100,48000,32000};	// EABI warning removal

const TInt KRateTab[2][3][16]=
{
  {
    {  0, 32, 64, 96,128,160,192,224,256,288,320,352,384,416,448,  0},
    {  0, 32, 48, 56, 64, 80, 96,112,128,160,192,224,256,320,384,  0},
    {  0, 32, 40, 48, 56, 64, 80, 96,112,128,160,192,224,256,320,  0},
  },
  {
    {  0, 32, 48, 56, 64, 80, 96,112,128,144,160,176,192,224,256,  0},
    {  0,  8, 16, 24, 32, 40, 48, 56, 64, 80, 96,112,128,144,160,  0},
    {  0,  8, 16, 24, 32, 40, 48, 56, 64, 80, 96,112,128,144,160,  0},
  },
};

const TInt KFreqTab[4]={44100,48000,32000, 0};

/**
*
* ReadChunk
* @param aChunk
*
*/
void CMMFWavFormatRead::ReadChunk(TMdaMP3Header* aChunk)
	{

	Mem::FillZ(REINTERPRET_CAST(TUint8*, aChunk),sizeof(TMdaMP3Header)); // Zero data
	
	const TUint8* aPtr = iStartPtr + 0;
    
	TUint8 x;
	TUint val;
	
	// Byte #0: frame sync
	x = *aPtr++;
	if (x != 0xFF)
		return;

	// Byte #1: version, layer & protected bit
	x = *aPtr++;
	if (x <0xE0)	// rest of flag
		return;

	val = (x>>3) & 3;
	aChunk->iVer = KVerTab[val];
	if (aChunk->iVer == 4)	// reserved
		return;
	aChunk->iLayer = (3-((x>>1) & 3)) + 1;
	if (aChunk->iLayer == 4)	// reserved
		return;
    if ((aChunk->iVer==3)&&(aChunk->iLayer!=3))
      return;
	aChunk->iProtected = x & 1;

	// Byte #2: bitrate, sample rate, padding, private
	x = *aPtr++;

	val = (x & 0xF0) >> 4;
   	aChunk->iBitRate = 
		KRateTab[aChunk->iVer == 1?0:1][aChunk->iLayer-1][val]*1000;
	if (aChunk->iBitRate == 0)
		return;
	
	val = (x>>2)&3;
	aChunk->iSampleRate = KFreqTab[val];
	if (aChunk->iSampleRate == 0)
		return;
	aChunk->iPadding = (x>>1)&1;
	aChunk->iPrivate = x&1;
    
	// Byte #3: stereo,Joint stereo mode extension, copyright, original, emphasis
	x = *aPtr++;
	aChunk->iStereo = ((x>>6)&3)!=3;
	aChunk->iModeExt = (x>>4)&3;
	aChunk->iCopyright = (x>>3)&1;
	aChunk->iOriginal = (x>>2)&1;
	aChunk->iEmphasis = x&3;

	aChunk->iFound = ETrue;
	}


/**
*
* ReadChunk
* @param aChunk
*
*/
void CMMFWavFormatRead::ReadChunk(TMdaRiffChunk* aChunk)
	{
	Mem::FillZ(REINTERPRET_CAST(TUint8*, aChunk),sizeof(TMdaRiffChunk)); // Zero data
	aChunk->iPosition=iPos + KRiffChunkHeaderLength;
	aChunk->iName = Read32(iStartPtr + iPos - iLastReadPosition);
	aChunk->iLength = Read32(iStartPtr + iPos - iLastReadPosition + KRiffChunkDataLength);
	}
/**
*
* ProcessFormatHeaderL
*  Read the format block and find the data chunk
*
*/
void CMMFWavFormatRead::ProcessFormatHeaderL()
	{
	TMdaRiffChunk* chunk = &iFormatChunk;

	// read the first MP3 header
	if (iCurrentID3Chunk.iFound)
		DoReadL(iCurrentID3Chunk.iPosition + iCurrentID3Chunk.iLength);
	else if (iDataChunk.iFound)
		DoReadL(iDataChunk.iPosition);

	ReadChunk(&iCurrentMP3Header);
	if (!iCurrentMP3Header.iFound)
		User::Leave(KErrNotSupported);

	iDataLength = KMaxTInt;		//unknown
	iSampleRate = iCurrentMP3Header.iSampleRate;
	iBitsPerSample = 16;
	iChannels = iCurrentMP3Header.iStereo?2:1;
	iCodecId = KMMFWavFormatTypeMP3;
	iFourCC.Set(KMMFFourCCCodeMP3);
	CalculateFrameSize();

	if (iRawHeader)
		{
		iStartPosition = 0;
		return;
		}

	if (iCurrentID3Chunk.iLength > 0)
		{

		// Find the data block
		TMdaID3Chunk* chunk=&iCurrentID3Chunk;
		// skip ID3 tag to get to data
		iStartPosition = chunk->iPosition + chunk->iLength; 
		return;
		}

	if (!chunk)
		User::Leave(KErrCorrupt);
	
	iLastReadPosition = chunk->iPosition; // Should be beginning of fmt block
	DoReadL(iLastReadPosition);
	
	// Set the real format
	TUint8* rawform = &(iBuffer->Data()[0]);	//skip _fmt & length
	iCodecId = Read16(rawform); rawform+=2;
	rawform+=2;
	
	switch (iCodecId)
		{
		case KMMFWavFormatTypeMP3:
			{
			iFourCC.Set(KMMFFourCCCodeMP3);
			}
			break;
		default:
			User::Leave(KErrNotSupported);
		}
	
	
	// Is there a fact chunk?
	if (iFactChunk.iFound)			
		iHasFactChunk = ETrue;
	
	// Find the data block
	chunk=&iDataChunk;
	iStartPosition = chunk->iPosition; 
	iDataLength = chunk->iLength;

	}

/**
*
* BufferFilledL
* @param aBuffer
*
*/
void CMMFWavFormatRead::BufferFilledL(CMMFBuffer* aBuffer)
	{
	//increment the current play position and check if that was the last buffer
	iVbrFilePosition += STATIC_CAST(CMMFDataBuffer*, aBuffer)->Data().Length();
	if (iVbrFilePosition >= iStartPosition + iDataLength)
		aBuffer->SetLastBuffer(ETrue);
	else
		aBuffer->SetLastBuffer(EFalse);

	
	//set position
	TTimeIntervalMicroSeconds position = //assumes frame numbers begin at frame 1 not 0
		TTimeIntervalMicroSeconds(TInt64(aBuffer->FrameNumber()-1)*iFrameTimeInterval.Int64());
	aBuffer->SetTimeToPlay(position);
	iDataPath->BufferFilledL(aBuffer);
	
	}

/**
*
* GetSupportedSampleRatesL
* @param aSampleRates)
*
*/
void CMMFWavFormatRead::GetSupportedSampleRatesL(RArray<TUint>& aSampleRates)
	{
	aSampleRates.Reset();

	// Iterate through the valid sample table and append each value to aSampleRates
	TInt i = sizeof(KWavSampleRates) / sizeof(TUint);
	
	while (i--)
		{
		User::LeaveIfError(aSampleRates.Append(KWavSampleRates[i]));
		}
	}

/**
*
* GetSupportedNumChannelsL
* @param aNumChannels
*/
void CMMFWavFormatRead::GetSupportedNumChannelsL(RArray<TUint>& aNumChannels)
	{
	aNumChannels.Reset();
	User::LeaveIfError(aNumChannels.Append(KMono));
	User::LeaveIfError(aNumChannels.Append(KStereo));
	}

/**
*
* GetSupportedDataTypesL
* @param aMediaId
* @param aDataTypes 
*
*/
void CMMFWavFormatRead::GetSupportedDataTypesL(TMediaId aMediaId, RArray<TFourCC>& aDataTypes)
	{
	if (aMediaId.iMediaType != KUidMediaTypeAudio)
		User::Leave(KErrNotSupported);
	aDataTypes.Reset();
//	User::LeaveIfError(aDataTypes.Append(KMMFFourCCCodePCMU8));	
//	User::LeaveIfError(aDataTypes.Append(KMMFFourCCCodePCM16));	
//	User::LeaveIfError(aDataTypes.Append(KMMFFourCCCodeIMAD));
//	User::LeaveIfError(aDataTypes.Append(KMMFFourCCCodeALAW));
//	User::LeaveIfError(aDataTypes.Append(KMMFFourCCCodeMuLAW));
//	User::LeaveIfError(aDataTypes.Append(KMMFFourCCCodeGSM610));
	User::LeaveIfError(aDataTypes.Append(KMMFFourCCCodeMP3));
	}


// Exported proxy for instantiation method resolution
// Define the interface UIDs
const TImplementationProxy ImplementationTable[] = 
	{
		IMPLEMENTATION_PROXY_ENTRY(KUidMP3ImplementationFormat,	CMMFWavFormatRead::NewL),
		IMPLEMENTATION_PROXY_ENTRY(KUidMP3ImplementationFormatID3,CMMFWavFormatRead::NewL),
		IMPLEMENTATION_PROXY_ENTRY(KUidMP3ImplementationFormatWAV,CMMFWavFormatRead::NewL)
	};

/**
* 
* ImplementationGroupProxy
* @param aTableCount
* @return TImplementationProxy*
*
*/
EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
	return ImplementationTable;
	}



