// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "oggdecode.h"

COggDecode* COggDecode::NewL(CMMFClip& aClip, MOggDecodeObserver& aObserver)
    {
    COggDecode* self = new(ELeave)COggDecode(aClip, aObserver);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }
    
COggDecode::COggDecode(CMMFClip& aClip, MOggDecodeObserver& aObserver)
	: MDataSink(KNullUid), iObserver(aObserver), iClip(&aClip)
    {
    }

COggDecode::~COggDecode()
    {
#ifdef SYMBIAN_SEP_HEAP
	if(iVorbisHeap)
		{
		User::SwitchHeap(iVorbisHeap);
		}
#endif
    delete iPager;
    delete iStream;
    delete iInfo;
#ifdef SYMBIAN_SEP_HEAP
    if(iVorbisHeap)
    	{
    	iVorbisChunk.Close();
    	User::SwitchHeap(iDefaultHeap);
    	}
#endif    
    delete iSourceBuffer;
    delete iDataStore;
    }

void COggDecode::ConstructL()
    {
#ifdef SYMBIAN_SEP_HEAP    
    User::LeaveIfError(iVorbisChunk.CreateLocal(KInitialChunkSize, KMaxChunkSize, EOwnerThread));
    iVorbisHeap = User::ChunkHeap(iVorbisChunk, KMinHeapLength);
    iDefaultHeap = &User::Heap();
#endif
    VORBIS_TRAPD(leaveErr, iPager = COggPager::NewL());
    User::LeaveIfError(leaveErr);
    VORBIS_TRAP(leaveErr, iStream = COggStream::NewL());
    User::LeaveIfError(leaveErr);
    VORBIS_TRAP(leaveErr, iInfo = CVorbisInfo::NewL());
    User::LeaveIfError(leaveErr);
    iSourceBuffer = CMMFPtrBuffer::NewL();
    iDataStore = new(ELeave) unsigned char[8000*sizeof(unsigned char)];
    /*
     Initialize vorbis stream only when there is some data in the clip. this is required because
     recorderutility tries to load both play and record controllers even when new file is opened
     */
    if(iClip->Size())
    	{
    	iPlayWindowEnd = iClip->Size();
    	iClip->SourcePrimeL();
    	VORBIS_TRAP(leaveErr, InitializeVorbisL());
    	User::LeaveIfError(leaveErr);
    	iPlayWindowStart = iHeaderSize;
    	/* 
    	 Incase of vorbis, we need to send the three header packets to the codec for its initialization
    	 so Reset OggDecode sothat headers go to codec.
    	 */
    	ResetL(); 
    	}
    else
    	{
    	iClip->SourceStopL();
    	}
    }
    
/**
 * Attempts to initialise a vorbis stream from the source. The stream must be
 * freshly opened/reset. Returns ETrue iff the source contains valid vorbis. 
 */
void COggDecode::InitializeVorbisL()
    {
    ASSERT(iStream->GetSerialNo() == -1);
    for (TInt i = 0; i < KNumHeaders; ++i)
        {
        GetPacketSyncL();
        iInfo->InitializeL(iPacket);
        }
    }

/*
 Read the next packet from the source synchronously. When this returns, iPacket
 will contain a pointer to the next packet. We use this synchronous read when reading the headers.
 */
void COggDecode::GetPacketSyncL()
    {
    while (ETrue)
        {
        TInt err = iStream->PacketOut(iPacket);
        if (err == KErrNone)
            {
            return;
            }
        else if(err == KErrNotFound) //no packet in the stream.
            {
            err = iPager->NextPage(iPage);
                      
            if (err==KErrNone)
                {
                iHeaderSize = iHeaderSize+iPage.iHead.Size()+iPage.iBody.Size();
                User::LeaveIfError(iStream->PageIn(iPage));
                }
            else // need more data
                {
                //we get the buffer from the libogg and then fill it with the data from the file.
                TPtr8 buf(NULL, 0);
                User::LeaveIfError(iPager->GetBuffer(buf, KReadBufferSize));
                iSourceBuffer->SetPtr(buf);
                iClip->ReadBufferL(iSourceBuffer, iSourcePos);
                TInt len = iSourceBuffer->Data().Length();
                if (len==0) 
                	{
                	//Received Eof even before reading all the headers. File is Corrupt.
                	User::Leave(KErrCorrupt);
                	}
                iSourcePos+=len;
                User::LeaveIfError(iPager->DataWritten(iSourceBuffer->Data()));
                }
            }
        else
        	{
        	User::Leave(err);
        	}
        }
    }
    
/**
Reset the clip.
*/    
void COggDecode::ResetL()
    {
    iSourcePos = 0;
    iStoredSourcePos = iPlayWindowStart;
    SetPositionL(iPlayStartTime);
    iPlayWindowEndReached = EFalse;
    iAllDataSent = EFalse;
    iCurrentGranulePos = iPlayStartGranulePos;
#ifdef SYMBIAN_SEP_HEAP
	User::SwitchHeap(iVorbisHeap);
#endif
    iPager->Reset();
    iStream->Reset();
#ifdef SYMBIAN_SEP_HEAP
    User::SwitchHeap(iDefaultHeap);
#endif
    iClip->SourceStopL();
    iInitSeek = 0;
    }

void COggDecode::ResetFile()
	{
	iSourcePos = iPlayWindowStart;
	iCurrentGranulePos = iPlayStartGranulePos;
	}
	
void COggDecode::SetPlayWindowL(TTimeIntervalMicroSeconds aStart, TTimeIntervalMicroSeconds aEnd)
    {
    if((aStart == iPlayStartTime) && (aEnd == iPlayEndTime))
    	{
    	return;
    	}
    //Convert the given time in micro seconds into granulePos to pass it to the GetStreamPositionL()
    iPlayWindowStart = GetStreamPositionL(aStart.Int64() * iInfo->SampleRate()/1000000, iPlayStartGranulePos);
    iPlayStartTime = aStart;
    SetPositionL(aStart);
    iPlayWindowEnd = GetStreamPositionL(aEnd.Int64() * iInfo->SampleRate()/1000000, iPlayEndGranulePos);
    iPlayEndTime = aEnd;
    }
    
void COggDecode::ClearPlayWindowL()
    {
    iPlayWindowStart = 0;
    iPlayStartTime = TTimeIntervalMicroSeconds(0);
    SetPositionL(iPlayStartTime);
    iPlayWindowEnd = iClip->Size();
    iPlayEndTime = iDuration;
    }
    
TBool COggDecode::IsAllDataSent()
    {
    return iAllDataSent;
    }

void COggDecode::FillThisBufferL(CMMFBuffer* aBuffer)
    {
    /*
     loop for case where seek has taken place let the initialisation packets be sent then modify to new position.
     Reason is when seek the Vorbis HwDevice is reset so need to resend the initialisation packets and then instead of 
     reading first datapacket modify the iSourcePos variable(which contains the position to read from the file) to be 
     the seeked value 
     */   
     if(iSeek)
    	{
    	if (iInitSeek == 0)
	    	{
	    	iInitSeek = iHeaderSize;
	    	}
	    if( iInitSeek > KReadBufferSize)
		    {
		    iInitSeek = iInitSeek - KReadBufferSize;
		    }
		else if((iInitSeek > 0) && (iInitSeek <= KReadBufferSize))
			{
			iInitSeek = iInitSeek - KReadBufferSize;
			}	
    	else
			{
			//all 3 init packets are read, set to start reading from seeked position in file
			iSourcePos = iStoredSourcePos;
			iInitSeek=0;
			iSeek=EFalse;
			}
    	}
 	
    if (!CMMFBuffer::IsSupportedDataBuffer(aBuffer->Type()))
    	{
    	User::Leave(KErrNotSupported);	
    	}
    iBufferFromSink = static_cast<CMMFDataBuffer*>(aBuffer);
   
    iClip->ReadBufferL(iBufferFromSink, iSourcePos, this);
    }

//from MDataSink
void COggDecode::BufferFilledL(CMMFBuffer* aBuffer)
	{
	if (!CMMFBuffer::IsSupportedDataBuffer(aBuffer->Type()))
		{
		User::Leave(KErrNotSupported);
		}
	TInt len = iBufferFromSink->Data().Length();
    iSourcePos+=len;
    // If all the data sent
    if(len < iBufferFromSink->Data().MaxLength() || iSourcePos >= iPlayWindowEnd)
	    {//for playwindow we need to set this flag explicitly for the codec to recognize
	    if(iSourcePos > iPlayWindowEnd)
	    	{
	    	CMMFPtrBuffer* ptrBuf = static_cast<CMMFPtrBuffer*>(aBuffer);
	    	SetLastPageFlag(ptrBuf);
	    	}
	    iBufferFromSink->SetLastBuffer(ETrue);
	    iAllDataSent = ETrue;
	    }
    iObserver.BufferFilled();
 	}  
 
TFourCC COggDecode::SinkDataTypeCode(TMediaId /*aMediaId*/)
	{
	return TFourCC('V', 'O', 'R', 'B');
	}
		
void COggDecode::EmptyBufferL(CMMFBuffer* /*aBuffer*/, MDataSource* /*aSupplier*/, TMediaId /*aMediaId*/)
	{
	User::Leave(KErrNotSupported);
	}
	
TBool COggDecode::CanCreateSinkBuffer()
	{
	return EFalse;	
	}

CMMFBuffer* COggDecode::CreateSinkBufferL(TMediaId /*aMediaId*/, TBool& /*aReference*/)
	{
	return NULL;	
	}

void COggDecode::ConstructSinkL( const TDesC8& /*aInitData*/)
	{
	User::Leave(KErrNotSupported);
	}
	
/**
 Reads header of an oggpage in the buffer to retrieve the granulepos.It searches for a page header and returns position found in buffer 
 and the last pcm sample in the associated buffer. 
 @param  aSourceBuffer
        The source buffer in which the oggpage exists
 @param  aPos
 		The Positon of the start of the oggpage in the buffer
 @return The granuleposition in the oggpage header
 */
TInt64 COggDecode::ReadHeader(CMMFPtrBuffer* aSourceBuffer, TInt &aPos)
	{
	TInt64 granulePos = -1;
	TInt i = 0;
	const TInt granulePosStartByte = 7;
	const TInt granulePosEndByte = 14; 
	const TInt granulePosSizeinBytes = 8;
	_LIT8(KOggS, "OggS");
	
	TInt pos = aSourceBuffer->Data().Find(KOggS);
	if(pos != KErrNotFound)
		{
		//found OggS. Skip version and headertype bytes  
   		pos = pos + (granulePosStartByte - 1);
   		
   		// we can only extract the granulepos if it is present in this buffer otherwise search
   		//for another oggpage in the next buffer. So the positioning is not exact at the moment.
   		if(pos + granulePosSizeinBytes - 1 < aSourceBuffer->BufferSize())
   			{
   			//Extract and reconstruct the 64bit number 
	  		//holds 7 chars to make up granulePos
	  		TPtr8 temp = aSourceBuffer->Data().MidTPtr(pos, granulePosSizeinBytes - 1);
			granulePos = aSourceBuffer->Data()[pos+granulePosSizeinBytes - 1];
	  		for(i = granulePosSizeinBytes - 2;i >= 0;--i)
	  			{
	  			granulePos = granulePos << 8 | temp[i];
	  			}
   			//Position of start of the page within buffer 	
   			aPos = pos-granulePosEndByte;
   			}
		}
   	
   	//returns the number of last pcm sample in the packet 
   	return granulePos;	//-1 means there is no vorbis header in buffer 
	}

void COggDecode::SetLastPageFlag(CMMFPtrBuffer* aSourceBuffer)
	{
	const TInt KEndOfStreamBytePos = 5;
	const TUint KEndOfStreamFlag = 0x0004;
	_LIT8(KOggS, "OggS");
	
	TInt pos = aSourceBuffer->Data().Find(KOggS);
	if(pos != KErrNotFound)
		{
		if(pos + KEndOfStreamBytePos < aSourceBuffer->BufferSize())
   			{
	  		aSourceBuffer->Data()[pos + KEndOfStreamBytePos] = aSourceBuffer->Data()[pos + KEndOfStreamBytePos] | KEndOfStreamFlag;
	  		}
		}
   	}
	  
TTimeIntervalMicroSeconds COggDecode::DurationL()
    {
    if(!iClip->Size())
    	{
    	return TTimeIntervalMicroSeconds(0);
    	}
    if(!iDurationCalculated)
    	{
    	//Average OggPage Size
		TInt bufSize = 0x1000;
		
		//Pointer Buffer to read from Source
		CMMFPtrBuffer* sourceBuffer = CMMFPtrBuffer::NewL();
    	CleanupStack::PushL(sourceBuffer);
   		
		HBufC8* tmp = HBufC8::NewLC(bufSize);
		TPtr8 tmpPoint = tmp->Des();
		sourceBuffer->SetPtr(tmpPoint);
  		
  		//Construct the Num Samples result
    	TInt64 granulePos = -1;
      	TInt n = 1;
    	TInt pos = 0;
    
    	//Get total length of file     	
    	TInt sourceBytes = iClip->Size();
    
    	if(sourceBytes < bufSize)
    		{
    		bufSize = sourceBytes;
    		}
    	iClip->SourcePrimeL();
    	
    	while(granulePos == -1)
    		{
    		if(sourceBytes - (bufSize*n) < 0)//we have completed the whole file without finding granulePos
    			{
    			iClip->ReadBufferL(sourceBuffer, iHeaderSize);
    			granulePos = ReadHeader(sourceBuffer, pos);
    			if((granulePos == -1) || (granulePos == 0))
    				{
    				granulePos = 0;//we dont have any data.only header
    				}
    			break;
    			}
    		
    		else
    			{
    			iClip->ReadBufferL(sourceBuffer, (sourceBytes - (bufSize*n)));
    			}
    
			//returns -1 if no pcm position data otherwise returns the number of last pcm sample in vorbus packet
    		granulePos = ReadHeader(sourceBuffer, pos);
    	  			
   			n++;
    		}
   		iClip->SourceStopL();
		CleanupStack::PopAndDestroy(2, sourceBuffer); //tmp and sourceBuffer
 		iEndGranulePos = granulePos;
 		//Convert the granulePos into the time in micro secs
 		TInt64 duration = (granulePos * 1000000)/iInfo->SampleRate();
    	iDuration = TTimeIntervalMicroSeconds(duration);
    	iDurationCalculated = ETrue;
    	}
	
    return iDuration;
    }

CVorbisInfo* COggDecode::Info()
	{
	return iInfo;	
	}

void COggDecode::SetPositionL(TTimeIntervalMicroSeconds aPosition)
	{
	if(aPosition == 0)
		{
		//This is start of file so no need to search through file 
		iStoredSourcePos = 0;
		iCurrentGranulePos = 0;
		iSeek = EFalse;
		return;
		}
	if(aPosition == iDuration)
		{
		//This is end of file so no need to search
		iStoredSourcePos = iClip->Size();
		iCurrentGranulePos = iEndGranulePos;
		iSeek = EFalse;
		return;
		}
	//Convert the given time in micro seconds into granulePos to pass it to the GetStreamPositionL()
	TInt64 samples = (aPosition.Int64() * iInfo->SampleRate())/1000000;
	
	//Control variable set to show seeking is taking place,
	iSeek=ETrue;	
	iStoredSourcePos = GetStreamPositionL(samples, iCurrentGranulePos);
	}

//Seeking using GranulePos if we know the exact granulePos to seek in the file. Mainly used during pause.
void COggDecode::SetPositionL(TInt64 aGranulePos)
	{
	if(aGranulePos == 0)
		{
		//This is start of file so no need to search through file 
		iStoredSourcePos = 0;
		iCurrentGranulePos = 0;
		iSeek = EFalse;
		return;
		}
	if(aGranulePos == iEndGranulePos)
		{
		//This is end of file so no need to search
		iStoredSourcePos = iClip->Size();
		iCurrentGranulePos = iEndGranulePos;
		iSeek = EFalse;
		return;
		}
	
	//Control variable set to show seeking is taking place,
	iSeek=ETrue;	
	iStoredSourcePos = GetStreamPositionL(aGranulePos, iCurrentGranulePos);
	}

TTimeIntervalMicroSeconds COggDecode::SeekingTime()
	{
	return (iCurrentGranulePos*1000000)/iInfo->SampleRate();
	}

TInt COggDecode::GetStreamPositionL(TInt aSamples, TInt64& aGranulePos)
	{
	if(aSamples == 0)
		{
		aGranulePos = 0;
		return iHeaderSize;
		}
	//Average OggPage size is from 4000 bytes to 5000 bytes.
	TInt bufSize = 4096;
	TInt totalBytes = iClip->Size();
	TInt bytes = 0;
	if(totalBytes < bufSize)
    	{
    	bufSize = totalBytes;
    	}
	CMMFPtrBuffer* buffer = CMMFPtrBuffer::NewL();
    CleanupStack::PushL(buffer);
    HBufC8* tmp = HBufC8::NewLC(bufSize);
	TPtr8 tmpPoint = tmp->Des();
    buffer->SetPtr(tmpPoint);
    
    TInt64 granulePos = -1;
    TInt64 lowerGranulePos = 0;
    TInt upperLimit = totalBytes; 
    TInt lowerLimit = iHeaderSize;
    TInt upperPagePos = 0;
    TInt pos = 0;
    TBool clipOpened = EFalse;
    TBool pageFound = ETrue;
    TBool reachedEnd = EFalse;
        
    //Check if the clip is opened. There is no method in iClip which tells us whether clip is opened or not.
    //When we try to read the data from the clip before opening, we get KErrNotReady. I am doing the check before
    //loop inorder to avoid the TRAP inside the loop.
    TRAPD(err, iClip->ReadBufferL(buffer, 0));
    if(err != KErrNone)
		{
		if(err == KErrNotReady)
			{
			iClip->SourcePrimeL();
			clipOpened = ETrue;
			}
		else
			{
			User::Leave(err);
			}
		}
    
    /* 
    SEEK METHOD :- 
    1) Determine size of the file in bytes, then ReadBuffer from middle of source file and determine PCM sample count(granulePos) at this point.
    
    2) Based on this information move through the file by add/subtract constantly halving amounts of bytes depending on whether 
    the result pcm samples count read from file is greater/less than the desired samples count.
    
    3) Store the determined offset for the file which addresses the correct oggpage in the file.
    */
	while(upperLimit - lowerLimit > 0)
    	{
    	if(pageFound)
    		{
    		bytes = (upperLimit - lowerLimit)/2;
    		bytes = lowerLimit+bytes;
    		}
    	iClip->ReadBufferL(buffer, bytes);
    	if(bytes + bufSize > upperLimit)//do not consider bytes beyond upperLimit
    		{
    		buffer->Data().SetLength(upperLimit-bytes);
    		}    		
    	pos = -1;
    	granulePos = -1;
    	granulePos = ReadHeader(buffer, pos);
    	if(pos == -1 && granulePos == -1)//No page found
    		{
    	   	if(reachedEnd)
    	   		{
    	   		break;
    	   		}
    	   	if(bytes + bufSize >= upperLimit)//already reached upperlimit and no page in the right half
    	   		{
    	   		upperLimit = bytes+14;//search on the left half
    	   		bytes = lowerLimit;
	    	   	pageFound = EFalse; //indicate that bytes value need not be calculated in the next cycle
    	   		reachedEnd = ETrue;
    	   		}
    	   	else
    	   		{
    	   		bytes = bytes+bufSize-14;//inorder to avoid loosing partial OggS+granulePos(14 bytes) from the prev. buffer if exists
    	   		pageFound = EFalse; //indicate that bytes value need not be calculated in the next cycle
    	   		}
    	   	continue;
    	   	}
    	else if(pos != -1 && granulePos == -1) //page is found but granulePos is out of the buffer
    		{
    		bytes = bytes+14; //includes granulePos in the next cycle
    	   	pageFound = EFalse; //indicate that bytes value need not be calculated in the next cycle
    		continue;
    		}   	 	
    	pageFound = ETrue; //if we reach here, page is found   	
    	if(aSamples > granulePos)
    		{
    		lowerGranulePos = granulePos;
    		lowerLimit = bytes+pos;
    		}
    	else
    		{
    		upperPagePos = bytes+pos;
    		upperLimit = bytes+pos;
    		if(aSamples == granulePos)
    			{
    			break;
    			}
    		}
    	if(reachedEnd)
	   		{
	   		break;
	   		}
    	}
    if(clipOpened)
    	{
    	iClip->SourceStopL();
    	}
 	aGranulePos = lowerGranulePos;
 	CleanupStack::PopAndDestroy(2, buffer);//tmp and buffer
 	return upperPagePos;
	}
