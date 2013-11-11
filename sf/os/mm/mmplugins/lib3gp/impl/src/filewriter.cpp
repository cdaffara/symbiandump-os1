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
#include "filewriter.h"

// MACROS
// Debug print macro
#ifdef _DEBUG
#include <e32svr.h>
//#define PRINT(x) RDebug::Print x
#define PRINT(x)
#else
#define PRINT(x)
#endif

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CFileWriter::CFileWriter
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CFileWriter::CFileWriter( TInt aInitSetSize, TInt aOutputBufferSizeSmall, TInt aOutputBufferSizeLarge ):
	CActive( EPriorityHigh ),
	iSetSize( aInitSetSize ),
	iOutputBufferSizeSmall( aOutputBufferSizeSmall ),
	iOutputBufferSizeLarge( aOutputBufferSizeLarge )
    {
    }

// -----------------------------------------------------------------------------
// CFileWriter::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CFileWriter::ConstructL( RFile64& aFile )
    {
    PRINT((_L("CFileWriter::ConstructL() in")));         
    iFlush = EFalse;
    iError = KErrNone;

    iOutputFile = &aFile;
    iWritingStarted = EFalse;
    iOutputBufferSize = KFileWriterBufferSizeSmall;
    iMaxOutputBufHardLimit = KFileWriterHardBufLimit;
    iMaxOutputBufSoftLimit = KFileWriterSoftBufLimit;

    iMemReadyForWriting = EFalse;
    iInputBuf = NULL;
    iEmptyBufferQueue.Reset();
    iFullBufferQueue.Reset();

    AllocateBuffersL();
    CActiveScheduler::Add(this);
    PRINT((_L("CFileWriter::ConstructL() out")));         
    }

// -----------------------------------------------------------------------------
// CFileWriter::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CFileWriter* CFileWriter::NewL( RFile64& aFile, TInt aInitSetSize, TInt aOutputBufferSizeSmall, TInt aOutputBufferSizeLarge )
    {
    CFileWriter* self = new(ELeave) CFileWriter( aInitSetSize, aOutputBufferSizeSmall, aOutputBufferSizeLarge );
    CleanupStack::PushL(self);
    self->ConstructL( aFile );
    CleanupStack::Pop(self);
    return self;
    }


// Destructor
CFileWriter::~CFileWriter()
    {
    PRINT((_L("CFileWriter::~CFileWriter() in")));         
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

    if ( iInputBuf )
        {
        delete iInputBuf;
        }

    iEmptyBufferQueue.ResetAndDestroy();
    iFullBufferQueue.ResetAndDestroy();
    PRINT((_L("CFileWriter::~CFileWriter() out")));         
    }

// -----------------------------------------------------------------------------
// CFileWriter::UpdateOutputFileSize()
// Updates output file size and reserves extra space for following writing 
// if iSetSize is set.  
// Takes into account if the position in the file was changed.
// -----------------------------------------------------------------------------
//
void CFileWriter::UpdateOutputFileSize()
    {
    TInt64 pos = 0;
    PRINT((_L("e_cfilewriter_write_updateoutputfilesize_seek 1")));
    iOutputFile->Seek(ESeekCurrent, pos);
    PRINT((_L("e_cfilewriter_write_updateoutputfilesize_seek 0")));
    
    PRINT((_L("CFileWriter::UpdateOutputFileSize() pos: %Ld"), pos));
    PRINT((_L("CFileWriter::UpdateOutputFileSize() iOutputFileSize: %Ld"), iOutputFileSize));
    PRINT((_L("CFileWriter::UpdateOutputFileSize() iSetSize: %Ld"), iSetSize));
    
    if (pos > iOutputFileSize) 
        {
        iOutputFileSize = pos;
        }
    
    while (iOutputFileSize >= iSetSize) 
        {
        iSetSize += static_cast<TInt64>(iOutputBufferSize) * (static_cast<TInt64>(iMaxOutputBufHardLimit) >> 1); 
        PRINT((_L("e_cfilewriter_updateoutputfilesize_setsize 1")));                     
        iOutputFile->SetSize( iSetSize );
        PRINT((_L("e_cfilewriter_updateoutputfilesize_setsize 0")));                     
        }
    }

// -----------------------------------------------------------------------------
// CFileWriter::Write( const TDesC8& aBuf )
// Writes incoming buffer data to internal buffers for writing to disk.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CFileWriter::Write( const TDesC8& aBuf )
    {
    PRINT(_L("CFileWriter::Write() in"));
    PRINT((_L("e_cfilewriter_write 1")));

    iWritingStarted = ETrue;
    
    if ( !iMemReadyForWriting )
    	{
    	return KErrNoMemory;
    	}

    PRINT((_L("e_cfilewriter_write_adddatatobuffer 1")));	
    TInt error = AddDataToBuffer( aBuf );
    PRINT((_L("e_cfilewriter_write_adddatatobuffer 0")));	    
    
    if ( error != KErrNone )
        {
        PRINT((_L("CFileWriter::Write() buffer write error: %d"), error));         
        return error;
        }

    PRINT((_L("CFileWriter::Write() Write Buffer, Status: Full:%d Empty:%d "), 
                    iFullBufferQueue.Count(), iEmptyBufferQueue.Count() ));

    if ( iAsyncWritingOngoing )
        {
        if ( iFullBufferQueue.Count() >= iMaxOutputBufHardLimit )
            {
            PRINT((_L("CFileWriter::Write() Waiting async write to complete")));
			PRINT((_L("e_cfilewriter_write_wait_async 1")));            
            User::WaitForRequest( iStatus );
			PRINT((_L("e_cfilewriter_write_wait_async 0")));            
            PRINT((_L("CFileWriter::Write() Async write done")));
            TRAP(error, RunL());
            if (error != KErrNone)
            	{
            	PRINT((_L("CFileWriter::Write() call runL leave, error: %d"), error));         
                return error;
            	}
            }
        }
    else
        {
        if ( iFullBufferQueue.Count() )
            {
            PRINT(_L("CFileWriter::Write() writing async"));                     
            PRINT((_L("e_cfilewriter_write_startwrite 1")));                     
            iOutputFile->Write( *iFullBufferQueue[0], iStatus );
            PRINT((_L("e_cfilewriter_write_startwrite 0")));
            iAsyncWritingOngoing = ETrue;
            if ( !IsActive() )
                {
                SetActive();
                }
            }
        }

    PRINT(_L("CFileWriter::Write() out"));
    PRINT((_L("e_cfilewriter_write 0")));    
    return error;
    }

// -----------------------------------------------------------------------------
// CFileWriter::Flush( const TDesC8& aBuf )
// Flush internal buffers to disk.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CFileWriter::Flush( const TDesC8& aBuf )
    {
    PRINT(_L("CFileWriter::Flush() in"));
    PRINT((_L("e_cfilewriter_flush 1")));
 
 	if ( !iMemReadyForWriting )
 		{
 		return KErrNoMemory;
 		}
 	
    iWritingStarted = ETrue;

    PRINT((_L("e_cfilewriter_flush_adddatatobuf 1")));
    TInt error = AddDataToBuffer( aBuf );
    if ( error != KErrNone )
        {
        return error;
        }
	PRINT((_L("e_cfilewriter_flush_adddatatobuf 0")));        

    PRINT((_L("CFileWriter::Flush() FullCount: %d "), iFullBufferQueue.Count()));
    iFlush = ETrue;

    if ( iAsyncWritingOngoing )
        {
        PRINT((_L("CFileWriter::Flush() Waiting async write to complete")));
        PRINT((_L("e_cfilewriter_flush_waitasynctostop 1")));
        User::WaitForRequest( iStatus );
        PRINT((_L("e_cfilewriter_flush_waitasynctostop 0")));
        PRINT((_L("CFileWriter::Flush() Async write done, flushing")));
        TRAP(error, RunL());
        if (error != KErrNone)
        	{
        	PRINT((_L("CFileWriter::Flush() call runL leave, error: %d"), error));         
            return error;
        	}
        }

    while ( iFullBufferQueue.Count() )
        {
        PRINT((_L("e_cfilewriter_flush_writequeue_sync 1")));
        error = iOutputFile->Write( *iFullBufferQueue[0] );
        PRINT((_L("e_cfilewriter_flush_writequeue_sync 0")));
        PRINT((_L("e_cfilewriter_flush_remove_buf 1")));
        if ( error == KErrNone )
            {
            UpdateOutputFileSize();
            iFullBufferQueue[0]->Des().Zero();
            if ( iEmptyBufferQueue.Append( iFullBufferQueue[0] ) )
                {
                PRINT(_L("CFileWriter::Flush() Append failed"));
                delete ( iFullBufferQueue[0] ); 
                }
            iFullBufferQueue.Remove( 0 );
            }
        else
            {
            PRINT((_L("CFileWriter::Flush() fullBufQueue write failed, error: %d"), error));
            iFlush = EFalse;
            return error;
            }
        PRINT((_L("e_cfilewriter_flush_remove_buf 0")));    
        }

    if ( iInputBuf->Length() )
        {
        PRINT((_L("e_cfilewriter_flush_writeinput_sync 1")));
        error = iOutputFile->Write( *iInputBuf );
        PRINT((_L("e_cfilewriter_flush_writeinput_sync 0")));
        if ( error == KErrNone )
            {
            UpdateOutputFileSize();
            iInputBuf->Des().Zero();
            }
        else
            {
            PRINT((_L("CFileWriter::Flush() inputbuf write failed, error: %d"), error));
            iFlush = EFalse;
            return error;
            }
        }
        
    iFlush = EFalse;
    PRINT((_L("CFileWriter::Flush() FullCount: %d <= Should be 0"), iFullBufferQueue.Count()));
    PRINT(_L("CFileWriter::Flush() out"));
    PRINT((_L("e_cfilewriter_flush 0")));    
    return KErrNone;
    }


// -----------------------------------------------------------------------------
// CFileWriter::SetOutputBufferSize( TOutputBufferSize aBufferSize )
// Set output buffer sizes.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CFileWriter::SetOutputBufferSize( TOutputBufferSize aBufferSize, MP4Handle aHandle )
    {
    MP4HandleImp handle = (MP4HandleImp)aHandle;    
    TInt size = 0;

    if ( iWritingStarted )
        {
        return KErrNotReady;
        }

    if ( aBufferSize == EBufferSizeSmall ) 
        {
        size = iOutputBufferSizeSmall;
        }
    else if ( aBufferSize == EBufferSizeLarge ) 
        {
        size = iOutputBufferSizeLarge;
        }
    else if ( aBufferSize == EBufferSizeCustom )
        {
        size = handle->mediaWriteBufferSize;
        }
    else
        {
        return KErrArgument;    
        }

    if ( size == iOutputBufferSize )
        {
        return KErrNone;
        }
    else
        {
        iOutputBufferSize = size;
        }

    iMemReadyForWriting = EFalse;
    delete iInputBuf;
    iInputBuf = NULL;
    iEmptyBufferQueue.ResetAndDestroy();
    iFullBufferQueue.ResetAndDestroy();

    TRAPD(err, AllocateBuffersL() );
    return err;
    }

// -----------------------------------------------------------------------------
// CFileWriter::SetOutputBufferCount( MP4Handle aHandle )
// Set output buffer count.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CFileWriter::SetOutputBufferCount( MP4Handle aHandle )
    {
    MP4HandleImp handle = (MP4HandleImp)aHandle;    

    if (  handle->writeBufferMaxCount >= 6 )     
        {
        iMaxOutputBufHardLimit = handle->writeBufferMaxCount;
        iMaxOutputBufSoftLimit = KFileWriterMinBufferCount + ((iMaxOutputBufHardLimit-KFileWriterMinBufferCount)/2);
        }
    }


// -----------------------------------------------------------------------------
// CFileWriter::AddDataToBuffer( const TDesC8& aBuf )
// Writes incoming data to internal buffers and buffer queues..
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CFileWriter::AddDataToBuffer( const TDesC8& aBuf )
    {
    PRINT(_L("CFileWriter::AddDataToBuffer() in"));

    TInt byteswritten = 0;
    TInt numbytes = 0;
    TInt available = 0; // Available bytes in write buffer
    TInt error = KErrNone;

    if ( iError != KErrNone )
        {
        PRINT((_L("CFileWriter::AddDataToBuffer() out, RunL iError: %d"), iError));
        return iError;
        }

    if ( iInputBuf == NULL )
    	{
    	return KErrNoMemory;
    	}

    while (byteswritten < aBuf.Length() )
        {
        available = iOutputBufferSize - iInputBuf->Length();

        if (available > 0)
            {
            numbytes = aBuf.Length() - byteswritten;
            if (numbytes > available)
                {
                numbytes = available;
                }
            iInputBuf->Des().Append( aBuf.Mid( byteswritten, numbytes ) );
            byteswritten += numbytes;
            }
        else // Buffer is full, write it to disk
            {
            // input is full, move full inputbuf to full buf queue
            if ( iFullBufferQueue.Append( iInputBuf ) != KErrNone )
                {
                PRINT(_L("CFileWriter::AddDataToBuffer() Append failed"));
                delete iInputBuf;
                iInputBuf = NULL;
                }
            if ( iEmptyBufferQueue.Count() == 0 )
                {
                // no empty buffers in queue, allocating new one
                TRAP(error, iInputBuf = HBufC8::NewL( iOutputBufferSize ));
                if ( error != KErrNone )
                    {
                    PRINT((_L("CFileWriter::AddDataToBuffer(), memory alloc failed: %d"), error));
                    iInputBuf = NULL;
                    iError = error;
                    break;
                    }
                }
            else
                {
                iInputBuf = iEmptyBufferQueue[ 0 ];
                iEmptyBufferQueue.Remove( 0 );
                }
            }
        }

    PRINT((_L("CFileWriter::AddDataToBuffer() out, error: %d"), error));
    return error;
    }

// -----------------------------------------------------------------------------
// CFileWriter::AllocateBuffersL()
// Allocates input and output buffers.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CFileWriter::AllocateBuffersL()
    {
    PRINT((_L("CFileWriter::AllocateBuffersL() in, outbufsize: %d"), iOutputBufferSize));
    HBufC8* buf = NULL;
    TInt err = 0;
    iMemReadyForWriting = EFalse;

    iInputBuf = HBufC8::NewL( iOutputBufferSize );
    for( TInt i=0; i<KFileWriterMinBufferCount; i++ )
        {
        buf = HBufC8::NewL( iOutputBufferSize );
        err = iEmptyBufferQueue.Append( buf );
        if ( err )
            {
            delete ( buf );
            User::Leave( err );
            }
        }
    iMemReadyForWriting = ETrue;
    PRINT((_L("CFileWriter::AllocateBuffersL() out")));
    }

// -----------------------------------------------------------------------------
// CFileWriter::DoCancel()
// From CActive Cancels async request.
// -----------------------------------------------------------------------------
//
void CFileWriter::DoCancel()
    {
    }

// -----------------------------------------------------------------------------
// CFileWriter::RunL()
// From CActive Called when async request completes.
// -----------------------------------------------------------------------------
//
void CFileWriter::RunL()
    {
    PRINT(_L("CFileWriter::RunL() in"));
	PRINT((_L("e_cfilewriter_runl 1")));    
    iAsyncWritingOngoing = EFalse;

    if ( iStatus == KErrNone )
        {
        UpdateOutputFileSize();                    
        iFullBufferQueue[0]->Des().Zero();
        iError = iEmptyBufferQueue.Append( iFullBufferQueue[0] );
        if ( iError )
            {
            PRINT((_L("CFileWriter::RunL() Append failed 1 %d"), iError ));
            delete ( iFullBufferQueue[0] );
            iFullBufferQueue.Remove( 0 );
            return;
            }
        iFullBufferQueue.Remove( 0 );
        }
    else
        {
        PRINT((_L("CFileWriter::RunL() Write error in previous async: %d "), iStatus.Int() ));
        iError = iStatus.Int();
        return;
        }

    PRINT((_L("CFileWriter::RunL() Buffer written, Status: Full:%d Empty:%d Filesize:%Ld"), iFullBufferQueue.Count(), iEmptyBufferQueue.Count(), iOutputFileSize ));
    
    if ( iFlush )
        {
        PRINT(_L("CFileWriter::RunL() out, flushing"));
        PRINT((_L("e_cfilewriter_runl 0")));  
        return;
        }

    if ( iFullBufferQueue.Count() >= iMaxOutputBufHardLimit )
        {
        while ( iFullBufferQueue.Count() > iMaxOutputBufSoftLimit )
            {
            PRINT((_L("e_cfilewriter_runl_write 1")));                     
            iError = iOutputFile->Write( *iFullBufferQueue[0]);
            PRINT((_L("e_cfilewriter_runl_write 0")));                     
            if ( iError == KErrNone )
                {
                UpdateOutputFileSize();
                iFullBufferQueue[0]->Des().Zero();
                iError = iEmptyBufferQueue.Append( iFullBufferQueue[0] );
                if ( iError )
                    {
                    PRINT((_L("CFileWriter::RunL() Append failed 2 %d"), iError));
                    delete ( iFullBufferQueue[0] );
                    iFullBufferQueue.Remove( 0 );
                    return;
                    }
                iFullBufferQueue.Remove( 0 );
    			PRINT((_L("CFileWriter::RunL() Hardlimit : Buffer sync written, Status: Full:%d Empty:%d Filesize:%Ld"), iFullBufferQueue.Count(), iEmptyBufferQueue.Count(), iOutputFileSize ));
                }   
            else
                {
                PRINT((_L("CFileWriter::RunL() Write error: %d "), iError));
                return;
                }   
            }
        }

    if ( iFullBufferQueue.Count() >= iMaxOutputBufSoftLimit )
        {
        PRINT((_L("e_cfilewriter_runl_outfile_write 1")));                     
        iError = iOutputFile->Write( *iFullBufferQueue[0]);
        PRINT((_L("e_cfilewriter_runl_outfile_write 0")));                     
        if ( iError == KErrNone )
            {
            UpdateOutputFileSize();
            iFullBufferQueue[0]->Des().Zero();
            iError = iEmptyBufferQueue.Append( iFullBufferQueue[0] );
            if ( iError )
                {
                PRINT((_L("CFileWriter::RunL() Append failed 3 %d"), iError));
                delete ( iFullBufferQueue[0] );
                iFullBufferQueue.Remove( 0 );
                return;
                }
            iFullBufferQueue.Remove( 0 );
    		PRINT((_L("CFileWriter::RunL() Softlimit : Buffer sync written, Status: Full:%d Empty:%d Filesize:%Ld"), iFullBufferQueue.Count(), iEmptyBufferQueue.Count(), iOutputFileSize ));
            }   
        else
            {
            PRINT((_L("CFileWriter::RunL() Write error: %d "), iError));
            return;
            }
        }

    if ( iFullBufferQueue.Count() )
        {
        PRINT((_L("e_cfilewriter_runl_outfile2_write 1")));                     
        iOutputFile->Write( *iFullBufferQueue[0], iStatus );
        PRINT((_L("e_cfilewriter_runl_outfile2_write 0")));                     
        iAsyncWritingOngoing = ETrue;
        if ( !IsActive() )
            {
            SetActive();
            }
        }

	PRINT((_L("e_cfilewriter_runl 0")));  
    PRINT((_L("CFileWriter::RunL() out, iError=%d"), iError));
    }

//  End of File  
