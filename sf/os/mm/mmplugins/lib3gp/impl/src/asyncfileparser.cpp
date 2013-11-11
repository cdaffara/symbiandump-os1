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

// INCLUDE FILES
#include <e32base.h>
#include <f32file.h>
#include <f32file64.h>
#include <3gplibrary/mp4lib.h>
#include "mp4atom.h"
#include "mp4memwrap.h"
#include "asyncfileparser.h"

// MACROS
// Debug print macro
#if defined(_DEBUG) && defined(_ASYNCFILEPARSERLOGGING)
#include <e32svr.h>
#define PRINT(x)
#else
#define PRINT(x)
#endif

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CFileAsyncParser::CFileAsyncParser
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CFileAsyncParser::CFileAsyncParser() : CActive( EPriorityHigh ), iDiskBufferPointer(NULL,0)
    {
    
    }

// -----------------------------------------------------------------------------
// CFileAsyncParser::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CFileAsyncParser::ConstructL( MP4HandleStruct* aHandle, RFile64& aFile )
    {
    PRINT(_L("CFileAsyncParser::ConstructL() IN"));
    iError = KErrNone;
    iInputFile = &aFile;
    iHandle = aHandle;    
	iAudioSize = 0;
	iReturnedAudioFrames = 0;
	iAudioTimeStamp = 0; 		
	iAudioTimeStamp2 = 1;   // always fill timestamp2 too (null = dont fill)
	iAllDataInMemory = EFalse; 
	
	if ( iHandle->readBufferSize == 0)
	{
		iReadBufferSize = READBUFSIZE;
	}
	else
	{
		iReadBufferSize = iHandle->readBufferSize;
	}	
	
	iDiskBuffer = HBufC8::NewL(iReadBufferSize);
	iCurrentDiskReadPosition = 0;
	iCurrentBufferReadPosition = 0;
    CActiveScheduler::Add(this);
    
    PRINT(_L("CFileAsyncParser::ConstructL() OUT"));
    }

// -----------------------------------------------------------------------------
// CFileAsyncParser::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CFileAsyncParser* CFileAsyncParser::NewL( MP4HandleStruct* aHandle, RFile64& aFile )
    {
    CFileAsyncParser* self = new(ELeave) CFileAsyncParser;
    CleanupStack::PushL(self);
    self->ConstructL( aHandle, aFile );
    CleanupStack::Pop(self);
    return self;
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
CFileAsyncParser::~CFileAsyncParser()
    {
    PRINT(_L("CFileAsyncParser::~CFileAsyncParser() in"));

    if ( IsActive() )
        {
        if ( iAsyncReadOngoing )
            {
            Cancel();
            }
        }

    delete iDiskBuffer;
    PRINT(_L("CFileAsyncParser::~CFileAsyncParser() out"));
    }

// -----------------------------------------------------------------------------
// CFileAsyncParser::ReadAudioFrames(  );
// Writes incoming buffer data to internal buffers for writing to disk.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CFileAsyncParser::ReadAudioFrames( mp4_u8 *buffer, mp4_i64 aPosition, mp4_u32 aBytesToRead )
	{
	PRINT(_L("CFileAsyncParser::ReadAudioFrames()"));
	iProcessingAudio = ETrue;
	return ReadDataAsync( buffer, aPosition, aBytesToRead );
	}

// -----------------------------------------------------------------------------
// CFileAsyncParser::ReadVideoFrame( );
// Writes incoming buffer data to internal buffers for writing to disk.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CFileAsyncParser::ReadVideoFrame( mp4_u8* buffer, mp4_i64 aPosition, mp4_u32 aBytesToRead )
	{
	PRINT(_L("CFileAsyncParser::ReadVideoFrame()"));
	iProcessingAudio = EFalse;
	return ReadDataAsync( buffer, aPosition, aBytesToRead );
	}


// -----------------------------------------------------------------------------
// CFileAsyncParser::ReadDataAsync(  );
// Reads data from file asynchronously.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CFileAsyncParser::ReadDataAsync( mp4_u8 *buffer, mp4_i64 aPosition, mp4_u32 aBytesToRead )
	{
	PRINT(_L("CFileAsyncParser::ReadDataAsync() in"));
	iBuffer = buffer;
	if ( iAsyncReadOngoing )
		{
		return -1; // only one async read can be ongoing at one time;	
		}
	if (!iDiskBuffer)
		{
		return -1;
		}
		
  	// Is the new seek point inside the current disk buffer?
  	if ( (iCurrentDiskReadPosition > aPosition) &&  
  		 (( iCurrentDiskReadPosition - iDiskBuffer->Length() ) <= aPosition  ))
  		{
    	// Yes
	    iCurrentBufferReadPosition = iDiskBuffer->Length() - (iCurrentDiskReadPosition - aPosition);
  		}
  	else
  		{
    	// No, set current position and zero diskbuffer
		iCurrentBufferReadPosition = 0;
		iCurrentDiskReadPosition = (mp4_i64)aPosition;
		iDiskBuffer->Des().SetLength(0);
		}
		
	iBytesToRead = aBytesToRead;
	iBytesRead = 0;
	TInt available = 0;
	
	// How much data is available in diskbuffer.
	available =  iDiskBuffer->Length() - iCurrentBufferReadPosition;
	if (available > iBytesToRead)
	{
	  available = iBytesToRead;
	}

	// If any available copy it first to output buffer
	if (available ) 
	{
	  memcpy(iBuffer, iDiskBuffer->Ptr() + iCurrentBufferReadPosition, available);
	  iCurrentBufferReadPosition += available;
	  iBytesRead += available;
	}

	// If we got everything from diskbuffer process it right away
	if (iBytesRead == iBytesToRead)
	{
		PRINT(_L("CFileAsyncParser::ReadDataAsync() Data found in memory, no need to read file - return right away"));
		iAllDataInMemory = ETrue;
		SetActive();
        TRequestStatus* tmp = &iStatus;
        User::RequestComplete(tmp, KErrNone);
        PRINT(_L("CFileAsyncParser::ReadDataAsync() out"));
		return MP4_OK;
	}
	else
	{
		// Need to read rest of the requested data from file.
		iAllDataInMemory = EFalse;
	}

	// Determine used readbuffer size
	if ( iHandle->readBufferSize == 0)
	{
		iReadBufferSize = READBUFSIZE;
	}
	else
	{
		iReadBufferSize = iHandle->readBufferSize;
	}
	
	// Increase disk read buffer size if requested frames are larger than current disk buffer.
	if ( (iBytesToRead > iReadBufferSize ) || (iReadBufferSize != iDiskBuffer->Des().MaxLength()) )
	{
        iReadBufferSize = iBytesToRead;
        if (iDiskBuffer)
        {
            delete iDiskBuffer;
            iDiskBuffer = NULL;
            TRAPD(memerror, iDiskBuffer = HBufC8::NewL(iReadBufferSize));
            if (memerror)
            {
	            return MP4_OUT_OF_MEMORY;
            }
            else
            {
				iCurrentBufferReadPosition = 0;
            }
        }
	}

	iAsyncReadOngoing = ETrue;
	iDiskBufferPointer.Set(iDiskBuffer->Des());
	iCurrentDiskReadPosition = aPosition + iBytesRead;
	switch (iHandle->sourceType)
	  {
	    case MP4_SOURCE_RFILE:
	    {
		  PRINT(_L("CFileAsyncParser::ReadDataAsync() Data not in memory, reading RFile64"));	    
	      RFile64* rfile = (RFile64*)iHandle->rfile;
	      rfile->Read(iCurrentDiskReadPosition, iDiskBufferPointer, iDiskBufferPointer.MaxLength(), iStatus);
	      break;
	    }
	    case MP4_SOURCE_CAF:
	    {
		  PRINT(_L("CFileAsyncParser::ReadDataAsync() Data not in memory, reading CAF object"));	    	    
	      iHandle->cafError = iHandle->cfile->Read(iCurrentDiskReadPosition, iDiskBufferPointer, iDiskBufferPointer.MaxLength(), iStatus);
	      if ( iHandle->cafError != KErrNone)
	        return -2;
	      break;
	    }
	    default:
	      return -1;
	  }

    if ( !IsActive() )
	    {
	    SetActive();
	    }
	PRINT(_L("CFileAsyncParser::ReadDataAsync() out"));
	return 0;
	}



// -----------------------------------------------------------------------------
// CFileAsyncParser::DoCancel()
// From CActive Cancels async request.
// -----------------------------------------------------------------------------
//
void CFileAsyncParser::DoCancel()
    {
    PRINT(_L("CFileAsyncParser::DoCancel() in"));
    if (iAsyncReadOngoing)
    	{
    	if (iHandle->sourceType == MP4_SOURCE_RFILE)
    	    {
    	    // cancel read from file
    	    ((RFile64 *)(iHandle->rfile))->ReadCancel();
        	}
    	else if (iHandle->sourceType == MP4_SOURCE_CAF)
        	{
        	// cancel read from caf object
        	iHandle->cfile->ReadCancel(iStatus);
        	}
    	iAsyncReadOngoing = EFalse;
    	}
    
    PRINT(_L("CFileAsyncParser::DoCancel() out"));
    }

// -----------------------------------------------------------------------------
// CFileAsyncParser::ReturnAudioFrames()
// Return audio frames to observer.
// -----------------------------------------------------------------------------
//
void CFileAsyncParser::ReturnAudioFrames()
	{
	PRINT(_L("CFileAsyncParser::ReturnAudioFrames() in"));
	TInt error = KErrNone;
	
	// Update last accessed position in file pointer
    if (iHandle->audioSampleOffset + iHandle->audioSampleSize - 1 > iHandle->lastAccessedPosInFile)
	    {
	    iHandle->lastAccessedPosInFile = iHandle->audioSampleOffset + iHandle->audioSampleSize - 1;
	    }
	
	// Fill audio frame size
	iAudioSize = iHandle->audioSampleSize;
	
	// Fill audio timestamp information
	iAudioTimeStamp = 0;
	iAudioTimeStamp2 = 1; // fill also timestamp2 (wont be filled if 0)
	error = convertAudioSampleToTime(iHandle, iHandle->moov->trakAudio->mdia, &iAudioTimeStamp, &iAudioTimeStamp2);	
	if (error == MP4_OK)
		{
		// Fill iReturnedAudioFrames
		iReturnedAudioFrames = 0;
		error = CalculateAudioFrameCount();
		}
	
	// Move forward in audio samples
	if (error == MP4_OK)
		{
		error = advanceAudioSample(iHandle, iHandle->moov->trakAudio);
		if ( error == -1)
			{
			error = MP4_ERROR;
			}
		else if ( error == -2 )
			{
			error = MP4_OK;
			iHandle->audioLast = MP4TRUE;
			}
		}

	iAsyncReadOngoing = EFalse;
	iHandle->asyncObserver->M3GPMP4LibAudioFramesAvailable(error, 
    													  iAudioSize,
    													  iAudioTimeStamp,
    													  iReturnedAudioFrames,
    										  			  iAudioTimeStamp2);
	PRINT(_L("CFileAsyncParser::ReturnAudioFrames() out"));		
	}

// -----------------------------------------------------------------------------
// CFileAsyncParser::ReturnVideoFrame()
// Return video frame to observer.
// -----------------------------------------------------------------------------
//
void CFileAsyncParser::ReturnVideoFrame()
	{
	PRINT(_L("CFileAsyncParser::ReturnVideoFrame() in"));
	TInt error = KErrNone;
	
	// Update last accessed position in file pointer
    if (iHandle->videoFrameOffset + iHandle->videoFrameSize - 1 > iHandle->lastAccessedPosInFile)
	    {
	    iHandle->lastAccessedPosInFile = iHandle->videoFrameOffset + iHandle->videoFrameSize - 1;
	    }
	
	// Fill video frame size
	iVideoSize = iHandle->videoFrameSize;
	
	// Fill video timestamp information
	iVideoTimeStamp = 0;
	iVideoTimeStamp2 = 1; // fill also timestamp2 (wont be filled if 0)
	error = convertVideoSampleToTime(iHandle, iHandle->moov->trakVideo->mdia, &iVideoTimeStamp, &iVideoTimeStamp2);	
	if (error == MP4_OK)
		{
		// Fill iKeyFrame
		iVideoKeyFrame = 0;
		error = isVideoFrameKeyFrame(iHandle, iHandle->moov->trakVideo, &iVideoKeyFrame);
		}
	
	// Move forward in video frames
	if (error == MP4_OK)
		{
		error = advanceVideoFrame(iHandle, iHandle->moov->trakVideo);
		if ( error == -1)
			{
			error = MP4_ERROR;
			}
		else if ( error == -2 )
			{
			error = MP4_OK;
			iHandle->videoLast = MP4TRUE;
			}
		}

	iAsyncReadOngoing = EFalse;
	iHandle->asyncObserver->M3GPMP4LibVideoFrameAvailable(error,
        										   		  iVideoSize, 
        										  		  iVideoTimeStamp, 
        										   		  iVideoKeyFrame, 
        										   		  iVideoTimeStamp2);
	PRINT(_L("CFileAsyncParser::ReturnVideoFrame() out"));		
	}

// -----------------------------------------------------------------------------
// CFileAsyncParser::CalculateAudioFrameCount()
// Return video frame to observer.
// -----------------------------------------------------------------------------
//
TInt CFileAsyncParser::CalculateAudioFrameCount()
	{
  	mp4_i32 frameLength = 0;
  	mp4_u32 numOfFrames = 0;
  	mp4_u8 *framepointer = 0;
  	mp4_u32 rawAmrFrameLength[16] = {13,14,16,18,20,21,27,32,6,0,0,0,0,0,0,1};	
	trackAtom *trak = iHandle->moov->trakAudio;
	
	if (!trak)
		{
		return -1;
		}

	/* AMR */
	if (trak->mdia->minf)
	if (trak->mdia->minf->stbl)
	  if (trak->mdia->minf->stbl->stsd)
	    if (iHandle->type & MP4_TYPE_AMR_NB)
	    {
	        framepointer = iBuffer;
	        numOfFrames = 0;
	        while ( iBytesRead > 0 )
	        {
	            frameLength = rawAmrFrameLength[(TInt)(((*framepointer) & 0x78) >> 3)];
	            if ( frameLength == 0)
	            {
	                return -4;
	            }
	            iBytesRead -= frameLength;
	            framepointer += frameLength;
	            numOfFrames++;
	        }
	        iReturnedAudioFrames = numOfFrames;
	    }
	    else if (iHandle->type & MP4_TYPE_AMR_WB)
	    {
		  /* Return the number of sample entries listed for this particular sample entry index */
	      if (trak->mdia->minf->stbl->stsd->sawb[iHandle->audioSampleEntryIndex - 1])
	        if (trak->mdia->minf->stbl->stsd->sawb[iHandle->audioSampleEntryIndex - 1]->damr)
	          iReturnedAudioFrames = trak->mdia->minf->stbl->stsd->sawb[iHandle->audioSampleEntryIndex - 1]->damr->framesPerSample;
	    }
	    else
	    {
	    }

	/* MPEG-4 audio */
	if (trak->mdia->minf)
	if (trak->mdia->minf->stbl)
	  if (trak->mdia->minf->stbl->stsd)
	    if (trak->mdia->minf->stbl->stsd->mp4a[iHandle->audioSampleEntryIndex - 1])
	      iReturnedAudioFrames = 1;

	/* QCELP 13K as QCELPSampleEntry*/
	if (trak->mdia->minf)
	if (trak->mdia->minf->stbl)
	  if (trak->mdia->minf->stbl->stsd)
	    if ((iHandle->type & MP4_TYPE_QCELP_13K) && (!iHandle->qcelpStoredAsMPEGAudio))
	    {
		  /* Return the number of sample entries listed for this particular sample entry index */
	      if (trak->mdia->minf->stbl->stsd->sqcp[iHandle->audioSampleEntryIndex - 1])
	        if (trak->mdia->minf->stbl->stsd->sqcp[iHandle->audioSampleEntryIndex - 1]->dqcp)
	          iReturnedAudioFrames = trak->mdia->minf->stbl->stsd->sqcp[iHandle->audioSampleEntryIndex - 1]->dqcp->framesPerSample;
	    }

	/* QCELP 13K as MPEG-4 audio */
	if (trak->mdia->minf)
	if (trak->mdia->minf->stbl)
	  if (trak->mdia->minf->stbl->stsd)
	    if (trak->mdia->minf->stbl->stsd->mp4a[iHandle->audioSampleEntryIndex - 1])
	      iReturnedAudioFrames = 1;

	return MP4_OK;	
	}

// -----------------------------------------------------------------------------
// CFileAsyncParser::RunL()
// From CActive Called when async request completes.
// -----------------------------------------------------------------------------
//
void CFileAsyncParser::RunL()
    {
    PRINT(_L("CFileAsyncParser::RunL() in"));
    if ( iStatus != KErrNone )
        {
        PRINT((_L("CFileAsyncParser::RunL() error in previous async: %d "), iStatus.Int() ));
        iError = iStatus.Int();
		iHandle->asyncObserver->M3GPMP4LibAudioFramesAvailable(MP4_FILE_ERROR,0,0,0,0);
        return;
        }

    if (!iAllDataInMemory)
	    {
		if ((mp4_u32)iDiskBuffer->Length() == 0) // EOF or error
		{
			iError = MP4_FILE_ERROR; // metadata info doesn't match file -> corrupted clip.
		}
		
		memcpy(iBuffer+iBytesRead, iDiskBuffer->Ptr(), iBytesToRead-iBytesRead);
	  	iCurrentBufferReadPosition += iBytesToRead-iBytesRead;
	  	iCurrentDiskReadPosition += iDiskBuffer->Length();
		iBytesRead = iBytesToRead;
	  	
	  	// set handle disk buffer sizes to zero just in case.
	  	iHandle->diskReadBufPos = 0;
		iHandle->diskReadSize = 0;
		iHandle->diskReadBufStart = 0;
		iHandle->diskReadPos = iCurrentDiskReadPosition;
    	}

	if ( iProcessingAudio )
		{
		ReturnAudioFrames();
		}
	else
		{
		ReturnVideoFrame();	
		}
    
    PRINT(_L("CFileAsyncParser::RunL() out"));
    }

//  End of File  
