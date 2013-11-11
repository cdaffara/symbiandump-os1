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

#include "avireader.h"

const TInt KReadBufferSize = 0x8000; // 4k
const TInt KChunkHeaderSize = 8;

const TInt KInvalidMediaIdx = -1;

#define RiffChunkName(A,B,C,D) ((D<<24)+(C<<16)+(B<<8)+A)

const TInt KRiffChunkNameRiff = RiffChunkName('R','I','F','F');
const TInt KRiffChunkNameList = RiffChunkName('L','I','S','T');
const TInt KRiffChunkNameAvih = RiffChunkName('a','v','i','h');
const TInt KRiffChunkTypeAvi  = RiffChunkName('A','V','I',' ');
const TInt KRiffChunkTypeHdrl = RiffChunkName('h','d','r','l');
const TInt KRiffChunkTypeStrl = RiffChunkName('s','t','r','l');
const TInt KRiffChunkNameStrh = RiffChunkName('s','t','r','h');
const TInt KRiffChunkNameStrf = RiffChunkName('s','t','r','f');
const TInt KRiffChunkNameStrn = RiffChunkName('s','t','r','n');
const TInt KRiffChunkTypeVids = RiffChunkName('v','i','d','s');
const TInt KRiffChunkTypeAuds = RiffChunkName('a','u','d','s');
const TInt KRiffChunkNameJunk = RiffChunkName('J','U','N','K');
const TInt KRiffChunkTypeMovi = RiffChunkName('m','o','v','i');
const TInt KRiffChunkName00db = RiffChunkName('0','0','d','b');
const TInt KRiffChunkName00dc = RiffChunkName('0','0','d','c');
const TInt KRiffChunkName00wb = RiffChunkName('0','0','w','b');
const TInt KRiffChunkName01wb = RiffChunkName('0','1','w','b');
const TInt KRiffChunkName01db = RiffChunkName('0','1','d','b');
const TInt KRiffChunkName01dc = RiffChunkName('0','1','d','c');
const TInt KRiffChunkNameXvid = RiffChunkName('X','V','I','D');
const TInt KRiffChunkNameIdx1 = RiffChunkName('i','d','x','1');
const TInt KRiffChunkNameStrd = RiffChunkName('s','t','r','d'); 
const TInt KRiffChunkNameIndex = RiffChunkName('i','n','d','x');
const TInt KRiffChunkNameVprp = RiffChunkName('v','p','r','p');
const TInt KAVIF_ISINTERLEAVED = 0x00000001;




//CAviReader Functions

/**
Fast read for non-aligned 16 bit data.
@param aPtr
@return
*/
LOCAL_C TUint16 Read16(const TUint8* aPtr)
	{
    return *(TUint16*)aPtr;
    }

/**
Fast read for non-aligned 32 bit data.
@param aPtr
@return
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
Creates CAviReader object.
@param aClip
@param aObserver
@return CAviReader*
*/ 
CAviReader* CAviReader::NewL(CMMFClip& aClip, MAviReaderObserver& aObserver)
    {
    CAviReader* self = new(ELeave) CAviReader(aClip, aObserver);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }
  
/**
Constructor of CAviReader
*/
CAviReader::CAviReader(CMMFClip& aClip, MAviReaderObserver& aObserver)
         :MDataSink(KNullUid),iObserver(aObserver),iClip(aClip),iAudioIdx(KInvalidMediaIdx),
         iVideoIdx(KInvalidMediaIdx)
    {
    }

/**
Destructor of CAviReader
*/  
CAviReader::~CAviReader()
    {
	delete iVideoBuffer;
	delete iAudioBuffer;
	delete iSourceBuffer;
    }

/**
Second phase constructor.
*/ 
void CAviReader::ConstructL()
    {
    if (iClip.Size() > 0)
    	{
    	iClip.SourcePrimeL();
    	iSourceBuffer = CMMFDescriptorBuffer::NewL(KReadBufferSize);
    	ReadFormatL();
    	}
     else
    	{
    	iClip.SourceStopL();
    	User::Leave(KErrCorrupt);
    	}
    }   

/**
Reads the header information in the clip.
@leave KErrCorrupt if the header information is incorrect.
*/ 
TInt CAviReader::ReadFormatL()
	{
	User::LeaveIfError(FindRiffChunksL());
	return 0;
	}
	
/**
Reads the header information in the clip.
@return KErrNone on Success or KErrCorrupt on incorrect header.
*/ 
TInt CAviReader::FindRiffChunksL()
	{
	TBool videoEnabled = EFalse;
	iClip.ReadBufferL(iSourceBuffer,0);
	TUint8* rawform = &(iSourceBuffer->Data()[0]);
	TUint8* pBufBegin =  rawform;
	if (Read32(rawform) !=  KRiffChunkNameRiff)
		{
		return KErrCorrupt; //not a RIFF file	
		}
	rawform+=4;
	rawform+=4;// skip file size
	if (Read32(rawform) != KRiffChunkTypeAvi)
		{
	 	return KErrCorrupt;//not an .avi file
		}
	rawform+=4;
	if (Read32(rawform) != KRiffChunkNameList)
		{
		return KErrCorrupt; 	//it is not a list
		}
	rawform+=4;
	TUint32 dwToRead = Read32(rawform);
	rawform+=4;//crossed the list size
	if (Read32(rawform) !=KRiffChunkTypeHdrl)
		{
		return KErrCorrupt;
		}
	rawform+=4;
	if (Read32(rawform) != KRiffChunkNameAvih)
		{
		return KErrCorrupt;	
		}
	rawform+=4;
	rawform+=4; //skip chunk size
	iMainHeader.iMicroSecPerFrame = Read32(rawform);rawform+=4;
	iMainHeader.iMaxBytesPerSec = Read32(rawform);rawform+=4;
	iMainHeader.iPaddingGranularity= Read32(rawform);rawform+=4;
	//Audio gives underflow if the file is interleaved.So audio and video
	//is read separately.
	iMainHeader.iFlags= Read32(rawform); rawform+=4;
	iMainHeader.iTotalFrames= Read32(rawform);rawform+=4;
	iMainHeader.iInitialFrames= Read32(rawform);rawform+=4;
	iMainHeader.iStreams	= Read32(rawform);rawform+=4;
	if(iMainHeader.iStreams > 2)
		{
		return KErrCorrupt;
		}
	iMainHeader.iSuggestedBufferSize= Read32(rawform);rawform+=4;
	iMainHeader.iWidth= Read32(rawform);rawform+=4;
	iMainHeader.iHeight= Read32(rawform);rawform+=4;
	iMainHeader.iReserved[0]= Read32(rawform);rawform+=4;
    iMainHeader.iReserved[1]= Read32(rawform);rawform+=4;
    iMainHeader.iReserved[2]= Read32(rawform);rawform+=4;
    iMainHeader.iReserved[3]= Read32(rawform);rawform+=4;

	for (TUint8 i=0; i < iMainHeader.iStreams; i++)
		{
		if(Read32(rawform) != KRiffChunkNameList)
			{
			return KErrCorrupt;	
			}
		rawform+=4;
		rawform+=4; //skip list size;
		if(Read32(rawform) != KRiffChunkTypeStrl)
			{
			return KErrCorrupt;
			}
		rawform+=4;
		if(Read32(rawform) != KRiffChunkNameStrh)
			{
			return KErrCorrupt;
			}
		rawform+=4;//read stream header chunk
		rawform+=4; //skip stream header chunk size;
		iStreamHeader[i].iFccType = Read32(rawform);rawform+=4;
		iStreamHeader[i].iFccHandler = Read32(rawform);rawform+=4;
		iStreamHeader[i].iFlags= Read32(rawform);rawform+=4; 
		iStreamHeader[i].iReserved = Read32(rawform);rawform+=4;
		iStreamHeader[i].iInitialFrames =Read32(rawform);rawform+=4;
		iStreamHeader[i].iScale =Read32(rawform);rawform+=4;
		iStreamHeader[i].iRate= Read32(rawform);rawform+=4;
		iStreamHeader[i].iStart = Read32(rawform);rawform+=4;
		iStreamHeader[i].iLength = Read32(rawform);rawform+=4;
		iStreamHeader[i].iSuggestedBufferSize = Read32(rawform);rawform+=4;
		iStreamHeader[i].iQuality = Read32(rawform);rawform+=4;
		iStreamHeader[i].iSampleSize = Read32(rawform);rawform+=4;
		iStreamHeader[i].iRcFrame.iLeft =Read16(rawform);rawform+=2;
		iStreamHeader[i].iRcFrame.iTop =Read16(rawform);rawform+=2;
		iStreamHeader[i].iRcFrame.iRight =Read16(rawform);rawform+=2;
		iStreamHeader[i].iRcFrame.iBottom =Read16(rawform);rawform+=2;
		if (Read32(rawform) != KRiffChunkNameStrf)		
			{
			return KErrCorrupt;		
			}
		rawform+=4;//read stream format chunk
		TInt chunklen = Read32(rawform);
		rawform+=4; //skip stream format chunk size;	
	
		if (iStreamHeader[i].iFccType == KRiffChunkTypeAuds) //if audio stream
			{
			if(iAudioEnabled)
				{
				return KErrCorrupt;	
				}
			iAudioEnabled = ETrue;
			if (Read16(rawform) == 0x0001) // Do not have support except PCM
				{
	    		iPCMWaveFormat.iWaveFormat.iFormatTag = Read16(rawform);rawform+=2;
                iPCMWaveFormat.iWaveFormat.iNChannels = Read16(rawform);rawform+=2;
                iPCMWaveFormat.iWaveFormat.iNSamplesPerSec = Read32(rawform);rawform+=4; 
                 iPCMWaveFormat.iWaveFormat.iNAvgBytesPerSec= Read32(rawform); rawform+=4;
                iPCMWaveFormat.iWaveFormat.iNBlockAlign = Read16(rawform);rawform+=2;
                iPCMWaveFormat.iBitsPerSample = Read16(rawform);rawform+=2;                

	    		iAudioIdx = i;				
				}
			else
				{
				return KErrNotSupported;
				}
			}
			
		else if (iStreamHeader[i].iFccType == KRiffChunkTypeVids) //if video stream
			{
			if(iStreamHeader[i].iFccHandler != KRiffChunkNameXvid) 
				{
				return KErrNotSupported;
				}
			if(videoEnabled)
				{
				return KErrCorrupt;	
				}
				
			TUint8* temp =  rawform;
			
			iBitmapInfoHeader.iBiSize= Read32(rawform);rawform+=4;
            iBitmapInfoHeader.iBiWidth= Read32(rawform);rawform+=4;
            iBitmapInfoHeader.iBiHeight= Read32(rawform);rawform+=4;
            iBitmapInfoHeader.iBiPlanes= Read16(rawform);rawform+=2;
            iBitmapInfoHeader.iBiBitCount= Read16(rawform);rawform+=2;
            iBitmapInfoHeader.iBiCompression= Read32(rawform);rawform+=4;
            iBitmapInfoHeader.iBiSizeImage= Read32(rawform);rawform+=4;
            iBitmapInfoHeader.iBiXPelsPerMeter= Read32(rawform);rawform+=4;
            iBitmapInfoHeader.iBiYPelsPerMeter= Read32(rawform);rawform+=4;
            iBitmapInfoHeader.iBiClrUsed= Read32(rawform);rawform+=4;
            iBitmapInfoHeader.iBiClrImportant= Read32(rawform);rawform+=4;
            
            rawform = temp + chunklen;

			iVideoIdx = i;
			videoEnabled = ETrue;
			}
		else				//if unidentified stream types.
			{
			return KErrCorrupt;
			}
	
		TUint32 temp = Read32(rawform);	
		while (temp == KRiffChunkNameStrn || temp == KRiffChunkNameStrd || temp == KRiffChunkNameJunk || temp == KRiffChunkNameIndex || temp == KRiffChunkNameVprp) // Skipping the chunks
			{
			rawform+=4;
			temp = Read32(rawform);
			rawform+=4+temp;
			temp = Read32(rawform);
			}
		}
	
	//added to skip the rest of the header to reach movi list. 
	TInt bytesRead = rawform - pBufBegin;
	iSourcePos = bytesRead;
	
	for (TInt i = 0; i < KReadBufferSize - bytesRead; i ++)
		{
		if(Read32(rawform) == KRiffChunkTypeMovi)
			{
			iSourcePos += i + 4;
			iAudioPos = iSourcePos;
			iVideoPos = iSourcePos;
			break;
			}
		rawform += 1;	
		}
	
	return KErrNone;
	}

/**
Fills the aBuffer with video data depends on interleaved or not.
@param aBuffer 
*/ 
void CAviReader::FillVideoBufferL(TVideoInputBuffer* aBuffer)
	{
	if (!aBuffer)
		{
		User::Leave(KErrArgument);
		}
	iBufferFromDevVideo = aBuffer;
	iVideoRequestMade = ETrue;

	if (iMainHeader.iFlags == KAVIF_ISINTERLEAVED)
		{
		
		//Here Audio and Video data are interleaved in the form of REc lists.
		if (!iReadRequestMade) 
			{
			//Read request for REC list is not yet made. So read the REC list into the sourcebuffer.
			ReadNextFrameL(TUid::Null(), iSourcePos);
			iReadRequestMade = ETrue;
			}

		else if (iReadCompleted)
			{
			//REC list is already read into the video buffer.
			//copy the data from the video buffer to devvideo buffer.
			aBuffer->iData.Copy(iVideoBuffer->Data());
			ReadComplete();
			}
		}

	else
		{
		//There are no REC lists. Audio and Video is spread across the file. So we read audio and video
		//chunks independently
		ReadNextFrameL(KUidMediaTypeVideo, iVideoPos);
		}
	}

/**
Fills the aBuffer with audio data depends on interleaved or not.
@param aBuffer 
*/ 
void CAviReader::FillAudioBufferL(CMMFBuffer* aBuffer)
	{
	if (!aBuffer)
		{
		User::Leave(KErrArgument);
		}
	iBufferFromDevSound = aBuffer;
	iAudioRequestMade = ETrue;
	if (iMainHeader.iFlags == KAVIF_ISINTERLEAVED)
		{
		//Here Audio and Video data are interleaved in the form of REc lists.
		if (!iReadRequestMade)
			{
			//Read request for REC list is not yet made. So read the REC list into the sourcebuffer.
			ReadNextFrameL(TUid::Null(), iSourcePos);
			iReadRequestMade = ETrue;
			}
		else if (iReadCompleted)
			{
			//audio frame is already read into the audio buffer
			//copy the data from the audio buffer to devsound buffer.
			CMMFDataBuffer* bufferFromSink = static_cast<CMMFDataBuffer*>(aBuffer);
			bufferFromSink->Data().Copy(iAudioBuffer->Data());
			ReadComplete();
			}
		}
	else
		{
		if (iAudioBuffer)
			{
			BufferFilledL(iAudioBuffer);
			}
		else
			{
			//non-interleaved
			ReadNextFrameL(KUidMediaTypeAudio, iAudioPos);
			}
		}
	}			

/**
Identifies if the chunk is valid by the media type and fills the data in buffer
@param aMediaType
@param aPosition
*/
void CAviReader::ReadNextFrameL(TUid aMediaType, TInt& aPosition)
	{
	TUint32 dwChunkId = 0;
	TUint32 dwChunkSz = 0;
	TUint8* rawForm = NULL;
	
	//We read chunkId and chunkSize first into the headerframe below. If Audio and Video chunks are found, data is read into the respective buffers 
	//we skip chunkSize bytes if we come across any other blocks.
	CMMFDescriptorBuffer* headerFrame = CMMFDescriptorBuffer::NewL(KChunkHeaderSize);
    CleanupStack::PushL(headerFrame);
    
	for(;;)
		{
		iClip.ReadBufferL(headerFrame, aPosition);
		TInt len = headerFrame->Data().Length();
		if (len == 0)
			{
			SetMediaEOS(aMediaType);
			break;
			}
			
		rawForm = &(headerFrame->Data()[0]);
		dwChunkId = Read32(rawForm);
		if (!IsValidBlock(dwChunkId))
			{
			//Sometimes we dont get next chunk immediately after the Previous chunk(Padding). 
			//we need to search for next valid data chunk ourselves if the chunkId read is not valid
			if (!ReadNextFrameStartL(aPosition))
				{
				//no more data frames
				SetMediaEOS(aMediaType);
				break;
				}
			continue;//found valid chunk. so continue to read the chunk
			}

		rawForm += 4; //Skip ChunkId
		aPosition += 4; //Skip ChunkId 
		dwChunkSz = Read32(rawForm);
							
		rawForm+=4; //Skip ChunkSize
		aPosition += 4; //Skip ChunkSIze

		if (dwChunkSz == 0)
			{
			//We may get chunks of length Zero. So loop again to read the next chunk.
			continue;
			}
		if (dwChunkId == KRiffChunkNameList)//list rec
			{
			if(iMainHeader.iFlags == KAVIF_ISINTERLEAVED)
				{
				//found REC list. Read the whole list into the SourceBuffer
				delete iSourceBuffer;
				iSourceBuffer = NULL;
				iSourceBuffer = CMMFDescriptorBuffer::NewL(dwChunkSz);
				iClip.ReadBufferL(iSourceBuffer,aPosition, this);
				aPosition += dwChunkSz;//skip the list size
				break;
				}
			else
				{
				continue;
				}
			}
		else if (dwChunkId == KRiffChunkNameJunk)//junk
			{
			//found Junk chunk. Skip it.
			aPosition += dwChunkSz;
			}
		else if ((dwChunkId == KRiffChunkName00db) || (dwChunkId == KRiffChunkName00dc)
			|| (dwChunkId == KRiffChunkName01db) || (dwChunkId == KRiffChunkName01dc)) //video
			{
			if (aMediaType == KUidMediaTypeVideo)
				{
				//found video when aMediaType is Video. So read the video chunk into the videobuffer.
				delete iVideoBuffer;
				iVideoBuffer = NULL;
				iVideoBuffer = CMMFDescriptorBuffer::NewL(dwChunkSz);
				iClip.ReadBufferL(iVideoBuffer,aPosition, this);
				aPosition += dwChunkSz;
				break;
				}
			else
				{
				//found video when aMediaType is not Video. So Skip it.
				aPosition += dwChunkSz;	
				}
			}
		else if ((dwChunkId == KRiffChunkName00wb) || (dwChunkId == KRiffChunkName01wb)) //audio
			{
			if (aMediaType == KUidMediaTypeAudio)
				{
				//found audio when aMediaType is Audio. So read the audio chunk into the audiobuffer.
				delete iAudioBuffer;
				iAudioBuffer = NULL;
				iAudioBuffer = CMMFDescriptorBuffer::NewL(dwChunkSz);
				iClip.ReadBufferL(iAudioBuffer,aPosition, this);	
				aPosition += dwChunkSz;
				break;	
				}
			else
				{
				//found audio when aMediaType is not audio. So Skip it.
				aPosition += dwChunkSz;
				}		
			}
		else if (dwChunkId == KRiffChunkNameIdx1)//index block
			{
			aPosition += dwChunkSz;
			//Reached index block. This is placed at the end of the data. No more data
			SetMediaEOS(aMediaType);
			break;
			}
	   }
	CleanupStack::PopAndDestroy(headerFrame);
	
	}		


/**
Loops through until a valid chunk is identified.
@param aStartPos
@return ETrue if valid chunk is found.
*/
TBool CAviReader::ReadNextFrameStartL(TInt& aStartPos)
	{
	TBool found = EFalse;
	TInt count = 0;

	CMMFDescriptorBuffer* buffer = CMMFDescriptorBuffer::NewL(KReadBufferSize);
    CleanupStack::PushL(buffer);
	iClip.ReadBufferL(buffer, aStartPos);

	TUint8* rawform = &(buffer->Data()[0]);
	
	while (count < KReadBufferSize)
		{
		if (IsValidBlock(Read32(rawform)))
			{
			found = ETrue;
			break;
			}
		aStartPos++;
		rawform++;
		count++;
		}
	CleanupStack::PopAndDestroy(buffer);
	return found;
	}

/**
Identifies a valid audio and video chunk is present
@param aChunkId
@return ETrue if validblock is found else EFalse.
*/
TBool CAviReader::IsValidBlock(TUint aChunkId) 
	{
	switch(aChunkId)
		{
		case KRiffChunkName00wb:
		case KRiffChunkName01wb:
		case KRiffChunkName00db:
		case KRiffChunkName00dc:
		case KRiffChunkName01db:
		case KRiffChunkName01dc:	
		case KRiffChunkNameList:
		case KRiffChunkNameJunk:
		case KRiffChunkNameIdx1:
			return ETrue;
		default:
			return EFalse;
		}
	}			

/**
Fills the buffer with audio/Video data and informs the observer
@param aBuffer
*/
void CAviReader::BufferFilledL(CMMFBuffer* aBuffer)
	{
	if (!aBuffer)
		{
		User::Leave(KErrArgument);
		}
	if (aBuffer == iVideoBuffer)
		{
		//callback for video read when the file is not interleaved(No REC lists)
		iBufferFromDevVideo->iData.Copy(iVideoBuffer->Data());
		iObserver.VideoBufferFilled(iBufferFromDevVideo);
		}
	else if (aBuffer == iAudioBuffer) 
		{
		//callback for audio read when the file is not interleaved(No REC lists)
		CMMFDataBuffer* bufferFromSink = static_cast<CMMFDataBuffer*>(iBufferFromDevSound);

		TInt sinkBufLen = bufferFromSink->Data().MaxSize();
		TInt audBufLen = iAudioBuffer->Data().Length();
		
		if (sinkBufLen < audBufLen)
			{
			bufferFromSink->Data().Copy(iAudioBuffer->Data().Ptr(), sinkBufLen);
			CMMFDescriptorBuffer* tempBuffer = CMMFDescriptorBuffer::NewL(audBufLen - sinkBufLen);
			tempBuffer->Data().Copy(iAudioBuffer->Data().Mid(sinkBufLen));
			delete iAudioBuffer;
			iAudioBuffer = tempBuffer;
			}
		else
			{
			// DEF113319 - call SetLastBuffer when sending last audio chunk
			TInt position = 0;
			position = (iMainHeader.iFlags == KAVIF_ISINTERLEAVED) ? iSourcePos : iAudioPos;
			if(!IsAudioDataAvailableL(position))
				{
				bufferFromSink->SetLastBuffer(ETrue);
				SetMediaEOS(KUidMediaTypeAudio);
				}

			bufferFromSink->Data().Copy(iAudioBuffer->Data());
			delete iAudioBuffer;
			iAudioBuffer = NULL;
			}
		iObserver.AudioBufferFilled();
		}
	else if (aBuffer == iSourceBuffer)//callback for REC list
		{
		//callback for REC list read. We need to extract video and audio chunks from the sourcebuffer
		TUint8* rawform = &(iSourceBuffer->Data()[0]);
		TInt desPos = 0; //position in descriptor

		//Now we should be pointing to audio or video chunks
		TInt bufLen = iSourceBuffer->Data().Length();
		if(bufLen < 4)
			{
			User::Leave(KErrCorrupt);	
			}
		rawform += 4; //skip the fourcc of REC
		desPos += 4; //byte count corresponding to where rawform points to
		
		if (bufLen - desPos < 8)
		 	{
		 	//if the buffer is not long enough to accomodate - chunk id and size leave
		  	User::Leave(KErrCorrupt);
		  	}

		for (TUint8 i=0; i < iMainHeader.iStreams; i++)//we expect only 2 streams at the moment
			{
			if (desPos == bufLen) //caution check for the necessity of the block
				{
	 			//Sometimes we only have one stream in this list. No more data to read
				break;
				}

			if (bufLen - desPos < 8 && i== 1)
			 	{
			 	//if the buffer is not long enough to accomodate - chunk id and size leave
			  	break;
			  	}

			TUint32 dwChunkId = Read32(rawform);
			rawform += 4; //Skip chunkId
			desPos += 4;
			TBool bAudioId = EFalse;
			TBool bVideoId = EFalse;
			switch (dwChunkId)	//identify chunk
				{
				case KRiffChunkName00db:
				case KRiffChunkName00dc:
				case KRiffChunkName01db:
				case KRiffChunkName01dc:
					bVideoId = ETrue;
					break;
				case KRiffChunkName00wb:
				case KRiffChunkName01wb:
					bAudioId = ETrue;
					break;
				default:
					User::Leave(KErrCorrupt);
				}
			
			if (!bAudioId && !bVideoId)
				{
				//we are supposed to get audio or video stream here. if anything else, we return KErrCorrupt				
				User::Leave(KErrCorrupt);
				}
			TUint32 dwChunkSz = Read32(rawform);
			if (dwChunkSz > bufLen - desPos)
				{
				User::Leave(KErrCorrupt); //caution - check	
				}
			rawform += 4; //Skip chunkSize
			desPos += 4;
			TPtr8 temp = iSourceBuffer->Data().MidTPtr(desPos, dwChunkSz);
			if (bVideoId)// video
				{
				if (iVideoRequestMade)
					{
					//if video request is already made we can copy the data directly into the devvideo buffer
					//instead of iVideoBuffer
					iBufferFromDevVideo->iData.Copy(temp);
					}
				else 
					{
					//DevVideo request is not made. So copy the data into video buffer
					delete iVideoBuffer;
					iVideoBuffer = NULL;
					iVideoBuffer = CMMFDescriptorBuffer::NewL(dwChunkSz);
					iVideoBuffer->Data().Copy(temp);
					}
				}
			else if (bAudioId)//audio
				{
				if (iAudioRequestMade)
					{
					//if audio request is already made, copy the data into devsound buffer
					CMMFDataBuffer* bufferFromSink = static_cast<CMMFDataBuffer*>(iBufferFromDevSound);
					bufferFromSink->Data().Copy(temp);
					}
				else 
					{
					//DevSound request is not made. copy the audio chunk into audio buffer
					delete iAudioBuffer;
					iAudioBuffer = NULL;
					iAudioBuffer = CMMFDescriptorBuffer::NewL(dwChunkSz);
					iAudioBuffer->Data().Copy(temp);
					}
				}
			rawform	+= dwChunkSz;//Skip video/audio chunk
			desPos += dwChunkSz;	
			}
		iReadCompleted = ETrue;//REC list is read
		// we send the bufferfilled callbacks after both audio and video requests are made.
		ReadComplete();
		}
	else
		{
		User::Leave(KErrCorrupt);
		}
	}

/**
Informs audio and video data in rec list is read and the buffer can
now be sent for decoding.This function is specific for interleaved files
where it is sent for decoding only after both audio and video data is read.
*/
void CAviReader::ReadComplete()
	{
	if ((iAudioRequestMade) && (iVideoRequestMade))
		{
		//reset back everything for the next cycle
		iReadRequestMade = EFalse;
		iReadCompleted = EFalse;
		iAudioRequestMade = EFalse;
		iVideoRequestMade = EFalse;
		iObserver.VideoBufferFilled(iBufferFromDevVideo);
		iObserver.AudioBufferFilled();
		}
	}

/**
Resets the data positions in the file 
*/			
void CAviReader::ResetL()
	{
	iVideoEos = EFalse;
	iAudioEos = EFalse;
	iAudioPos = iSourcePos;
	iVideoPos = iSourcePos;
	iReadRequestMade = EFalse;
	iReadCompleted = EFalse;
	iAudioRequestMade = EFalse;
	iVideoRequestMade = EFalse;
	iClip.SourceStopL();
	}

/**
Identifies if video data in the clip is fully read.
@return ETrue if end of videodata
*/			
TBool CAviReader::IsVideoInputEnd()
	{
	return iVideoEos;		
	}

/**
Identifies if audio data in the clip is fully read.
@return ETrue if end of audiodata
*/		
TBool CAviReader::IsAudioInputEnd()
	{
	return iAudioEos;		
	}
	
/**
Set the position in the file to play from
@param aPosition - this is not used.
@leave KErrNotSupported.
*/
void CAviReader::SetPositionL(TInt /*aPosition*/)
	{
	//This will leave KErrNotSuuported as there is no support to
	//seek the position in an .avi file.
	User::Leave(KErrNotSupported);
	}

/**
Returns if audio is enabled in an .avi file.
@param aEnabled.
*/
void CAviReader::AudioEnabled(TBool& aEnabled)
	{
	aEnabled = iAudioEnabled;
	}
	

/**
Returns the frame rate applied for video
@param aFramesPerSecond
*/
void CAviReader::FrameRate(TReal32 &aFramesPerSecond)
	{
	ASSERT(iVideoIdx > KInvalidMediaIdx);
	if (iStreamHeader[iVideoIdx].iScale == 0)	
		{
		//If this value is 0 consider it as 1 second.
		iStreamHeader[iVideoIdx].iScale = 1; 
		}	
	aFramesPerSecond = iStreamHeader[iVideoIdx].iRate;	
	}

/**
Returns the sample rate applied for audio.
@return audio samplerate 
*/	
TInt CAviReader::SampleRate()
	{
	ASSERT(iAudioIdx > KInvalidMediaIdx);
	return iPCMWaveFormat.iWaveFormat.iNSamplesPerSec ;
	}

/**
Returns the number of channels set in audio.
@return number of audio channels.
*/	
TInt CAviReader::Channels()
	{
	ASSERT(iAudioIdx > KInvalidMediaIdx);
	return iPCMWaveFormat.iWaveFormat.iNChannels;
	}

/**
Returns the bits per sample for audio.
@return bitspersample
*/
TInt CAviReader::BitsPerSample()
	{
	ASSERT(iAudioIdx > KInvalidMediaIdx);
	return iPCMWaveFormat.iBitsPerSample;
	}
	

/**
Returns the framesize of video file.
@param aFrameSize
*/
void CAviReader::VideoFrameSize(TSize &aFrameSize)
	{
	ASSERT(iVideoIdx > KInvalidMediaIdx);
	aFrameSize.iHeight = iMainHeader.iHeight;
	aFrameSize.iWidth =  iMainHeader.iWidth;
	}	
	
/**
Returns the Video codec used in the video file
@param aMimeType -this is not used
*/
void CAviReader::VideoMimeType(TDes8& /*aMimeType*/)
	{
	//there is no conversion from fcchandler to descriptor
	}

/**
Returns the audioCodec used in the video file. 
@param aCodec
*/	
void CAviReader::AudioCodec(TFourCC& /*aCodec*/)
	{
	//there is no conversion from iFccHandler to descriptor
	}

/**
Returns the duration of video file.
@return Duration of the file in TTimeIntervalMicroSeconds
*/
TTimeIntervalMicroSeconds CAviReader::Duration() const
	{
	ASSERT(iVideoIdx > KInvalidMediaIdx);
	ASSERT(iStreamHeader[iVideoIdx].iRate);
	return (iMainHeader.iTotalFrames*1000000)/iStreamHeader[iVideoIdx].iRate;				
	}
	
/**
Sets the playwindow for playing audio and video.
@param aStart
@param aEnd
*/	
void CAviReader::SetPlayWindowL(TTimeIntervalMicroSeconds /*aStart*/,TTimeIntervalMicroSeconds /*aEnd*/)
	{
	//this will leave with KErrNotSupported as there is no
	//support for seeking the position in the file.
	User::Leave(KErrNotSupported);
	}
	
/**
Sets the end of audio and video data.
@param aMediaType
*/	
void CAviReader::SetMediaEOS(TUid aMediaType)	
	{
	if (aMediaType == KUidMediaTypeAudio) //end of audio
		{
		iAudioEos = ETrue;	
		}
	else if (aMediaType == KUidMediaTypeVideo)//end of video
		{
		iVideoEos = ETrue;
		}
	else //end of REC lists
		{
		iAudioEos = ETrue;
		iVideoEos = ETrue;
		}
	}

//MDataSink
CMMFBuffer* CAviReader::CreateSinkBufferL(TMediaId /*aMediaId*/, TBool&/*aReference*/)
	{
	User::Leave(KErrNotSupported);
	return NULL;
	}

void CAviReader::ConstructSinkL( const TDesC8& /*aInitData*/)
	{
	User::Leave(KErrNotSupported);
	}
	
TFourCC CAviReader::SinkDataTypeCode(TMediaId /*aMediaId*/)
	{
	return 0;
	}
		
void CAviReader::EmptyBufferL(CMMFBuffer* /*aBuffer*/, MDataSource* /*aSupplier*/, TMediaId /*aMediaId*/)
	{
	User::Leave(KErrNotSupported);
	}
	
TBool CAviReader::CanCreateSinkBuffer()
	{
	return EFalse;	
	}

/**
// DEF113319 - added to check last audio chunk
Loops through to check whether valid audio chunk is available.
@param aStartPos
@return ETrue if valid audio chunk is found.
*/
TBool CAviReader::IsAudioDataAvailableL(TInt aStartPos)
	{
	TBool foundValidAudioChunk = EFalse;
	TUint32 dwChunkId = 0;
	TUint8* rawform = NULL;

	CMMFDescriptorBuffer* headerFrame = CMMFDescriptorBuffer::NewL(KChunkHeaderSize);
    CleanupStack::PushL(headerFrame);
	
	for(;;)
		{
		iClip.ReadBufferL(headerFrame, aStartPos);
		TInt len = headerFrame->Data().Length();
		if (len == 0)
			{
			// end of stream
			break;
			}
		rawform = &(headerFrame->Data()[0]);
		dwChunkId = Read32(rawform);
		
		if (!IsValidBlock(dwChunkId))
			{
			// No valid chunk id found ... 
			//Sometimes we dont get next chunk immediately after the Previous chunk(Padding). 
			//we need to search for next valid data chunk ourselves if the chunkId read is not valid
			if (!ReadNextFrameStartL(aStartPos))
				{
				// reached EOS 
				break;
				}
			continue;//found valid chunk. so continue to read the chunk
			}
		if ((dwChunkId == KRiffChunkName00wb) || (dwChunkId == KRiffChunkName01wb)) // check if its valid audio chunk
			{
			foundValidAudioChunk = ETrue;
			break;
			}
		// non audio chunk found, so go and look for next frame header
		rawform += 4; //Skip ChunkId
		aStartPos += ((Read32(rawform)) + 4 + 4); //Skip: 1. dwChunkSz, 2. ChunkSize(4), 3.ChunkId(4) 
		}
	CleanupStack::PopAndDestroy(headerFrame);
	return foundValidAudioChunk;
	}
