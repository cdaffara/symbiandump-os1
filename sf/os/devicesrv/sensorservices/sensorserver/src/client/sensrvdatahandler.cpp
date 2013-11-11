/*
* Copyright (c) 2006-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  CSensrvDataHandler implementation
*
*/



#include "sensrvdatahandler.h"
#include "sensrvtrace.h"
#include "sensrvdatahandlerobserver.h"
#include "sensrvdefines.h"



// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// C++ constructor
// ---------------------------------------------------------------------------
//

CSensrvDataHandler::CSensrvDataHandler( RSensrvClient& aSensrvClient, 
                                        const TSensrvChannelInfo& aChannelInfo )
    : CActive( EPriorityStandard ), 
      iSensrvClient( aSensrvClient ), 
      iChannelInfo( aChannelInfo )
    {
    
    CActiveScheduler::Add( this );
    
    }


// ---------------------------------------------------------------------------
// 2nd phase of construction
// ---------------------------------------------------------------------------
//
void CSensrvDataHandler::ConstructL()
    {
    
    }


// ---------------------------------------------------------------------------
// 2-phase constructor
// ---------------------------------------------------------------------------
//
CSensrvDataHandler* CSensrvDataHandler::NewL( RSensrvClient& aSensrvClient, 
                                        const TSensrvChannelInfo& aChannelInfo )
    {
    CSensrvDataHandler* self = CSensrvDataHandler::NewLC( aSensrvClient, aChannelInfo );
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// 2-phase constructor
// ---------------------------------------------------------------------------
//
CSensrvDataHandler* CSensrvDataHandler::NewLC( RSensrvClient& aSensrvClient, 
                                        const TSensrvChannelInfo& aChannelInfo )
    {
    CSensrvDataHandler* self = new( ELeave ) CSensrvDataHandler( aSensrvClient, aChannelInfo );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CSensrvDataHandler::~CSensrvDataHandler()
    {
    Cancel();
    delete iWriteBuffer;
    delete iWriteBufferPtr;
    delete iReadBuffer;
    }




// ---------------------------------------------------------------------------
// From class CActive
// ---------------------------------------------------------------------------
//
void CSensrvDataHandler::DoCancel()
    {
    COMPONENT_TRACE( ( _L( "Sensrv Client - CSensrvDataHandler::DoCancel()" ) ) );
    
    iSensrvClient.SendReceiveSync( ESensrvSrvReqStopListening, 
                                        TIpcArgs( iChannelInfo.iChannelId ) );
                                        
    COMPONENT_TRACE( ( _L( "Sensrv Client - CSensrvDataHandler::DoCancel - return void" ) ) );
    }
    
// ---------------------------------------------------------------------------
// From class CActive
// ---------------------------------------------------------------------------
//
void CSensrvDataHandler::RunL()
    {
    COMPONENT_TRACE( ( _L( "Sensrv Client - CSensrvDataHandler::RunL - iStatus %d" ), iStatus.Int() ) );

	Deque();
	CActiveScheduler::Add(this);
    if( iStatus.Int() == KErrNone )
        {
        if( iErrorCount > 0 )
            {
            iErrorCount = 0;
            }
        HBufC8* temp = iReadBuffer;
        iReadBuffer = iWriteBuffer;
        iWriteBuffer = temp;    
        
        TSensrvAsyncChannelDataCountsRetval dataCounts = iDataCountsPckgBuf();
        COMPONENT_TRACE( ( _L( "Sensrv Client - CSensrvDataHandler::RunL - DataItemCount = %d, DataLostCount = %d "), dataCounts.iDataItemCount, dataCounts.iDataLostCount ) );
        TInt lostCount( dataCounts.iDataLostCount );
        lostCount += ( iReadBufferCount - iGetDataCount );
        
        // Reset buffer counts
        iReadBufferCount = dataCounts.iDataItemCount;
        iGetDataCount = 0; 
        
        CreateAndSendRequest();
        
        // Notify to listener
        iDataObserver->DataReceived( iReadBufferCount, lostCount );
        
        }
    else
        {
        if( iErrorCount == 0 )
            {
            iErrorCount++;
            CreateAndSendRequest();
            
            // Send minor error
            iDataObserver->DataError( ESensrvErrorSeverityMinor );
            
            }
        else if( iErrorCount == 1 )
            {
            iErrorCount++;
            // Reopen channel
            
            TInt maxBufferCount(0);
            iDataObserver->ReopenChannelForDataListeningL( maxBufferCount );
            
            TSensrvStartListeningParametersPckgBuf startListeningPckg( iListeningParameters );

            COMPONENT_TRACE( ( _L( "Sensrv Client - CSensrvDataHandler::RunL - Restart listening - iDesiredBufferingCount = %d, iMaximumBufferingCount = %d, iBufferingPeriod = %d "), 
                iListeningParameters.iDesiredBufferingCount, 
                iListeningParameters.iMaximumBufferingCount, 
                iListeningParameters.iBufferingPeriod ) );
            
            
            TInt err = iSensrvClient.SendReceiveSync( ESensrvSrvReqStartListening, 
                            TIpcArgs( iChannelInfo.iChannelId, &startListeningPckg ) );
            if( err )
                {
                // send fatal error
                iDataObserver->DataError( ESensrvErrorSeverityFatal );                    
                }
            else
                {
                TInt newMaxSize( maxBufferCount * iChannelInfo.iDataItemSize );
                if( iWriteBuffer->Length() > newMaxSize )
                    {
                    iWriteBuffer->ReAllocL( newMaxSize );
                    iReadBuffer->ReAllocL( newMaxSize );
                    delete iWriteBufferPtr;
                    iWriteBufferPtr = NULL;
                    iWriteBufferPtr = new (ELeave) TPtr8( const_cast<TUint8*>( iWriteBuffer->Des().Ptr() ), newMaxSize );
                    }
                // Start listening
                CreateAndSendRequest();
                }
            }
        else
            {
            // send fatal error
            iDataObserver->DataError( ESensrvErrorSeverityFatal );  
            }
        
        }
        
    }
    
// ---------------------------------------------------------------------------
// CSensrvDataHandler::RunError
// ---------------------------------------------------------------------------
//
TInt CSensrvDataHandler::RunError( TInt /*aError*/ )
    {
    COMPONENT_TRACE( ( _L( "Sensrv Client - CSensrvDataHandler::RunError" ) ) );
    // send fatal error
    iDataObserver->DataError( ESensrvErrorSeverityFatal );
    return KErrNone;
    }


// ---------------------------------------------------------------------------
// CSensrvDataHandler::StartListening
// ---------------------------------------------------------------------------
//
void CSensrvDataHandler::StartListeningL( MSensrvDataHandlerObserver* aDataObserver, 
                                TSensrvStartListeningParameters aListeningParameters,
                                TInt aMaxBufferCount )
    {
    COMPONENT_TRACE( ( _L( "Sensrv Client - CSensrvDataHandler::StartListening - Start" ) ) );
    
    if( IsActive() )
        {
        COMPONENT_TRACE( ( _L( "Sensrv Client - CSensrvDataHandler::StartListening - Return" ) ) );
        return;
        }
    
    iDataObserver = aDataObserver;
    iListeningParameters = aListeningParameters;
        
    iErrorCount = 0;
    
    TInt size = iListeningParameters.iMaximumBufferingCount * iChannelInfo.iDataItemSize;
    
    // Check size
    if( size <= 0 || iListeningParameters.iMaximumBufferingCount > ( KMaxTInt / iChannelInfo.iDataItemSize )
        || iListeningParameters.iMaximumBufferingCount < iListeningParameters.iDesiredBufferingCount 
        || iListeningParameters.iMaximumBufferingCount > aMaxBufferCount )
        {
        size = aMaxBufferCount * iChannelInfo.iDataItemSize;
        }
    COMPONENT_TRACE( ( _L( "Sensrv Client - CSensrvDataHandler::StartListening - bufferSize: %d" ), size ) );
    delete iWriteBuffer;
    iWriteBuffer = NULL;
    iWriteBuffer = HBufC8::NewL( size );
    delete iReadBuffer;
    iReadBuffer = NULL;
    iReadBuffer = HBufC8::NewL( size );

    delete iWriteBufferPtr;
    iWriteBufferPtr = NULL;
    iWriteBufferPtr = new (ELeave) TPtr8( const_cast<TUint8*>( iWriteBuffer->Des().Ptr() ), size );
    

    CreateAndSendRequest();
            
    COMPONENT_TRACE( ( _L( "Sensrv Client - CSensrvDataHandler::StartListening - Return" ) ) );
    }

// ---------------------------------------------------------------------------
// CSensrvDataHandler::GetData
// ---------------------------------------------------------------------------
//
TInt CSensrvDataHandler::GetData( TDes8& aDataPtr )
    {
    COMPONENT_TRACE( ( _L( "Sensrv Client - CSensrvDataHandler::GetData - Start" ) ) );
    TInt err( KErrNone );
    
    if( aDataPtr.Length() < iChannelInfo.iDataItemSize )
        {
        // aDataPtr is too small
        err = KErrOverflow;
        }
    else if( iGetDataCount >= iReadBufferCount || !iReadBuffer )
        {
        // data not found.
        err = KErrNotFound;
        }
    else
        {
        // set data
        TUint8* ptr = const_cast<TUint8*>( iReadBuffer->Des().Ptr() );
        ptr += ( iGetDataCount * iChannelInfo.iDataItemSize ); 
        
        aDataPtr.Copy( ptr, iChannelInfo.iDataItemSize );
        ++iGetDataCount;
        }
    
    COMPONENT_TRACE( ( _L( "Sensrv Client - CSensrvDataHandler::GetData - Return %d" ), err ) );
    return err;
    }
    
// ---------------------------------------------------------------------------
// CSensrvDataHandler::CreateAndSendRequestL
// ---------------------------------------------------------------------------
//
void CSensrvDataHandler::CreateAndSendRequest()
    {
    COMPONENT_TRACE( ( _L( "Sensrv Client - CSensrvDataHandler::CreateAndSendRequestL - Start" ) ) );
    
    iWriteBufferPtr->Set( iWriteBuffer->Des() );
    
    iSensrvClient.SendReceiveAsync( ESensrvSrvReqAsyncChannelData, 
            TIpcArgs( iChannelInfo.iChannelId, iWriteBufferPtr, &iDataCountsPckgBuf ), iStatus );
    
    SetActive();
    
    COMPONENT_TRACE( ( _L( "Sensrv Client - CSensrvDataHandler::CreateAndSendRequestL - Return" ) ) );
    }

