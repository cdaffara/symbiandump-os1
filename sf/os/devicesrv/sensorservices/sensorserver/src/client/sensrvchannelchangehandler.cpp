/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  CSensrvChannelChangeHandler implementation
*
*/


#include "sensrvchannelchangehandler.h"
#include "sensrvchannelchangehandlerobserver.h"
#include "sensrvtrace.h"


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// C++ constructor
// ---------------------------------------------------------------------------
//
CSensrvChannelChangeHandler::CSensrvChannelChangeHandler( RSensrvClient& aSensrvClient )
    : CActive( EPriorityStandard ), 
      iSensrvClient( aSensrvClient )
    {    
    CActiveScheduler::Add( this );    
    }


// ---------------------------------------------------------------------------
// 2-phase constructor
// ---------------------------------------------------------------------------
//
CSensrvChannelChangeHandler* CSensrvChannelChangeHandler::NewL( RSensrvClient& aSensrvClient )
    {
    CSensrvChannelChangeHandler* self = CSensrvChannelChangeHandler::NewLC( aSensrvClient );
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// 2-phase constructor
// ---------------------------------------------------------------------------
//
CSensrvChannelChangeHandler* CSensrvChannelChangeHandler::NewLC( RSensrvClient& aSensrvClient )
    {
    CSensrvChannelChangeHandler* self = new( ELeave ) CSensrvChannelChangeHandler( aSensrvClient );
    CleanupStack::PushL( self );
    return self;
    }


// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CSensrvChannelChangeHandler::~CSensrvChannelChangeHandler()
    {
    if( IsActive() )
        {
        Cancel();
        }
    }

// ---------------------------------------------------------------------------
// From class CActive
// ---------------------------------------------------------------------------
//
void CSensrvChannelChangeHandler::DoCancel()
    {
    COMPONENT_TRACE( ( _L( "Sensrv Client - CSensrvChannelChangeHandler::DoCancel()" ) ) );
    
    // Stop Listening
    TInt ignore = iSensrvClient.SendReceiveSync( ESensrvSrvReqStopChannelChangeListening, 
                                                 TIpcArgs() );
                                                 
    COMPONENT_TRACE( ( _L( "Sensrv Client - CSensrvChannelChangeHandler::DoCancel - return void" ) ) );
    }
    
// ---------------------------------------------------------------------------
// From class CActive
// ---------------------------------------------------------------------------
//
void CSensrvChannelChangeHandler::RunL()
    {
    COMPONENT_TRACE( ( _L( "Sensrv Client - CSensrvChannelChangeHandler::RunL(iStatus: %d" ), iStatus.Int() ) );
    
    
	Deque();
	CActiveScheduler::Add(this);
    if( iStatus.Int() == KErrNone )
        {        
        if( iErrorCount > 0 )
            {
            iErrorCount = 0;
            }

        // Note: Callback is done intentionally before resending async request,
        // so that we don't need duplicate buffer like in data reading.
        // It shouldn't have noticeable effect on performance in real use cases,
        iObserver->ChannelChangeDetected( iChannelInfoPckgBuf(), iChangeTypePckgBuf() ); 
        CreateAndSendRequest();  
        }
    else
        {
        if( iErrorCount == 0 )
            {
            iErrorCount++;
            CreateAndSendRequest();
            
            // Send minor error
            iObserver->ChannelDetectionError( ESensrvErrorSeverityMinor );
            }
        else
            {
            // send fatal error
            iObserver->ChannelDetectionError( ESensrvErrorSeverityFatal );  
            }
        }
        
    COMPONENT_TRACE( ( _L( "Sensrv Client - CSensrvChannelChangeHandler::RunL - return" ) ) );
    }
    
// ---------------------------------------------------------------------------
// CSensrvChannelChangeHandler::RunError
// ---------------------------------------------------------------------------
//
TInt CSensrvChannelChangeHandler::RunError( TInt /*aError*/ )
    {
    COMPONENT_TRACE( ( _L( "Sensrv Client - CSensrvChannelChangeHandler::RunError" ) ) );
    // This should never get hit, since there is nothing which can leave in RunL.
    return KErrNone;
    }

// ---------------------------------------------------------------------------
// CSensrvChannelChangeHandler::StartListening
// ---------------------------------------------------------------------------
//
void CSensrvChannelChangeHandler::StartListening( MSensrvChannelChangeHandlerObserver* aChannelChangeObserver,
                                                  const TSensrvChannelInfo& aSearchParameters )
    {
    COMPONENT_TRACE( ( _L( "Sensrv Client - CSensrvChannelChangeHandler::StartListening - Start" ) ) );
    if( !IsActive() )
        {
        iObserver = aChannelChangeObserver;
        iSearchParameterBuf = aSearchParameters;
        
        CreateAndSendRequest();
        }    
    COMPONENT_TRACE( ( _L( "Sensrv Client - CSensrvChannelChangeHandler::StartListening - Return" ) ) );
    }

// ---------------------------------------------------------------------------
// CSensrvChannelChangeHandler::CreateAndSendRequest
// ---------------------------------------------------------------------------
//
void CSensrvChannelChangeHandler::CreateAndSendRequest()
    {
    COMPONENT_TRACE( ( _L( "Sensrv Client - CSensrvChannelChangeHandler::CreateAndSendRequestL()" ) ) );
     
    iSensrvClient.SendReceiveAsync( ESensrvSrvReqAsyncChannelChangeNotification, 
                                TIpcArgs( &iChannelInfoPckgBuf, &iChangeTypePckgBuf, &iSearchParameterBuf ),
                                iStatus );

    SetActive();
        
    COMPONENT_TRACE( ( _L( "Sensrv Client - CSensrvChannelChangeHandler::CreateAndSendRequestL - Return" ) ) );
    }

