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
#include "mp4atom.h"
#include "metadatafilewriter.h"

// MACROS
// Debug print macro
#ifdef _DEBUG
#include <e32svr.h>
#define PRINT(x)
#else
#define PRINT(x)
#endif

const TInt KMetaDataWriterBufferSize        = (4*4096);
const TInt KMetaDataWriterInputBufCount     = 10;
const TInt KMetaDataWriterOutputBufCount    = 4;

// ============================ MEMBER FUNCTIONS ===============================

CMetaDataWriterBuffer::~CMetaDataWriterBuffer()
    {
    delete iData;
    }

// -----------------------------------------------------------------------------
// CMetaDataFileWriter::CMetaDataFileWriter
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CMetaDataFileWriter::CMetaDataFileWriter() : CActive( EPriorityHigh ) 
    {
    }

// -----------------------------------------------------------------------------
// CMetaDataFileWriter::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CMetaDataFileWriter::ConstructL( )
    {
    PRINT(_L("CMetaDataFileWriter::ConstructL() in"));

    iError = KErrNone;
    iAsyncWritingOngoing = EFalse;
    iFlush = EFalse;
    iFlushDone = EFalse;

    for (TInt i=0; i<KMetaDataWriterInputBufCount; i++)
        {
        iOutputFile.AppendL(NULL);

        CMetaDataWriterBuffer* emptyInputBuffer = new (ELeave) CMetaDataWriterBuffer;
        CleanupStack::PushL(emptyInputBuffer);
        
        emptyInputBuffer->iData = HBufC8::NewL(KMetaDataWriterBufferSize);
        emptyInputBuffer->iOutputFileNum = i;
        iInputBufferArray.AppendL(emptyInputBuffer);
        
        CleanupStack::Pop(emptyInputBuffer);
        
        iInputBufferArrayDelivered[i] = 0;
        }

    for (TInt j=0; j<KMetaDataWriterOutputBufCount; j++)
        {
        CMetaDataWriterBuffer* emptyOutputBuffer = new (ELeave) CMetaDataWriterBuffer;
        CleanupStack::PushL(emptyOutputBuffer);
        
        emptyOutputBuffer->iData = HBufC8::NewL(KMetaDataWriterBufferSize);
        emptyOutputBuffer->iOutputFileNum = -1;
        iEmptyBufferQueue.AppendL(emptyOutputBuffer);
        
        CleanupStack::Pop(emptyOutputBuffer);
        }

    CActiveScheduler::Add(this);

    PRINT(_L("CMetaDataFileWriter::ConstructL() out"));
    }

// -----------------------------------------------------------------------------
// CMetaDataFileWriter::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CMetaDataFileWriter* CMetaDataFileWriter::NewL()
    {
    PRINT(_L("CMetaDataFileWriter::NewL() in"));
    
    CMetaDataFileWriter* self = new(ELeave) CMetaDataFileWriter;
    CleanupStack::PushL(self);
    self->ConstructL( );
    CleanupStack::Pop(self);

    PRINT(_L("CMetaDataFileWriter::NewL() out"));
    return self;
    }


// Destructor
CMetaDataFileWriter::~CMetaDataFileWriter()
    {
    PRINT(_L("CMetaDataFileWriter::~CMetaDataFileWriter() in"));


    if ( IsActive() )
        {
        if ( iAsyncWritingOngoing )
            {
            Cancel();    
            }
        else
            {
            TRequestStatus* status = &iStatus;
            User::RequestComplete( status, KErrNone );
            Cancel();
            }
        }

    iOutputFile.Reset();
    iInputBufferArray.ResetAndDestroy();
    iOutputBufferQueue.ResetAndDestroy();
    iEmptyBufferQueue.ResetAndDestroy();
    PRINT(_L("CMetaDataFileWriter::~CMetaDataFileWriter() out"));
    }

// -----------------------------------------------------------------------------
// CMetaDataFileWriter::Write( const TDesC8& aBuf )
// Writes incoming buffer data to internal buffers for writing to disk.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CMetaDataFileWriter:: Write( RFile64& aFile, const TInt aFileNumber, const TDesC8& aBuf )
    {
    PRINT(_L("CMetaDataFileWriter::Write() in"));
    PRINT((_L("e_cmetadatawriter_write 1")));

    iOutputFile[aFileNumber] = &aFile;

	PRINT((_L("e_cmetadatawriter_write_adddatatobuffer 1")));
    TInt error = AddDataToBuffer( aFileNumber, aBuf );
    if ( error != KErrNone )
        {
        PRINT((_L("CMetaDataFileWriter::Write() buffer write error: %d"), error));         
        return error;
        }
	PRINT((_L("e_cmetadatawriter_write_adddatatobuffer 0")));

    PRINT((_L("CMetaDataFileWriter::Write() Write Buffer, Status: Full:%d Empty:%d "), 
                    iOutputBufferQueue.Count(), iEmptyBufferQueue.Count() ));

    if ( iAsyncWritingOngoing )
        {
        if ( iOutputBufferQueue.Count() >= KFileWriterHardBufLimit )
            {
            PRINT((_L("CMetaDataFileWriter::Write() Waiting async write to complete")));
			PRINT((_L("e_cmetadatawriter_write_waitasync 1")));            
            User::WaitForRequest( iStatus );
			PRINT((_L("e_cmetadatawriter_write_waitasync 0")));            
            PRINT((_L("CMetaDataFileWriter::Write() Async write done")));
            TRAP(error, RunL());
            if (error != KErrNone)
            	{
            	PRINT((_L("CMetaDataFileWriter::Write() runL leave, error: %d"), error));         
                return error;
            	}
            }
        }
    else
        {
        if ( iOutputBufferQueue.Count() )
            {
            PRINT(_L("CMetaDataFileWriter::Write() writing async"));       

			PRINT((_L("e_cmetadatawriter_write_startwrite 1")));
			
            iOutputFile[iOutputBufferQueue[0]->iOutputFileNum]->Write( *(iOutputBufferQueue[0]->iData), iStatus );
			PRINT((_L("e_cmetadatawriter_write_startwrite 0")));            
            iAsyncWritingOngoing = ETrue;
            if ( !IsActive() )
                {
                SetActive();
                }
            }
        }

    PRINT(_L("CMetaDataFileWriter::Write() out"));
    PRINT((_L("e_cmetadatawriter_write 0")));    
    return error;
    }

// -----------------------------------------------------------------------------
// CMetaDataFileWriter::Flush( )
// Flush internal buffers to disk.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CMetaDataFileWriter::Flush( )
    {
    PRINT(_L("CMetaDataFileWriter::Flush() in"));
    PRINT((_L("e_cmetadatawriter_flush 1")));
    PRINT((_L("CMetaDataFileWriter::Flush() FullCount: %d "), iOutputBufferQueue.Count()));

    TInt error = KErrNone;
    iFlush = ETrue;

    if ( iAsyncWritingOngoing )
        {
        PRINT((_L("CMetaDataFileWriter::Flush() Waiting async write to complete")));
        PRINT((_L("e_cmetadatawriter_flush_waitasyncstop 1")));
        User::WaitForRequest( iStatus );
		PRINT((_L("e_cmetadatawriter_flush_waitasyncstop 0")));        
        PRINT((_L("CMetaDataFileWriter::Flush() Async write done, flushing")));
        TRAP(error, RunL());
        if (error != KErrNone)
        	{
        	PRINT((_L("CMetaDataFileWriter::Flush() call runL leave, error: %d"), error));         
            return error;
        	}
        }

    while ( iOutputBufferQueue.Count() )
        {
        PRINT((_L("e_cmetadatawriter_flush_writesync 1")));
        
        error = iOutputFile[iOutputBufferQueue[0]->iOutputFileNum]->Write( *(iOutputBufferQueue[0]->iData) );
        if ( error == KErrNone )
            {
            error = iEmptyBufferQueue.Append(iOutputBufferQueue[0]);
            if (error == KErrNone)
            	{                   
            	iOutputBufferQueue[0]->iData->Des().Zero();
            	iOutputBufferQueue[0]->iOutputFileNum = -1;
            	iOutputBufferQueue.Remove(0);
            	}
            else
            	{
            	PRINT((_L("CMetaDataFileWriter::Flush() iEmptyBufferQueue.Append failed, error: %d"), error));
            	return error;
            	}
            }
        else
            {
            PRINT((_L("CMetaDataFileWriter::Flush() fullBufQueue write failed, error: %d"), error));
            iFlush = EFalse;
            return error;
            }
		PRINT((_L("e_cmetadatawriter_flush_writesync 0")));            
        }

    if ( !iFlushDone ) // Flush only once
        {
        iFlushDone = ETrue;
        for ( TInt i=0; i < KMetaDataWriterInputBufCount; i++ )
            {
            if ( iOutputFile[i] )
                {
		    	PRINT((_L("e_cmetadatawriter_flush_write_tempfiles 1")));
                error = iOutputFile[i]->Flush();
    			PRINT((_L("e_cmetadatawriter_flush_write_tempfiles 0")));
                PRINT((_L("CMetaDataFileWriter::Flush() inputbuf[%d] flush return code: %d"), i, error));
                }
	        }
        }

    iFlush = EFalse;
    PRINT((_L("CMetaDataFileWriter::Flush() FullCount: %d <= Should be 0"), iOutputBufferQueue.Count()));
    PRINT(_L("CMetaDataFileWriter::Flush() out"));
    PRINT((_L("e_cmetadatawriter_flush 0")));
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CMetaDataFileWriter::AddDataToBuffer( const TDesC8& aBuf )
// Writes incoming data to internal buffers and buffer queues..
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CMetaDataFileWriter::AddDataToBuffer(const TInt aFileNumber, const TDesC8& aBuf )
    {
    PRINT(_L("CMetaDataFileWriter::AddDataToBuffer() in"));

    TInt byteswritten = 0;
    TInt numbytes = 0;
    TInt available = 0; // Available bytes in write buffer
    TInt error = KErrNone;
    
    HBufC8* inputBuf = iInputBufferArray[aFileNumber]->iData;

    if ( iError != KErrNone )
        {
        PRINT((_L("CMetaDataFileWriter::AddDataToBuffer() out, RunL iError: %d"), iError));
        return iError;
        }

    PRINT((_L("CMetaDataFileWriter::AddDataToBuffer() Save Buffer, Size: %d "),  aBuf.Length() ));

    while (byteswritten < aBuf.Length() )
        {
        available = (inputBuf->Des()).MaxLength() - inputBuf->Length();

        if (available > 0)
            {
            numbytes = aBuf.Length() - byteswritten;
            if (numbytes > available)
                {
                numbytes = available;
                }
            inputBuf->Des().Append( aBuf.Mid( byteswritten, numbytes ) );
            byteswritten += numbytes;
            }
        else // Buffer is full, move it to outputqueue and use empty/new buf as input.
            {
            if ( iEmptyBufferQueue.Count() == 0 )
                {
                CMetaDataWriterBuffer* emptyInputBuffer = 0;

                emptyInputBuffer = new CMetaDataWriterBuffer;
                if ( !emptyInputBuffer )
                    {
                    PRINT((_L("CMetaDataFileWriter::AddDataToBuffer(), memory alloc failed")));
                    iError = KErrNoMemory;
                    break;
                    }

                TRAP(error, emptyInputBuffer->iData = HBufC8::NewL( KMetaDataWriterBufferSize ));
                if ( error != KErrNone )
                    {
                    PRINT((_L("CMetaDataFileWriter::AddDataToBuffer(), memory alloc failed: %d"), error));
                    delete emptyInputBuffer;
                    emptyInputBuffer = NULL;
                    iError = error;
                    break;
                    }
                else
                    {                    
                    error = iOutputBufferQueue.Append(iInputBufferArray[aFileNumber]);
                    if (error != KErrNone)
                    	{
                        PRINT((_L("CMetaDataFileWriter::AddDataToBuffer(), memory alloc failed: %d"), error));
                        delete emptyInputBuffer;
                        emptyInputBuffer = NULL;
                        iError = error;
                        break;
                    	}
                    
                	// old full buffer to output queue.
                    emptyInputBuffer->iOutputFileNum = aFileNumber;
                    
                    // new buffer to input.
                    iInputBufferArray[aFileNumber] = emptyInputBuffer;
                    }
                }
            else
                {
                // old full buffer to output queue.
                TInt err = iOutputBufferQueue.Append( iInputBufferArray[aFileNumber] );
                if (err != KErrNone)
                	{
                	PRINT((_L("CMetaDataFileWriter::AddDataToBuffer(), iOutputBufferQueue.Append failed: %d"), err));
                	iError = err;
                	break;
                	}
                
                // empty buffer from empty queue to input.
                iInputBufferArray[aFileNumber] = iEmptyBufferQueue[0];
                iInputBufferArray[aFileNumber]->iData->Des().Zero();
                iInputBufferArray[aFileNumber]->iOutputFileNum = aFileNumber;
                iEmptyBufferQueue.Remove(0);                
                }

            inputBuf = iInputBufferArray[aFileNumber]->iData;
            }
        }

    PRINT((_L("CMetaDataFileWriter::AddDataToBuffer() out, error: %d"), error));
    return error;
    }

// -----------------------------------------------------------------------------
// CMetaDataFileWriter::DoCancel()
// From CActive Cancels async request.
// -----------------------------------------------------------------------------
//
void CMetaDataFileWriter::DoCancel()
    {
    }

// -----------------------------------------------------------------------------
// CMetaDataFileWriter::RunL()
// From CActive Called when async request completes.
// -----------------------------------------------------------------------------
//
void CMetaDataFileWriter::RunL()
    {
    PRINT(_L("CMetaDataFileWriter::RunL() in"));
	PRINT((_L("e_cmetadatawriter_runl 1")));
    iAsyncWritingOngoing = EFalse;

    if ( iStatus == KErrNone )
        {
        iEmptyBufferQueue.AppendL( iOutputBufferQueue[0] );        
        iOutputBufferQueue[0]->iData->Des().Zero();
        iOutputBufferQueue[0]->iOutputFileNum = -1;
        iOutputBufferQueue.Remove( 0 );
        }
    else
        {
        PRINT((_L("CMetaDataFileWriter::RunL() Write error in previous async: %d "), iStatus.Int() ));
        iError = iStatus.Int();
        return;
        }

    PRINT((_L("CMetaDataFileWriter::RunL() Buffer written, Status: Full:%d Empty:%d "), iOutputBufferQueue.Count(), iEmptyBufferQueue.Count() ));
    
    if ( iFlush )
        {
        PRINT(_L("CMetaDataFileWriter::RunL() out, flushing"));
        PRINT((_L("e_cmetadatawriter_runl 0")));
        return;
        }

    if ( iOutputBufferQueue.Count() >= KFileWriterHardBufLimit )
        {
        while ( iOutputBufferQueue.Count() > KFileWriterSoftBufLimit )
            {
        	PRINT((_L("e_cmetadatawriter_runl1_write 1")));
            iError = iOutputFile[iOutputBufferQueue[0]->iOutputFileNum]->Write( *(iOutputBufferQueue[0]->iData));
        	PRINT((_L("e_cmetadatawriter_runl1_write 0")));
            if ( iError == KErrNone )
                {
                iEmptyBufferQueue.AppendL( iOutputBufferQueue[0] );
                iOutputBufferQueue[0]->iData->Des().Zero();
                iOutputBufferQueue[0]->iOutputFileNum = -1;                
                iOutputBufferQueue.Remove( 0 );
                }   
            else
                {
                PRINT((_L("CMetaDataFileWriter::RunL() Write error: %d "), iError));
                return;
                }   
            }
        }

    if ( iOutputBufferQueue.Count() >= KFileWriterSoftBufLimit )
        {
      	PRINT((_L("e_cmetadatawriter_runl2_write 1")));
        iError = iOutputFile[iOutputBufferQueue[0]->iOutputFileNum]->Write(  *(iOutputBufferQueue[0]->iData));
      	PRINT((_L("e_cmetadatawriter_runl2_write 0")));
        if ( iError == KErrNone )
            {
            iEmptyBufferQueue.AppendL( iOutputBufferQueue[0] );            
            iOutputBufferQueue[0]->iData->Des().Zero();
            iOutputBufferQueue[0]->iOutputFileNum = -1;
            iOutputBufferQueue.Remove( 0 );
            }   
        else
            {
            PRINT((_L("CMetaDataFileWriter::RunL() Write error: %d "), iError));
            return;
            }
        }

    if ( iOutputBufferQueue.Count() )
        {
      	PRINT((_L("e_cmetadatawriter_runl3_write 1")));
        iOutputFile[iOutputBufferQueue[0]->iOutputFileNum]->Write( *(iOutputBufferQueue[0]->iData), iStatus );
      	PRINT((_L("e_cmetadatawriter_runl3_write 0")));
        iAsyncWritingOngoing = ETrue;
        if ( !IsActive() )
            {
            SetActive();
            }
        }

	PRINT((_L("e_cmetadatawriter_runl 0")));
    PRINT(_L("CMetaDataFileWriter::RunL() out"));
    }

// -----------------------------------------------------------------------------
// CMetaDataFileWriter::RunError
// -----------------------------------------------------------------------------
//
 TInt CMetaDataFileWriter::RunError(TInt aError)
	 {
	 //RunL can leave.
	 iError = aError;
	 return KErrNone ;
	 }
 
// -----------------------------------------------------------------------------
// CMetaDataFileWriter::ReadBuffer
// -----------------------------------------------------------------------------
//
TInt CMetaDataFileWriter::ReadBuffer( const TInt aFileNumber, TDes8& aBuf, TInt bytestoread )
    {
    PRINT(_L("CMetaDataFileWriter::ReadBuffer() in"));
	PRINT((_L("e_cmetadatawriter_readbuffer 1")));
    TInt bytesread;
    
    PRINT((_L("CMetaDataFileWriter::ReadBuffer() iInputBufferArray[%d]->iData->Length(): %d, requested %d"), aFileNumber, iInputBufferArray[aFileNumber]->iData->Length(), bytestoread));
    if ( !iInputBufferArray[aFileNumber]->iData->Length() )
        {
        bytesread = 0;
        }
    else if ( (iInputBufferArray[aFileNumber]->iData->Length() -
        iInputBufferArrayDelivered[aFileNumber]) >= bytestoread )
        {
        bytesread = bytestoread;
        aBuf.Append( iInputBufferArray[aFileNumber]->iData->Mid( iInputBufferArrayDelivered[aFileNumber], bytesread ) );
        iInputBufferArrayDelivered[aFileNumber] += bytesread;
        
        // If the buffer is read empty, release it
        if ( iInputBufferArray[aFileNumber]->iData->Length() == iInputBufferArrayDelivered[aFileNumber] )
            {
            PRINT((_L("CMetaDataFileWriter::ReadBuffer() iInputBufferArray[%d] %d delivered"), aFileNumber, iInputBufferArray[aFileNumber]->iData->Length()));
            iInputBufferArray[aFileNumber]->iData->Des().Zero();
            }
        }
    else
        {
        bytesread = iInputBufferArray[aFileNumber]->iData->Length() - iInputBufferArrayDelivered[aFileNumber];
        aBuf.Append( iInputBufferArray[aFileNumber]->iData->Mid( iInputBufferArrayDelivered[aFileNumber], bytesread ) );
        iInputBufferArrayDelivered[aFileNumber] += bytesread;

        PRINT((_L("CMetaDataFileWriter::ReadBuffer() iInputBufferArray[%d] %d bytes delivered"), aFileNumber, iInputBufferArray[aFileNumber]->iData->Length()));
        iInputBufferArray[aFileNumber]->iData->Des().Zero();
        }
        
	PRINT((_L("e_cmetadatawriter_readbuffer 0")));
    PRINT(_L("CMetaDataFileWriter::ReadBuffer() out"));

    return ( bytesread );
    }

//  End of File  
