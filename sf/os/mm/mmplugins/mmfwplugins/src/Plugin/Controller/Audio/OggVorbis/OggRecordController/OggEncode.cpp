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

#include "OggEncode.h"

//COggEncode methods
COggEncode* COggEncode::NewL(MOggEncodeObserver& aObserver, CMMFClip& aClip)
    {
    COggEncode* self = new(ELeave)COggEncode(aObserver, aClip);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }
    
COggEncode::COggEncode(MOggEncodeObserver& aObserver, CMMFClip& aClip)
	:MDataSource(KNullUid), iObserver(aObserver), iClip(&aClip)
    {
    }

void COggEncode::ConstructL()
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
    iSinkBuffer = CMMFPtrBuffer::NewL();
    iClip->SinkPrimeL();//this creates the clip
    // we need to check if the clip already exists
	if(iClip->Size())
		{
		//exists. so we need to get the header information and store it in iInfo
		iClipAlreadyExists = ETrue;
		VORBIS_TRAP(leaveErr, InitializeVorbisL());
		//set position to the end of the file sothat new data gets appended to old one.
		iCurrentPosition = iClip->Size();
		}
	else
		{
    	//new clip! so set to default configuration. Client can set them using the customcommands later
    	VORBIS_TRAP(leaveErr, iInfo->InitializeL(KOggVorbisDefaultSampleRate, KOggVorbisDefaultBitRate, KMMFFormatDefaultChannels));
		}
	iClip->SinkStopL(); //release it otherwise playcontroller will fail to open for read
	}
    
COggEncode::~COggEncode()
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
    delete iSinkBuffer;
    }

/**
 * Reads next oggpacket synchronously. This method is used to read the header packets
 * when a clip is opened for append. 
 */
void COggEncode::GetPacketSyncL()
    {
    while (ETrue)
        {
        TInt err = iStream->PacketOut(iPacket);//try to retrieve an oggpacket from the stream
        if (KErrNone==err)
            {
            iHeaderSize = iHeaderSize + iPacket.iData.Size();
            return;
            }
        else   //err == KErrNotFound only
            {
            err = iPager->NextPage(iPage);//try to retrieve an oggpage from the stream
            if (err==KErrNone)
                {
                User::LeaveIfError(iStream->PageIn(iPage));
                }
            else // no oggpage . so read more data from the clip
                {
                //we get the buffer from the libogg and then fill it with the data from the file.
                TPtr8 buf(NULL, 0);
                User::LeaveIfError(iPager->GetBuffer(buf, KReadBufferSize));
                iSinkBuffer->SetPtr(buf);
                iClip->ReadBufferL(iSinkBuffer, iCurrentPosition);
                TInt len = iSinkBuffer->Data().Length();
                if (len==0) 
                	{
                	//Received Eof even before reading all the headers. File is Corrupt.
                	User::Leave(KErrCorrupt);
                	}
                iCurrentPosition+=len;
                User::LeaveIfError(iPager->DataWritten(iSinkBuffer->Data()));
                }
            }
        }
    }
    
/**
 * Attempts to initialise a vorbis stream from the sink. 
 * Return ETrue iff the sink contains valid vorbis. 
 */
void COggEncode::InitializeVorbisL()
    {
    // vorbis has 3 header packets
    for (TInt i = 0; i < KMaxNumHeaders; ++i)
        {
        GetPacketSyncL();
        iInfo->InitializeL(iPacket);
        }
    }

TBool COggEncode::IsAllDataWritten()
    {
    return iAllDataWritten;;
    }
    
CVorbisInfo* COggEncode::Info()
	{
	return iInfo;	
	}
	
void COggEncode::EmptyThisBufferL(CMMFBuffer* aBuffer)
    {
    if (!CMMFBuffer::IsSupportedDataBuffer(aBuffer->Type())) 
    	{
    	User::Leave(KErrNotSupported);	
    	}
    CMMFDataBuffer* buf = static_cast<CMMFDataBuffer*>(aBuffer);
    if (iMaxClipSize > 0)
		{
		TInt currentClipLength = static_cast<CMMFClip*>(iClip)->Size();
		TInt currentBufSize = buf->Data().Length();
		if ((currentClipLength + currentBufSize) >= iMaxClipSize)
			{
			iAllDataWritten = ETrue;
			User::Leave(KErrEof);
			}
		}
    DEBUG1("PageCount %d ",++iPageCount);
    DEBUG1("CurrentPostion %d ",iCurrentPosition);
    if(buf->LastBuffer())
    	{
    	iAllDataWritten = ETrue;
    	}
    DEBUG1("Page length %d ",buf->Data().Length());
    iClip->WriteBufferL(buf, iCurrentPosition, this);
    }

void COggEncode::GetRecordTimeAvailableL(TTimeIntervalMicroSeconds& aTime)
    {
    //TimeLeft = BytesFree/BytesUsedPerSec(approx.). We use bitratenominal
    //here. so the recordtimeavailable is just an approximation.
    if(iMaxClipSize > 0)
    	{
    	aTime = (iMaxClipSize - iClip->Size())/(8 * iInfo->BitRateNominal());
    	}
    else
    	{
    	aTime = (iClip->BytesFree())/(8 * iInfo->BitRateNominal());
    	}
    }
    
void COggEncode::SetMaximumClipSizeL(TInt aFileSize)
    {
    if( aFileSize <= 0 )
    	{
    	User::Leave(KErrArgument);
    	}
	iMaxClipSize = aFileSize;
    }
    
void COggEncode::Reset()
    {
    iCurrentPosition = 0;
    iCurrentGranulePos = 0;
    iHeaderSize = 0;
    iMaxClipSize = 0;
    iStream->Reset();
    iPager->Reset();
    iAllDataWritten = EFalse;
    iDurationCalculated = EFalse;
    }
    
void COggEncode::GetSupportedSampleRatesL(RArray<TUint>& aSampleRates)
	{
	aSampleRates.Reset();
	for( TInt index =0; index < KMaxNumSampleRates; index++)
		{
		User::LeaveIfError(aSampleRates.Append(KOggVorbisSampleRates[index]));
		}
	}

void COggEncode::GetSupportedNumChannelsL(RArray<TUint>& aNumChannels)
	{
	aNumChannels.Reset();
	User::LeaveIfError(aNumChannels.Append(KMono));
	User::LeaveIfError(aNumChannels.Append(KStereo));
	}

void COggEncode::GetSupportedBitRatesL(RArray<TUint>& aBitRates)
	{
	aBitRates.Reset();
	for( TInt index =0; index < KMaxNumBitRates; index++)
		{
		User::LeaveIfError(aBitRates.Append(KOggVorbisBitRates[index]));
		}
	}
	
/**
 This method checks if the samplerate passed is a valid samplerate. Sets the sample rate on the format
 if it is valid. Otherwise returns KErrNotSupported..
 @param  aSampleRate
        The sample rate, in samples per second.
 @return One of the system-wide error codes.
 */
TInt COggEncode::SetSampleRate(TUint aSampleRate)
	{		
	TInt status = KErrNotSupported;
	if (iClipAlreadyExists) 
		{
		if (iInfo->SampleRate() != aSampleRate)
			{
			status = KErrAlreadyExists;
			}
		else
			{
			status = KErrNone;
			}
		}
	else
		{
		for( TInt index =0; index < KMaxNumSampleRates; index++)
			{
			if (aSampleRate == KOggVorbisSampleRates[index])
				{
				iInfo->SetSampleRate(aSampleRate);
				status = KErrNone;
				break;
				}
			}
		}
	return status;
	}
/**
 This method checks if the bitrate passed is a valid bitrate. Sets the bitrate on the format
 if it is valid. Otherwise returns KErrNotSupported..
 @param  aBitRateRate
        The bitrate, in bits per second.
 @return One of the system-wide error codes.
 */
TInt COggEncode::SetBitRate(TUint aBitRate)
	{		
	TInt status = KErrNotSupported;
	if (iClipAlreadyExists) 
		{
		if (iInfo->BitRateNominal() != aBitRate)
			{
			status = KErrAlreadyExists;
			}
		else
			{
			status = KErrNone;	
			}
		}
	else
		{
		for( TInt index =0; index < KMaxNumBitRates; index++)
			{
			if (aBitRate == KOggVorbisBitRates[index])
				{
				iInfo->SetBitRate(aBitRate);
				status = KErrNone;
				break;
				}
			}
		}
	return status;
	}

TInt COggEncode::SetNumChannels(TUint aNumChannels)
	{		
	TInt status = KErrNotSupported;
	if (iClipAlreadyExists) 
		{
		if (iInfo->Channels() != aNumChannels)
			{
			status = KErrAlreadyExists; 
			}
		else
			{
			status = KErrNone;	
			}
		}
	else
		{
		if(aNumChannels == KMono || aNumChannels == KStereo)
			{
			iInfo->SetChannels(aNumChannels);
			status = KErrNone;
			}
		}
	return status;
	}

TUint COggEncode::SampleRate()
    {
    return iInfo->SampleRate();
    }
    
TUint COggEncode::BitRate()
    {
    return iInfo->BitRateNominal();
    }
    
TUint COggEncode::NumChannels()
    {
    return iInfo->Channels();
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
TInt64 COggEncode::ReadHeader(CMMFPtrBuffer* aSourceBuffer, TInt &aPos)
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
   		if(pos + granulePosSizeinBytes - 1 <= aSourceBuffer->BufferSize())
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
  
TTimeIntervalMicroSeconds COggEncode::DurationL()
    {
    if(!iClip->Size())
    	{
    	return TTimeIntervalMicroSeconds(0);
    	}
    if(!iDurationCalculated)
    	{
    	//Average OggPage Size
		TInt bufSize = 5000;
		
		//Pointer Buffer to read from Source
		CMMFPtrBuffer* buffer = CMMFPtrBuffer::NewL();
    	CleanupStack::PushL(buffer);
   		
		HBufC8* tmp = HBufC8::NewLC(bufSize);
		TPtr8 tmpPoint = tmp->Des();
		buffer->SetPtr(tmpPoint);
  		
  		//Construct the Num Samples result
    	TInt64 granulePos = -1;
      	TInt n = 1;
    	TInt pos = 0;
    
    	//Get total length of file     	
    	TInt bytes = iClip->Size();
    
    	if(bytes < bufSize)
    		{
    		bufSize = bytes;
    		}
       	TBool fileOpened = EFalse;	
       	while(granulePos==-1)
    		{
			
			if(bytes - (bufSize*n) < 0)
    			{
    			//we have completed the whole file without finding granulePos. There is a buffer
    			//at the beginning of the file(size lessthan bufSize) that needs to be searched
    			iClip->ReadBufferL(buffer, 100);//skipped first 100 bytes to skip OggS of headers
    			granulePos = ReadHeader(buffer, pos);
    			if((granulePos == -1) || (granulePos == 0))
    				{
    				granulePos = 0;//we may have an oggpage.with zero granulepos
    				}
    			break;
    			}
    		
    		else
    			{
    			TRAPD(err, iClip->ReadBufferL(buffer, (bytes - (bufSize*n))));    	
    			if(err == KErrNotReady)
    				{
    				iClip->SourcePrimeL();
    				fileOpened = ETrue;
    				iClip->ReadBufferL(buffer, (bytes - (bufSize*n)));
    				}
    			}
			
			//returns -1 if no pcm position data otherwise returns the number of last pcm sample in vorbus packet
    		granulePos = ReadHeader(buffer, pos);
    	  			
   			n++;
    		}
   		if(fileOpened)
   			{
   			iClip->SourceStopL();	
   			}
   		CleanupStack::PopAndDestroy(2, buffer);
 		iEndGranulePos = granulePos;
 		//Convert the granulePos into the time in micro seconds
 		TInt64 duration = (granulePos * 1000000)/iInfo->SampleRate();
    	iDuration = TTimeIntervalMicroSeconds(duration);
    	iDurationCalculated = ETrue;
    	}
	
    return iDuration;
    }

void COggEncode::SetPositionL(TTimeIntervalMicroSeconds aPosition)
	{
	if(aPosition == 0)
		{
		//This is start of file so no need to search through file 
		iCurrentPosition = 0;
		iCurrentGranulePos = 0;
		return;
		}
	if(aPosition == iDuration)
		{
		//This is end of file so no need to search
		iCurrentPosition = iClip->Size();
		iCurrentGranulePos = iEndGranulePos;
		return;
		}
	//Convert the given time in micro seconds into granulePos to pass it to the GetStreamPositionL()
	TInt64 samples = (aPosition.Int64() * iInfo->SampleRate())/1000000;
	iCurrentPosition = GetStreamPositionL(samples, iCurrentGranulePos);
	}
	
TTimeIntervalMicroSeconds COggEncode::PositionL()
    {
    //convert the current granulePos into the time in micro seconds
    TInt64 microS=(iCurrentGranulePos * 1000000)/iInfo->SampleRate();
    return TTimeIntervalMicroSeconds(microS);
    }


TInt COggEncode::GetStreamPositionL(TInt64 aSamples, TInt64 &aGranulePos)
	{
	if(aSamples == 0)
		{
		return 0;
		}
	//Average OggPage Size
	TInt bufSize = 0x1000;
	TInt totalBytes = iClip->Size();
	totalBytes /= 2;
	TInt bytes = totalBytes;
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
    
    TInt n = 0;
    TInt pos = 0;
    
    /* 
    SEEK METHOD :- 
    1) Determine size of file in bytes, then ReadBuffer from middle of source file and detrermine PCM sample at this point
    
    2) Based on this information move through the file by add/subtract constantly halving amounts of bytes depending on whether 
    result PCM value read from file is greater/less than the desired PCM value
    
    3) Store the determined offset for the file which addresses the correct vorbis packet in the file 
    */
    
    while(granulePos == -1) //granulepos of -1 is not valid. so search until we get a valid granulepos
    	{
       	iClip->ReadBufferL(buffer, (bytes - (bufSize*n)));
       	granulePos = ReadHeader(buffer, pos);
    	n++;
    	}
    while(totalBytes > 1)
    	{
    	
    	totalBytes /= 2;//reduce the amount of bytes to be searched by half everytime we loop here
    	if(aSamples == granulePos)
    		{
    		break;
    		}
    	else if(aSamples > granulePos) 
    		{
    		bytes += totalBytes;	
    		}
    	else				  
    		{
    		bytes -= totalBytes;
    		}
    	
    	n = 0;
    	granulePos = -1;
    		
    	while(granulePos == -1)
    		{
    		iClip->ReadBufferL(buffer, (bytes - (bufSize*n)));
    		bytes -= bufSize*n;
       		granulePos = ReadHeader(buffer, pos);
   			n++; 
    		}
	   	}
    
    CleanupStack::PopAndDestroy(2, buffer);
 	aGranulePos = granulePos;
 	if(granulePos == 0)
 		{
 		return iHeaderSize;
 		}
 	return (bytes + pos);
 	}

//from MDataSink
void COggEncode::BufferEmptiedL(CMMFBuffer* aBuffer)
	{
	if (!CMMFBuffer::IsSupportedDataBuffer(aBuffer->Type()))
		{
		User::Leave(KErrNotSupported);
		}
	CMMFDataBuffer* buf = static_cast<CMMFDataBuffer*>(aBuffer);
	TInt len = buf->Data().Length();
    DEBUG1("OggPage of length  %d bytes",len);
    iCurrentPosition+=len;
    iObserver.BufferEmptied();
 	}   

TFourCC COggEncode::SourceDataTypeCode(TMediaId /*aMediaId*/)
	{
	return TFourCC('V', 'O', 'R', 'B');
	}
		
void COggEncode::FillBufferL(CMMFBuffer* /*aBuffer*/, MDataSink* /*aSupplier*/, TMediaId /*aMediaId*/)
	{
	User::Leave(KErrNotSupported);
	}
	
TBool COggEncode::CanCreateSourceBuffer()
	{
	return EFalse;	
	}

CMMFBuffer* COggEncode::CreateSourceBufferL(TMediaId /*aMediaId*/, TBool& /*aReference*/)
	{
	User::Leave(KErrNotSupported);
	return NULL;	
	}

void COggEncode::ConstructSourceL( const TDesC8& /*aInitData*/)
	{
	User::Leave(KErrNotSupported);
	}
