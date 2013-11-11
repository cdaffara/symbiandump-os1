/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*    Implementation for the CDosharedDataBase class
*
*/


#include "dosshareddatabase.h"
#include "dosclientserver.h"
#include "dosserver.h"
#include "shareddatafilesystemnotifier.h"
#include "dosclishareddata.h"
#include "dos_debug.h"
#include <e32svr.h>

// CONSTANTS
_LIT( KSDReserveFileName, "C:\\Private\\101f6efa\\reserve.bin" );
_LIT(KSDPath, "C:\\Private\\101f6efa\\");

// Definitions for reserve file system
const TInt KSDLotsOfFreeDiskSpace = 1000000;
const TInt KSDFreeDiskSpaceOverhead = 1024;
const TInt KSDReserveFileLoopMaxCount = 5;

// -----------------------------------------------------------------------------
// CDosSharedDataBase::CDosSharedDataBase* NewL()
// -----------------------------------------------------------------------------
//
EXPORT_C CDosSharedDataBase* CDosSharedDataBase::NewL(CSharedDataFileSystemNotifier* fileSystemNotifier)
    {
    COM_TRACE_1( "[DOSSERVER]\t CDosSharedDataBase* CDosSharedDataBase::NewL(0x%x)", fileSystemNotifier );		
    CDosSharedDataBase* self = NewLC(fileSystemNotifier);
    CleanupStack::Pop();
    return self;
    }

// -----------------------------------------------------------------------------
// CDosSharedDataBase::CDosSharedDataBase* NewLC()
// -----------------------------------------------------------------------------
//
EXPORT_C CDosSharedDataBase* CDosSharedDataBase::NewLC(CSharedDataFileSystemNotifier* fileSystemNotifier)
    {
    COM_TRACE_1( "[DOSSERVER]\t CDosSharedDataBase* CDosSharedDataBase::NewL(0x%x)", fileSystemNotifier );
    CDosSharedDataBase* self = new( ELeave ) CDosSharedDataBase();
    CleanupStack::PushL( self );
    self->ConstructL( fileSystemNotifier );
    return self;
    }

// -----------------------------------------------------------------------------
// CDosSharedDataBase::void ConstructL()
// -----------------------------------------------------------------------------
//
void CDosSharedDataBase::ConstructL(CSharedDataFileSystemNotifier* fileSystemNotifier)
    {
    COM_TRACE_1( "[DOSSERVER]\t CDosSharedDataBase::ConstructL(0x%x)", fileSystemNotifier );	
    iFileSystemNotifier = fileSystemNotifier;
    iFreeDiskSpaceRequest = 0;

	TInt err( iFs.Connect() );
	if ( err < 0 )
	    {
		User::LeaveIfError( err );
	    }
	User::LeaveIfError(iFs.ShareProtected());
    }

// Destructor
CDosSharedDataBase::~CDosSharedDataBase()    
    {
    COM_TRACE_( "[DOSSERVER]\t CDosSharedDataBase::~CDosSharedDataBase()" );	
    if ( iFileSystemNotifier )
        {
        iFileSystemNotifier->Cancel();
        }
    iFs.Close();    
    }
    
//
// ---------------------------------------------------------
// CDosSharedDataBase::ExecuteMessageL
// ---------------------------------------------------------
//
EXPORT_C TInt CDosSharedDataBase::ExecuteMessageL(const RMessage2& aMessage)
{
	COM_TRACE_( "[DOSSERVER]\t CDosSharedDataBase::ExecuteMessageL(...)" );    
	switch (aMessage.Function())
	{       
		case ERequestFreeDiskSpace:
            {
            
            TInt amount( aMessage.Int0() );
            COM_TRACE_2( "CDosSharedDataBase::ExecuteMessageL() case ERequestFreeDiskSpace, amount = %d iFreeDiskSpaceRequest = %d", amount, iFreeDiskSpaceRequest );
            
            if ( amount > iFreeDiskSpaceRequest )
                {
                if ( amount > KSDReserveFileMaxSize )
                    {
                    amount = KSDReserveFileMaxSize;
                    }
                
                COM_TRACE_2( "CDosSharedDataBase::ExecuteMessageL() case ERequestFreeDiskSpace, amount = %d iFreeDiskSpaceRequest = %d", amount, iFreeDiskSpaceRequest );
                
                iFreeDiskSpaceRequest = amount;
                RequestFreeDiskSpace( &iFreeDiskSpaceRequest );
                }  
            
            TInt err(KErrNone);
			return err;
            }

		case ERequestFreeDiskSpaceCancel:
			{
			COM_TRACE_1( "CDosSharedDataBase::ExecuteMessageL() case ERequestFreeDiskSpaceCancel iFreeDiskSpaceRequest = %d", iFreeDiskSpaceRequest );
            
			if ( iFreeDiskSpaceRequest > 0 )
                {
                // iFreeDiskSpaceRequest = 0;
                RequestFreeDiskSpaceCancel(&iFreeDiskSpaceRequest);
                }
                		    
            TInt err(KErrNone);
			return err;
            }

		default:
			COM_TRACE_( "[DOSSERVER]\t Panic: Illegal function!" );
			PanicClient(aMessage,EPanicIllegalFunction);
			return KErrNone;
	}
}

//
// ---------------------------------------------------------
// CDosSharedDataBase::RequestFreeDiskSpace
// ---------------------------------------------------------
//
void CDosSharedDataBase::RequestFreeDiskSpace( TInt* aRequest )
    {
    __ASSERT_DEBUG( aRequest, User::Invariant() );
    
    COM_TRACE_( "[DOSSERVER]\t CDosSharedDataBase::RequestFreeDiskSpace" );
            
    TInt* currentFreeDiskSpaceRequest = iFileSystemNotifier->iServer.CurrentFreeDiskSpaceRequest();
    if ( !currentFreeDiskSpaceRequest || // no request earlier
         aRequest == currentFreeDiskSpaceRequest || // current request updated
         *aRequest > *currentFreeDiskSpaceRequest ) // new higher request
        {
        COM_TRACE_( "CDosSharedDataBase::RequestFreeDiskSpace - no request earlier,..." );
        iFileSystemNotifier->Cancel();        
        iFileSystemNotifier->iServer.SetCurrentFreeDiskSpaceRequest(aRequest);
        SetReserveFileSize( *aRequest );
        }
    COM_TRACE_( "[DOSSERVER]\t CDosSharedDataBase::RequestFreeDiskSpace - return" );
    }
    
// -----------------------------------------------------------------------------
// CDosSharedDataBase::RequestFreeDiskSpaceCancel()
// -----------------------------------------------------------------------------
void CDosSharedDataBase::RequestFreeDiskSpaceCancel( TInt* aRequest )
    {
    __ASSERT_DEBUG( aRequest, User::Invariant() );
    
    COM_TRACE_( "[DOSSERVER]\t CDosSharedDataBase::RequestFreeDiskSpaceCancel" );
    
    TInt* currentFreeDiskSpaceRequest = iFileSystemNotifier->iServer.CurrentFreeDiskSpaceRequest();
    if ( currentFreeDiskSpaceRequest == aRequest )
        {
        
        //*currentFreeDiskSpaceRequest = KSDReserveFileMaxSize;
        *currentFreeDiskSpaceRequest = 0;
        
        iFileSystemNotifier->iServer.SetCurrentFreeDiskSpaceRequest( currentFreeDiskSpaceRequest );
        
        // Adjust the size of the reserve file to the maximum size

        // SetReserveFileSize( KSDReserveFileMaxSize);
        SetReserveFileSize( 0 );
        }
    COM_TRACE_( "[DOSSERVER]\t CDosSharedDataBase::RequestFreeDiskSpaceCancel - return" );
    
    }

// -----------------------------------------------------------------------------
// CDosSharedDataBase::SetReserveFileSize()
// -----------------------------------------------------------------------------

void CDosSharedDataBase::SetReserveFileSize( const TInt aRequiredFreeDiskSpace )
    {    	
    COM_TRACE_( "[DOSSERVER]\t CDosSharedDataBase::SetReserveFileSize" );    
    
    TInt newSize( 0 );
    TInt currentSize( 0 );
    TInt freeSpace( KSDLotsOfFreeDiskSpace );

	TVolumeInfo info;
	TInt ret = iFs.Volume( info, EDriveC );

    if ( ret == KErrNone )
        {
        RFile file;
        ret = file.Open( iFs, KSDReserveFileName, EFileShareAny | EFileWrite );

        switch ( ret )
            {
            case KErrPathNotFound:
                iFs.MkDirAll( KSDPath );    // no break here 
            case KErrNotFound:
                ret = file.Create( iFs, KSDReserveFileName, 
                                   EFileShareAny | EFileWrite );      
            default: {}
            }

        if ( ret == KErrNone )
            {
            ret = file.Size( currentSize );

            COM_TRACE_( "SharedData: SetReserveFileSize() start" );
            COM_TRACE_1( "SharedData: Free disk space: %d", TInt(info.iFree) );    
            COM_TRACE_1( "SharedData: Reserve file size: %d", currentSize );                

            if ( ret == KErrNone )
                {
                if ( info.iFree < freeSpace )
                    {
                    freeSpace = I64INT(info.iFree);
                    }

                // to make sure infinite loop can never happen
                TInt loopCounter( 0 );

                // *** START of reserve file size altering section ***

                do  {
                    loopCounter++;

                    newSize = freeSpace + currentSize - aRequiredFreeDiskSpace;

                    if ( aRequiredFreeDiskSpace > 0 )
                        {
                        newSize -= KSDFreeDiskSpaceOverhead;
                        }

                    if ( newSize < 0 )
                        {
                        newSize = 0;
                        }

                    else if ( newSize > KSDReserveFileMaxSize )
                        {
                        newSize = KSDReserveFileMaxSize;
                        }

                    // set the file size to 'newSize' here

                    TInt position( 0 );
                    ret = file.Seek( ESeekEnd, position );

                    if ( ret == KErrNone )
                        {
                        if ( position > newSize )
                            {
                            ret = file.SetSize( newSize );
                            
                            COM_TRACE_1( "SharedData: SetSize returned: %d", ret );

                            file.Flush();
                            }

                        else if ( position < newSize )
                            {
                            TInt toBeWritten( newSize - position );

                            HBufC8* buffer = HBufC8::New( toBeWritten );

                            // Write the new data in cycles,
                            // halving the total amount to be written in
                            // every cycle until we get below 1024 bytes.
                            // This leads to max 6 cycles and ensures that
                            // as much as possible gets reserved back.

                            if ( buffer )
                                {
                                while ( ret != KErrDiskFull &&
                                        toBeWritten > 1024 )
                                    {
                                    buffer->Des().SetLength( toBeWritten >> 1 );

                                    ret = file.Write( *buffer );

                                    if ( ret == KErrNone )
                                        {
                                        ret = file.Flush();
                                        }

                                    toBeWritten -= ( toBeWritten >> 1 );
                                    }
                            
                                buffer->Des().SetLength( toBeWritten );
                                ret = file.Write( *buffer );

                                if ( ret == KErrNone )
                                    {
                                    ret = file.Flush();
                                    }

                                delete buffer;
                                }
                            }
                        }

                    // update reserve file size info
                    file.Size( currentSize );

                    // update free disk space info
                    ret = iFs.Volume( info, EDriveC );

                    if ( ret == KErrNone )
                        {
                        freeSpace = KSDLotsOfFreeDiskSpace;

                        if ( info.iFree < freeSpace )
                            {
                            freeSpace = I64INT(info.iFree);
                            }
                        }

                    COM_TRACE_( "SharedData: SetReserveFileSize() loop" );
                    COM_TRACE_1( "SharedData: Free disk space: %d", (TInt) info.iFree );
                    COM_TRACE_1( "SharedData: Expected reserve file size: %d", newSize );
                    COM_TRACE_1( "SharedData: Reserve file size: %d", currentSize );                    
                    }

                while ( loopCounter < KSDReserveFileLoopMaxCount &&
                        aRequiredFreeDiskSpace > 0 &&
                        currentSize > 0 &&
                        freeSpace < aRequiredFreeDiskSpace );

                // *** END of reserve file size altering section ***

                // If there are no more free disk space requests but we didn't
                // manage to increase the reserve file size back to max, get
                // notification from file server when there is more disk space.

                if ( aRequiredFreeDiskSpace == 0 &&
                     currentSize < KSDReserveFileMaxSize )
                    {
                    iFileSystemNotifier->Start( freeSpace + 2500, iFs, this );
                    }
                }
                
            file.Close();
            }
        }

    COM_TRACE_1( "SharedData: Free disk space: %d", (TInt) info.iFree );
    COM_TRACE_1( "SharedData: Expected reserve file size: %d", newSize );
    COM_TRACE_1( "SharedData: Reserve file size:  %d", currentSize );
    
    COM_TRACE_( "[DOSSERVER]\t CDosSharedDataBase::SetReserveFileSize - return" );    
    }
