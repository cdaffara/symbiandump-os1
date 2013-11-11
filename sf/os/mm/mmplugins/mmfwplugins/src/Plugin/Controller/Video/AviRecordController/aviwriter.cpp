#include "aviwriter.h"
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

const TInt  KReadBufferSize = 0x10000;
const TUint KDefaultSampleRate = 22050;
const TInt  KDefaultChannels = 2;
const TInt  KDefaultBitsPerSample = 16;
const TInt 	KOneMillion = 1000000;
const TInt 	KMediaVideoIdxValue = 0;
const TInt 	KMediaAudioIdxValue = 1;
const TInt	KPcmWaveFormatSize = 16;	// sizeof(TPcmWaveFormat) returns 20 due to 4-bytes alignment
//The below constants gives the positions to update the headers
const TInt  KUpdateHeaderSize = 4;
const TInt  KTotalFramesPos = 48;	
const TInt	KVideoLengthPos = 140;
const TInt	KAudioLengthPos = 264;
const TInt	KMicroSecPerFramePos = 32;		

#define RiffChunkName(A,B,C,D) ((D<<24)+(C<<16)+(B<<8)+A)
const TInt KRiffChunkNameRiff = RiffChunkName('R','I','F','F');
const TInt KRiffChunkNameList = RiffChunkName('L','I','S','T');
const TInt KRiffChunkNameAvih = RiffChunkName('a','v','i','h');
const TInt KRiffChunkTypeAvi  = RiffChunkName('A','V','I',' ');
const TInt KRiffChunkTypeHdrl = RiffChunkName('h','d','r','l');
const TInt KRiffChunkTypeStrl = RiffChunkName('s','t','r','l');
const TInt KRiffChunkNameStrh = RiffChunkName('s','t','r','h');
const TInt KRiffChunkNameStrf = RiffChunkName('s','t','r','f');
const TInt KRiffChunkTypeVids = RiffChunkName('v','i','d','s');
const TInt KRiffChunkTypeAuds = RiffChunkName('a','u','d','s');
const TInt KRiffChunkTypeMovi = RiffChunkName('m','o','v','i');
const TInt KRiffChunkName00dc = RiffChunkName('0','0','d','c');
const TInt KRiffChunkName01wb = RiffChunkName('0','1','w','b');
const TInt KRiffChunkNameIdx1 = RiffChunkName('i','d','x','1');
const TInt KFccVideoHandler  = 	RiffChunkName('X','V','I','D');
const TInt KAVIF_ISINTERLEAVED = 0x00000100;
const TInt KAVIF_HASINDEX = 0x00000010;
const TInt KWFormatTag = 1;
const TUint  KVideoId = KRiffChunkName00dc;
const TUint  KAudioId = KRiffChunkName01wb;


//Fast write for non-aligned 16 bit data.
LOCAL_C TUint32 Write16(TUint8*& aPtr, TInt16 aValue)
    {
    *aPtr++ = (TUint8)aValue;
	*aPtr++ = (TUint8)(aValue >> 8);
	return TUint32(2);
    }


//Fast write for non-aligned 32 bit data.
LOCAL_C TUint32 Write32(TUint8*& aPtr,TInt aValue)
    {
	*aPtr++ = (TUint8)aValue;
	*aPtr++ = (TUint8)(aValue >> 8);
	*aPtr++ = (TUint8)(aValue >> 16);
	*aPtr++ = (TUint8)(aValue >> 24);
	return TUint32(4);
    }
    

//Fast read for non-aligned 16 bit data.
LOCAL_C TUint16 Read16(const TUint8* aPtr)
    {
    TUint16 x = *aPtr++;
    x |= *aPtr++ << 8;
    return x;
    } 


//Fast read for non-aligned 32 bit data.
LOCAL_C TUint32 Read32(const TUint8* aPtr)
    {
    TUint32 x = *aPtr++;
    x |= *aPtr++ << 8;
    x |= *aPtr++ << 16;
    x |= *aPtr++ << 24;
    return x;
    }    
  

//Creates AviWriter object.
CAviWriter* CAviWriter::NewL(CMMFClip& aClip, MAviWriterObserver& aObserver)
    {
    CAviWriter* self = new(ELeave)CAviWriter(aClip,aObserver);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }
    


//Constructor of CAviWriter
CAviWriter::CAviWriter(CMMFClip& aClip,MAviWriterObserver& aObserver)
          :MDataSource(KNullUid),iObserver(aObserver),iClip(&aClip),
          iMoviSize(4), iClipAlreadyExists(EFalse),iAudioEnabled(EFalse)
    {
    Mem::FillZ(&iMainHeader,sizeof(TAviMainHeader));
    iMainHeader.iFlags = KAVIF_HASINDEX; // default is to write an idx1 chunk to ensure interoperability
    Mem::FillZ(iStreamHeader,2*sizeof(TAviStreamHeader));
    Mem::FillZ(&iPcmWaveFormat, KPcmWaveFormatSize);
    Mem::FillZ(&iBitmapInfoHeader,sizeof(TBitmapInfoHeader));
    }
    


//Destructor of CAviWriter
CAviWriter::~CAviWriter()
    {
    delete iSourceBuffer;
    delete iReadBuffer;	
    iBufIdx1.Close();
    }

 


//Resets the current clip position to 0.
void CAviWriter::Reset()
    {
    iCurrentClipPosition = 0;
    iMaxClipSize = 0;
	// DEF088332: MVS UI Application does not update the timer during video recording
    iFrameCount = 0;
	}
    

void CAviWriter::ConstructL()
	{
    iSourceBuffer = CMMFDescriptorBuffer::NewL(KReadBufferSize);
    if (iClip->Size())
    	{
    	iClipAlreadyExists = ETrue;
    	iClip->SourcePrimeL();
    	iReadBuffer = CMMFDescriptorBuffer ::NewL(KReadBufferSize);
    	ReadFormatL();
    	iClip->SourceStopL();
    	}
    }
 

//Writes header information if clip is new ,else retains the old header information.
void CAviWriter::SetDefaultHeaderParametersL()
	{
	if (!iClipAlreadyExists)
		{
		//If this is a new file, set initial data parameters, else retain the existing settings.
		//This situation can occur if the file has been deleted 	
		SetNumberOfStreamsL();
		WriteHeaderL();
		iClipAlreadyExists = ETrue;
		}
	else
		{
		iCurrentClipPosition = iOffset;
		iSourceBuffer->Data().SetLength(KReadBufferSize);	
		iBnewRec = ETrue;
		}
	}




//Reads the header information in the clip.This is needed if we try to append to the already recorded file.
TInt CAviWriter::ReadFormatL()
	{
	iClip->ReadBufferL(iReadBuffer,0);
	TUint8* rawform = &(iReadBuffer->Data()[0]);
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
	TUint hdr1Offset = rawform - &(iReadBuffer->Data()[0]);
	rawform+=4;
	TUint32 hdr1ListSize = Read32(rawform);
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
	//are read separately.
	
	TUint flags = Read32(rawform); //Ignore for now except AVIF_HASINDEX.
	rawform+=4;
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
		rawform += 20;	// 'LIST' + (SIZE) + 'strl' + 'strh' + (SIZE)
		
		iStreamHeader[i].iFccType= Read32(rawform);rawform+=4;
		iStreamHeader[i].iFccHandler= Read32(rawform);rawform+=4;
		iStreamHeader[i].iFlags= Read32(rawform);rawform+=4;
		iStreamHeader[i].iReserved= Read32(rawform);rawform+=4;
		iStreamHeader[i].iInitialFrames= Read32(rawform);rawform+=4;
		iStreamHeader[i].iScale= Read32(rawform);rawform+=4;
		iStreamHeader[i].iRate= Read32(rawform);rawform+=4;
		iStreamHeader[i].iStart= Read32(rawform);rawform+=4;
		iStreamHeader[i].iLength= Read32(rawform);rawform+=4;
		iStreamHeader[i].iSuggestedBufferSize= Read32(rawform);rawform+=4;
		iStreamHeader[i].iQuality= Read32(rawform);rawform+=4;
		iStreamHeader[i].iSampleSize= Read32(rawform);rawform+=4;
		iStreamHeader[i].iRcFrame.iLeft= Read16(rawform);rawform+=2;
		iStreamHeader[i].iRcFrame.iTop= Read16(rawform);rawform+=2;
		iStreamHeader[i].iRcFrame.iRight= Read16(rawform);rawform+=2;
		iStreamHeader[i].iRcFrame.iBottom= Read16(rawform);rawform+=2;
		rawform += 8;	// 'strf' + SIZE
		if (iStreamHeader[i].iFccType == KRiffChunkTypeAuds)
			{
			iPcmWaveFormat.iWaveFormat.iFormatTag= Read16(rawform);rawform+=2;
			iPcmWaveFormat.iWaveFormat.iNChannels= Read16(rawform);rawform+=2;
			iPcmWaveFormat.iWaveFormat.iNSamplesPerSec= Read32(rawform);rawform+=4;
			iPcmWaveFormat.iWaveFormat.iNAvgBytesPerSec= Read32(rawform);rawform+=4;
			iPcmWaveFormat.iWaveFormat.iNBlockAlign= Read16(rawform);rawform+=2;
			iPcmWaveFormat.iBitsPerSample= Read16(rawform);rawform+=2;
			}
		else
			{
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
			}			
		}
	
	if (flags & KAVIF_HASINDEX)
		{
		iMainHeader.iFlags |= KAVIF_HASINDEX;
		
		// skip over the hdr1 and movi lists
		TUint8* rawform2 = &(iReadBuffer->Data()[hdr1Offset + 8 + hdr1ListSize]);

		if (Read32(rawform2) != KRiffChunkNameList)
			{
			return KErrCorrupt; 	// no movi list
			}
		iMoviPos = rawform2 - &(iReadBuffer->Data()[0]);
		rawform2 += 4;
		iMoviSize = Read32(rawform2);
		
		// read the existing idx1 chunk
		TInt idx1Pos = iMoviPos + 8 + iMoviSize;
		iClip->ReadBufferL(iReadBuffer, idx1Pos);
		
		rawform2 = &(iReadBuffer->Data()[0]);
		if (Read32(rawform2) != KRiffChunkNameIdx1)
			{
			return KErrCorrupt;
			}
		rawform2 += 4;
		TUint32 idx1ChunkSize = Read32(rawform2);		
		rawform2 += 4;
		
		iBufIdx1.Zero();
		if (iBufIdx1.MaxLength() < idx1ChunkSize)
			{
			iBufIdx1.ReAllocL(idx1ChunkSize);	
			}

		iBufIdx1.Append(rawform2, Min(idx1ChunkSize, iReadBuffer->BufferSize() - 8));
		while (iBufIdx1.Length() < idx1ChunkSize)
			{
			TInt currentClipOffset = idx1Pos + 8 + iBufIdx1.Length();
			iClip->ReadBufferL(iReadBuffer, currentClipOffset);
			rawform2 = &(iReadBuffer->Data()[0]);
			iBufIdx1.Append(rawform2, Min(idx1ChunkSize - iBufIdx1.Length(), iReadBuffer->BufferSize()));
			}

		//clip the idx1 chunk off the existing clip
		iOffset = idx1Pos;

		if (iMainHeader.iStreams == 2)
			{
			iAudioEnabled = ETrue;
			}

		// restore iReadBuffer			
		iClip->ReadBufferL(iReadBuffer, 0);
		}
	else
		{
		// we can't write an idx1 chunk because the entries are missing for the existing movi data
		iMainHeader.iFlags &= ~KAVIF_HASINDEX;
		iOffset = iClip->Size();
		}
	
	return KErrNone;
	}


//Sets the maximum clip size
void CAviWriter::SetMaximumClipSizeL(TInt aFileSize)
    {
    if (aFileSize <= 0)
    	{
    	User::Leave(KErrArgument);
    	}
	iMaxClipSize = aFileSize;
    }  
    


//Gets the remaining time available for recording
void CAviWriter::GetRecordTimeAvailableL(TTimeIntervalMicroSeconds& /*aTime*/)
    {
    //This will leave with KErrNotsupported as there is no support to seek the position
    //to calculate the available record time.
     User::Leave(KErrNotSupported);
    }    

 

//Sets the position to record from.
void CAviWriter::SetPositionL(TTimeIntervalMicroSeconds aPosition)	
	{
	if (aPosition == 0)
		{
		iCurrentClipPosition = 0;	
		}
	else if (aPosition == Duration())
		{
		iCurrentClipPosition = iClip->Size();		
		}
	else
		{
		User::Leave(KErrNotSupported);
		}	
	}    
 


//Gives the length of the Main header of.avi file.
     
//gives the length of the LIST hdrl - excluding <LIST><chunk size> i.e. 8 bytes
TUint CAviWriter::HdrlLen()
	{
	TUint16 temp =  4 /*hdrl*/ +
					4 /*avih*/ + 4 /*chunk sz*/	+ sizeof(TAviMainHeader);
	TUint16 len=0;
	for (TUint8 i = 0; i < iMainHeader.iStreams; i++)
		{
		//<LIST><chunk size><strl-fourCC> 
		len += 4 + 4 + 4;

		//<strh> <chunk size><stream header>
		len += 4 + 4 + sizeof(TAviStreamHeader);

		//<strf><chunk size><stream format>
		len += 4 + 4 + (iStreamHeader[i].iFccType == KRiffChunkTypeAuds ? KPcmWaveFormatSize:sizeof(TBitmapInfoHeader));
		}
	temp += len;
	return temp;
	}	



//Writes the .avi header information to the file.This writes the .avi main header and Stream header(audio and video).
void CAviWriter::WriteHeaderL() 	
	{
	//Read the data from buffer,and write into a temporary buffer.From the temporary buffer write it 
	//to a file using writeBufferL.
	iSourceBuffer->Data().SetLength(KReadBufferSize);
	iBufPtr = &(iSourceBuffer->Data()[0]);
	iOffset+=Write32(iBufPtr,KRiffChunkNameRiff);
	iOffset+=Write32(iBufPtr,0);
	iOffset+=Write32(iBufPtr,KRiffChunkTypeAvi);
	iOffset+=Write32(iBufPtr,KRiffChunkNameList);
	iOffset+=Write32(iBufPtr,HdrlLen());
	iOffset+=Write32(iBufPtr,KRiffChunkTypeHdrl);
	iOffset+=Write32(iBufPtr,KRiffChunkNameAvih);
	iOffset+=Write32(iBufPtr,sizeof(TAviMainHeader));

	iOffset+=Write32(iBufPtr,iMainHeader.iMicroSecPerFrame);
	iOffset+=Write32(iBufPtr,iMainHeader.iMaxBytesPerSec);  
	iOffset+=Write32(iBufPtr,iMainHeader.iPaddingGranularity);
	iOffset+=Write32(iBufPtr,iMainHeader.iFlags);
	iOffset+=Write32(iBufPtr,iMainHeader.iTotalFrames);
	iOffset+=Write32(iBufPtr,iMainHeader.iInitialFrames);
	iOffset+=Write32(iBufPtr,iMainHeader.iStreams);
	iOffset+=Write32(iBufPtr,iMainHeader.iSuggestedBufferSize);
	iOffset+=Write32(iBufPtr,iMainHeader.iWidth);
	iOffset+=Write32(iBufPtr,iMainHeader.iHeight);
	/* Main header reserved fields */
	iOffset+=Write32(iBufPtr,iMainHeader.iReserved[0]);
	iOffset+=Write32(iBufPtr,iMainHeader.iReserved[1]);
	iOffset+=Write32(iBufPtr,iMainHeader.iReserved[2]);
	iOffset+=Write32(iBufPtr,iMainHeader.iReserved[3]);
	
	for (TUint8 i=0; i < iMainHeader.iStreams; i++)
		{	
		iOffset+=Write32(iBufPtr,KRiffChunkNameList);
		TUint32 temp =	4/*strl*/ + 
				4/*strh*/ + 4 /*chunk size*/ + sizeof(TAviStreamHeader) +
				4 /*strf*/ + 4 /*chunk size*/ + 
				(iStreamHeader[i].iFccType == KRiffChunkTypeAuds ? KPcmWaveFormatSize : sizeof(TBitmapInfoHeader));
		iOffset+=Write32(iBufPtr,temp);
		iOffset+=Write32(iBufPtr,KRiffChunkTypeStrl);
		iOffset+=Write32(iBufPtr,KRiffChunkNameStrh);
		iOffset+=Write32(iBufPtr,sizeof(TAviStreamHeader));
		iOffset+=Write32(iBufPtr,iStreamHeader[i].iFccType);
		iOffset+=Write32(iBufPtr,iStreamHeader[i].iFccHandler);
		iOffset+=Write32(iBufPtr,iStreamHeader[i].iFlags);
		iOffset+=Write32(iBufPtr,iStreamHeader[i].iReserved);
		iOffset+=Write32(iBufPtr,iStreamHeader[i].iInitialFrames);
		iOffset+=Write32(iBufPtr,iStreamHeader[i].iScale);
		iOffset+=Write32(iBufPtr,iStreamHeader[i].iRate);
		iOffset+=Write32(iBufPtr,iStreamHeader[i].iStart);
		iOffset+=Write32(iBufPtr,iStreamHeader[i].iLength);
		iOffset+=Write32(iBufPtr,iStreamHeader[i].iSuggestedBufferSize);
		iOffset+=Write32(iBufPtr,iStreamHeader[i].iQuality);
		iOffset+=Write32(iBufPtr,iStreamHeader[i].iSampleSize);
		iOffset+=Write16(iBufPtr,iStreamHeader[i].iRcFrame.iLeft);
		iOffset+=Write16(iBufPtr,iStreamHeader[i].iRcFrame.iTop);
		iOffset+=Write16(iBufPtr,iStreamHeader[i].iRcFrame.iRight);
		iOffset+=Write16(iBufPtr,iStreamHeader[i].iRcFrame.iBottom);
		iOffset+=Write32(iBufPtr,KRiffChunkNameStrf);
		if (iStreamHeader[i].iFccType == KRiffChunkTypeAuds)
			{
			iOffset+=Write32(iBufPtr,KPcmWaveFormatSize);
			iOffset+=Write16(iBufPtr,iPcmWaveFormat.iWaveFormat.iFormatTag);
			iOffset+=Write16(iBufPtr,iPcmWaveFormat.iWaveFormat.iNChannels);
			iOffset+=Write32(iBufPtr,iPcmWaveFormat.iWaveFormat.iNSamplesPerSec);
			iOffset+=Write32(iBufPtr,iPcmWaveFormat.iWaveFormat.iNAvgBytesPerSec);
			iOffset+=Write16(iBufPtr,iPcmWaveFormat.iWaveFormat.iNBlockAlign);
			iOffset+=Write16(iBufPtr,iPcmWaveFormat.iBitsPerSample);
			}
		else
			{
			iOffset+=Write32(iBufPtr,sizeof(TBitmapInfoHeader));
			iOffset+=Write32(iBufPtr,iBitmapInfoHeader.iBiSize);
			iOffset+=Write32(iBufPtr,iBitmapInfoHeader.iBiWidth);
			iOffset+=Write32(iBufPtr,iBitmapInfoHeader.iBiHeight);
			iOffset+=Write16(iBufPtr,iBitmapInfoHeader.iBiPlanes);
			iOffset+=Write16(iBufPtr,iBitmapInfoHeader.iBiBitCount);
			iOffset+=Write32(iBufPtr,iBitmapInfoHeader.iBiCompression);
			iOffset+=Write32(iBufPtr,iBitmapInfoHeader.iBiSizeImage);
			iOffset+=Write32(iBufPtr,iBitmapInfoHeader.iBiXPelsPerMeter);
			iOffset+=Write32(iBufPtr,iBitmapInfoHeader.iBiYPelsPerMeter);
			iOffset+=Write32(iBufPtr,iBitmapInfoHeader.iBiClrUsed);
			iOffset+=Write32(iBufPtr,iBitmapInfoHeader.iBiClrImportant);
			}			
		}
	
	//code for adding movi
	iMoviPos = iOffset;
	iOffset  +=Write32(iBufPtr,KRiffChunkNameList);
	iOffset  +=Write32(iBufPtr,iMoviSize);
	iOffset+=Write32(iBufPtr,KRiffChunkTypeMovi);
	iSourceBuffer->Data().SetLength(iOffset);
	iClip->WriteBufferL(iSourceBuffer,0);
	iSourceBuffer->Data().SetLength(KReadBufferSize);
	iCurrentClipPosition = iOffset;
 	iBnewRec = ETrue;
	}




//Creates a new 'rec' if file is interleaved.
void CAviWriter::BeforeDataWriteL(TUint8 aMediaIdx)
	{
	if (iDataWritten[aMediaIdx])
		{
		OnRecordCompleteL();
		}

	if (iBnewRec)
		{
		iBufPtr = &iSourceBuffer->Data()[0];
		iBnewRec= EFalse;			
		}
	}



//Writes the encoded video data to the file.
void CAviWriter::WriteVideoDataL(TVideoOutputBuffer* aBuffer)
	{
	BeforeDataWriteL(KMediaVideoIdxValue);
	TUint32 len = aBuffer->iData.Length();
	const TUint8* sourceBuf = aBuffer->iData.Ptr();
	if (iMaxClipSize > 0)
		{
		TInt currentClipLength = static_cast<CMMFClip*>(iClip)->Size();
		if ((currentClipLength + len) >= iMaxClipSize)
			{
			User::Leave(KErrEof);
			}
		}

	if (iMainHeader.iFlags & KAVIF_HASINDEX)
		{
		// add info to idx1 buffer
		iBufIdx1.ReAllocL(iBufIdx1.Length() + KIdx1EntrySize);
		TUint8* currentPtr = iBufIdx1Entry;
		Write32(currentPtr, KVideoId);	// chunk id
		Write32(currentPtr, 0);			// flags
		Write32(currentPtr, iOffset - iMoviPos - 8);	// offset
		Write32(currentPtr, len);		// length
		iBufIdx1.Append(iBufIdx1Entry, KIdx1EntrySize);
		}
		
	//write videoid
	iOffset+=Write32(iBufPtr,KVideoId);
	
	//write length
	iOffset+=Write32(iBufPtr,len);
	
	//write data
	Mem::Copy(iBufPtr,sourceBuf,len); 
	iOffset += len;
	iBufPtr += len;
	if (len & 0x01) //2 byte word boundary
		{
		*iBufPtr = 0x00;
		iOffset++;
		iBufPtr++;
		len++;
		}

	AfterDataWriteL(KMediaVideoIdxValue,len);
	iFrameCount++;
	}


//Writes the encoded audio data to the file.
void CAviWriter::WriteAudioDataL(CMMFBuffer* aBuffer)
	{
	BeforeDataWriteL(KMediaAudioIdxValue);
	TUint32 len = ((CMMFDataBuffer*)aBuffer)->Data().Length();
	const TUint8* sourceBuf = ((CMMFDataBuffer*)aBuffer)->Data().Ptr();
	if (iMaxClipSize > 0)
		{
		TInt currentClipLength = static_cast<CMMFClip*>(iClip)->Size();
		if ((currentClipLength + len) >= iMaxClipSize)
			{
			User::Leave(KErrEof);
			}
		}

	if (iMainHeader.iFlags & KAVIF_HASINDEX)
		{
		// add info to idx1 buffer
		iBufIdx1.ReAllocL(iBufIdx1.Length() + KIdx1EntrySize);
		TUint8* currentPtr = iBufIdx1Entry;
		Write32(currentPtr, KAudioId);	// chunk id
		Write32(currentPtr, 0);			// flags
		Write32(currentPtr, iOffset - iMoviPos - 8);	// offset
		Write32(currentPtr, len);		// length
		iBufIdx1.Append(iBufIdx1Entry, KIdx1EntrySize);
		}
	
	//write audioid
	iOffset+=Write32(iBufPtr,KAudioId);
	
	//write length
	iOffset+=Write32(iBufPtr,len);
	
	//write data
	Mem::Copy(iBufPtr,sourceBuf,len); 
	iOffset += len;
	iBufPtr += len;
	if (len & 0x01) //2 byte word boundary
		{
		*iBufPtr = 0x00;
		iOffset++;
		iBufPtr++;
		len++;
		}
	AfterDataWriteL(KMediaAudioIdxValue,len);
	}


//Writes the encoded audio data to the file.
void CAviWriter::AfterDataWriteL(TUint8 aMediaIdx,TUint aDataLen)
	{
	//check for the completion of record.if complete add junk data required;write rec size 
	//in recSizePos	
	iRecLen += aDataLen + 8; //8 - 4 for fourcc, 4 for size bytes
	iMoviSize += aDataLen + 8;
	iDataWritten[aMediaIdx] = ETrue;
	TBool recordComplete = ETrue;
	
	if (iMainHeader.iFlags == KAVIF_ISINTERLEAVED)
		{ //note:this part doesn't add junk bytes
		for (TUint8 i=0; i < iMainHeader.iStreams; i++)
			{
			if (!iDataWritten[i])
				{
				recordComplete = EFalse;
				break;
				}	
			}
		}
	
	if(recordComplete)
		{
		OnRecordCompleteL();
		}

	}


//Writes the encoded audio and video data to a file.
void CAviWriter::OnRecordCompleteL()
	{
	if (iDataWritten[KMediaAudioIdxValue])
		{
		iObserver.AudioWritten(ETrue);			
		}

	iDataWritten[0] = EFalse;iDataWritten[1]= EFalse;
	
	iClip->WriteBufferL(iOffset - iCurrentClipPosition,
						iSourceBuffer,
						iCurrentClipPosition,
						this);
	
	iCurrentClipPosition = iOffset;	
	iBnewRec= ETrue;
	iRecLen = 0;
	}


//Updates the header information in the clip.
void CAviWriter::UpdateHeadersL()
	{	
	CMMFDescriptorBuffer* SourceUpdateBuffer = CMMFDescriptorBuffer::NewL(Max(iBufIdx1.Length(), KUpdateHeaderSize));
	CleanupStack::PushL(SourceUpdateBuffer);
	
	//write file size
	SourceUpdateBuffer->Data().SetLength(KUpdateHeaderSize);
	iBufPtr = &SourceUpdateBuffer->Data()[0]; //LIST<size>movi
	if (iMainHeader.iFlags & KAVIF_HASINDEX)
		{
		//file size = iOffset - (8 bytes for RIFF chunk name and size) + (8 bytes for idx1 chunk name and size) + idx1 data length
		Write32(iBufPtr, iOffset + iBufIdx1.Length());
		}
	else
		{
		//write (file size - 8) - consider <RIFF><file size><data>
		Write32(iBufPtr, iOffset - 8);
		}
	iClip->WriteBufferL(SourceUpdateBuffer,KUpdateHeaderSize);
	
	//write frame count
	iBufPtr = &SourceUpdateBuffer->Data()[0];
	//Append it with already existing frame count value.
	iMainHeader.iTotalFrames += iFrameCount;
	// DEF088332: MVS UI Application does not update the timer during video recording
	iFrameCount = 0;
		
	Write32(iBufPtr,iMainHeader.iTotalFrames);
	iClip->WriteBufferL(SourceUpdateBuffer,KTotalFramesPos);
	
	// Update streams length
	iBufPtr = &SourceUpdateBuffer->Data()[0];
	iStreamHeader[0].iLength = iMainHeader.iTotalFrames;
	Write32(iBufPtr, iStreamHeader[0].iLength);
	iClip->WriteBufferL(SourceUpdateBuffer,KVideoLengthPos);
	if (iAudioEnabled)
		{
		iBufPtr = &SourceUpdateBuffer->Data()[0];
		iStreamHeader[1].iLength = iMainHeader.iTotalFrames  * ((iStreamHeader[1].iRate / iStreamHeader[1].iScale) / 
									(iStreamHeader[0].iRate / iStreamHeader[0].iScale));
		Write32(iBufPtr, iStreamHeader[1].iLength);
		iClip->WriteBufferL(SourceUpdateBuffer,KAudioLengthPos);
		}
	
	//Micro seconds 
	//write frame intervals
	iBufPtr = &SourceUpdateBuffer->Data()[0];
	Write32(iBufPtr,iMainHeader.iMicroSecPerFrame);
	iClip->WriteBufferL(SourceUpdateBuffer,KMicroSecPerFramePos);

	//write movi size
	iBufPtr = &SourceUpdateBuffer->Data()[0];
	Write32(iBufPtr,iMoviSize);
	iClip->WriteBufferL(SourceUpdateBuffer, iMoviPos+4);

	if (iMainHeader.iFlags & KAVIF_HASINDEX)
		{
		// write idx1 chunk name
		SourceUpdateBuffer->Data().SetLength(KUpdateHeaderSize);
		iBufPtr = &SourceUpdateBuffer->Data()[0];
		Write32(iBufPtr, KRiffChunkNameIdx1);
		iClip->WriteBufferL(SourceUpdateBuffer, iOffset);
		
		// write idx1 chunk size
		iBufPtr = &SourceUpdateBuffer->Data()[0];
		Write32(iBufPtr, iBufIdx1.Length());
		iClip->WriteBufferL(SourceUpdateBuffer, iOffset + 4);

		// write idx1 chunk data
		if (iBufIdx1.Length())
			{
			SourceUpdateBuffer->Data().SetLength(iBufIdx1.Length());
			iBufPtr = &SourceUpdateBuffer->Data()[0];
			TPtr8 bufferPtr(iBufPtr, iBufIdx1.Length());
			bufferPtr.Copy(iBufIdx1);
			iClip->WriteBufferL(SourceUpdateBuffer, iOffset + 8);
			}
		}
	
	CleanupStack::PopAndDestroy(SourceUpdateBuffer);
	}
	

//Set the videoframerate in the file
void CAviWriter::SetVideoFrameRate(TReal32 aFramesPerSecond)
	{
	iStreamHeader[0].iRate = aFramesPerSecond;
    iStreamHeader[0].iScale = 1;
	iMainHeader.iMicroSecPerFrame = KOneMillion/aFramesPerSecond;
	}



//Set the videoframesize in the file
void CAviWriter::SetVideoFrameSize(TSize aFrameSize)
	{
	iMainHeader.iHeight =  aFrameSize.iHeight;
	iMainHeader.iWidth  =  aFrameSize.iWidth;
	iBitmapInfoHeader.iBiWidth = aFrameSize.iWidth;
	iBitmapInfoHeader.iBiHeight = aFrameSize.iHeight;
	iBitmapInfoHeader.iBiSizeImage = aFrameSize.iWidth*aFrameSize.iHeight*3;
	}



//Set audio enabled for recording.
void CAviWriter::SetAudioEnabledL(TBool aAudioEnabled)
	{
	iAudioEnabled = aAudioEnabled;
	}


//Set the number of streams in the file.If audio is enabled, consider it as an interleaved file.
//Number of streams will be 1 if only video is recorded.If both audio and video are recorded,then
//the number of streams will be 2.
void CAviWriter::SetNumberOfStreamsL()
	{
	if (iAudioEnabled)
		{
		iMainHeader.iStreams = 2;
		iStreamHeader[0].iFccType = KRiffChunkTypeVids; 
		iStreamHeader[1].iFccType = KRiffChunkTypeAuds;
		SetDefaultVideoHeaders();
		SetDefaultAudioHeaders();
		}	
	else
		{
		iMainHeader.iStreams = 1;
		iStreamHeader[0].iFccType = KRiffChunkTypeVids; 
		SetDefaultVideoHeaders();
		}	
	}	


//Set the default values for audio stream header.
void CAviWriter::SetDefaultAudioHeaders()
	{
	iStreamHeader[1].iRate = KDefaultSampleRate;
	iStreamHeader[1].iScale = 1;
	iPcmWaveFormat.iWaveFormat.iFormatTag = KWFormatTag;
	iPcmWaveFormat.iWaveFormat.iNChannels = KDefaultChannels;
	iPcmWaveFormat.iWaveFormat.iNSamplesPerSec=KDefaultSampleRate;
	iPcmWaveFormat.iBitsPerSample =KDefaultBitsPerSample;
	iPcmWaveFormat.iWaveFormat.iNBlockAlign = (iPcmWaveFormat.iWaveFormat.iNChannels*iPcmWaveFormat.iBitsPerSample)/8;
	iPcmWaveFormat.iWaveFormat.iNAvgBytesPerSec = (iPcmWaveFormat.iWaveFormat.iNSamplesPerSec *
													iPcmWaveFormat.iWaveFormat.iNBlockAlign);
	iStreamHeader[1].iSampleSize = iPcmWaveFormat.iWaveFormat.iNBlockAlign;
	}


//Set the default values for video stream header.
void CAviWriter::SetDefaultVideoHeaders()
	{
	iStreamHeader[0].iFccHandler = KFccVideoHandler;
	iStreamHeader[0].iSampleSize =0;
	iBitmapInfoHeader.iBiBitCount = 24;
	iBitmapInfoHeader.iBiPlanes = 1;
	iBitmapInfoHeader.iBiCompression =KFccVideoHandler;
	iBitmapInfoHeader.iBiSize = sizeof(TBitmapInfoHeader);
	}

TInt CAviWriter::GetAudioBitRate()
	{
	//we always use this default audio bitrate.
	return (KDefaultBitsPerSample*KDefaultChannels*KDefaultSampleRate);
	}
	
void CAviWriter::SetAudioBitRateL(TInt& aBitRate)
	{
	//if user passes bitrate other than default bit rate, leave with KErrNotSupported
	if(aBitRate != GetAudioBitRate())
		{
		User::Leave(KErrNotSupported);
		}
	}	


//Calculates the duration of the file.

TTimeIntervalMicroSeconds CAviWriter::Duration()
	{
	// DEF088332: MVS UI Application does not update the timer during video recording
	// if we're recording a new clip, iMainHeader.iTotalFrames will be zero until UpdateHeadersL()
	// is called (i.e. when we stop recording) therefore add iFrameCount here and reset it
	// in UpdateHeadersL() and/or in Reset()
	if (iStreamHeader[0].iRate == 0)
		{
		return 0;
		}
	return(static_cast<TInt64>(iMainHeader.iTotalFrames + iFrameCount)* static_cast<TInt64>(1000000))/iStreamHeader[0].iRate;
	}

// from MDataSource
void CAviWriter::FillBufferL(CMMFBuffer* /*aBuffer*/, MDataSink* /*aSupplier*/, TMediaId /*aMediaId*/)
	{
	User::Leave(KErrNotSupported);
	}
	
TBool CAviWriter::CanCreateSourceBuffer()
	{
	return EFalse;
	}
	
TFourCC CAviWriter::SourceDataTypeCode(TMediaId /*aMediaId*/)
	{
	return 0;	
	}

CMMFBuffer* CAviWriter::CreateSourceBufferL(TMediaId /*aMediaId*/, TBool& /*aReference*/)
	{
	return NULL;
	}

void CAviWriter::ConstructSourceL( const TDesC8& /*aInitData*/)
	{
	User::Leave(KErrNotSupported);
	}


//Inform the observer the data is written to the file.
void CAviWriter::BufferEmptiedL(CMMFBuffer* aBuffer)
	{
	if (aBuffer)
		{
		iObserver.BufferEmptied();	
		}
	}
