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
* Description:  CSensrvConditionHandler implementation
*
*/



#include "sensrvconditionhandler.h"
#include "sensrvtrace.h"
#include "sensrvconditionhandlerobserver.h"
#include "sensrvdefines.h"



// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// C++ constructor
// ---------------------------------------------------------------------------
//
CSensrvConditionHandler::CSensrvConditionHandler( RSensrvClient& aSensrvClient, 
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
void CSensrvConditionHandler::ConstructL()
    {    
    iWriteBuffer = HBufC8::NewL(iChannelInfo.iDataItemSize);
    iWriteBufferPtr = new (ELeave) TPtr8(iWriteBuffer->Des());
    }


// ---------------------------------------------------------------------------
// 2-phase constructor
// ---------------------------------------------------------------------------
//
CSensrvConditionHandler* CSensrvConditionHandler::NewL( RSensrvClient& aSensrvClient, 
                                        const TSensrvChannelInfo& aChannelInfo )
    {
    CSensrvConditionHandler* self = CSensrvConditionHandler::NewLC( aSensrvClient, aChannelInfo );
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// 2-phase constructor
// ---------------------------------------------------------------------------
//
CSensrvConditionHandler* CSensrvConditionHandler::NewLC( RSensrvClient& aSensrvClient, 
                                        const TSensrvChannelInfo& aChannelInfo )
    {
    CSensrvConditionHandler* self = new( ELeave ) CSensrvConditionHandler( aSensrvClient, aChannelInfo );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }


// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CSensrvConditionHandler::~CSensrvConditionHandler()
    {
    Cancel();
    
    delete iWriteBuffer;
    delete iWriteBufferPtr;
    }


// ---------------------------------------------------------------------------
// From class CActive
// ---------------------------------------------------------------------------
//
void CSensrvConditionHandler::DoCancel()
    {
    COMPONENT_TRACE( ( _L( "Sensrv Client - CSensrvConditionHandler::DoCancel()" ) ) );
    
    iListening = EFalse;
    iSensrvClient.SendReceiveSync( ESensrvSrvReqStopConditionListening, 
                                        TIpcArgs( iChannelInfo.iChannelId ) );
                                        
    COMPONENT_TRACE( ( _L( "Sensrv Client - CSensrvConditionHandler::DoCancel - return void" ) ) );
    }
    
// ---------------------------------------------------------------------------
// From class CActive
// ---------------------------------------------------------------------------
//
void CSensrvConditionHandler::RunL()
    {
    COMPONENT_TRACE( ( _L( "Sensrv Client - CSensrvConditionHandler::RunL - iStatus %d" ), iStatus.Int() ) );
	Deque();
	CActiveScheduler::Add(this);
    if( iStatus.Int() == KErrNone )
        {
        if( iErrorCount > 0 )
            {
            iErrorCount = 0;
            }

        // Extract id from response
        TInt metSetId = iMetSetIdBuf();
                
        // Notify to listener
        // Note: Callback is done intentionally before resending async request,
        // so that we don't need duplicate buffer like in data reading.
        // It shouldn't have noticeable effect on performance in real use cases,
        // and reduces overhead, as condition listening is quite often stopped 
        // inside ConditionMet method.
        iConditionObserver->ConditionMet(metSetId, *iWriteBuffer);

        CreateAndSendRequest();
        }
    else
        {
        if( iErrorCount == 0 )
            {
            iErrorCount++;
            CreateAndSendRequest();
            
            // Send minor error
            iConditionObserver->ConditionError( ESensrvErrorSeverityMinor );
            
            }
        else if( iErrorCount == 1 )
            {
            iErrorCount++;

            // Reopen channel
            iConditionObserver->ReopenChannelForConditionListeningL();
            
            TSensrvStartListeningParametersPckgBuf startListeningPckg( iListeningParameters );
            
            TInt err = iSensrvClient.SendReceiveSync( ESensrvSrvReqStartConditionListening, 
                            TIpcArgs( iChannelInfo.iChannelId, &startListeningPckg ) );
            if( err )
                {
                // send fatal error
                iListening = EFalse;
                iConditionObserver->ConditionError( ESensrvErrorSeverityFatal );
                }
            else
                {
                // Start listening
                CreateAndSendRequest();
                }
            }
        
        else
            {
            // Send fatal error
            iListening = EFalse;
            iConditionObserver->ConditionError( ESensrvErrorSeverityFatal );
            }
        }
    }
    
// ---------------------------------------------------------------------------
// CSensrvConditionHandler::RunError
// ---------------------------------------------------------------------------
//
TInt CSensrvConditionHandler::RunError( TInt /*aError*/ )
    {
    COMPONENT_TRACE( ( _L( "Sensrv Client - CSensrvConditionHandler::RunError" ) ) );
    
    // Send fatal error
    iListening = EFalse;
    iConditionObserver->ConditionError( ESensrvErrorSeverityFatal );   
    return KErrNone;
    }


// ---------------------------------------------------------------------------
// CSensrvConditionHandler::StartListening
// ---------------------------------------------------------------------------
//
void CSensrvConditionHandler::StartListening( MSensrvConditionHandlerObserver* aConditionObserver, 
                                TSensrvStartListeningParameters aListeningParameters)
    {
    COMPONENT_TRACE( ( _L( "Sensrv Client - CSensrvConditionHandler::StartListening - Start" ) ) );
    
    if( !iListening )
        {
        iConditionObserver = aConditionObserver;
        iListeningParameters = aListeningParameters;
        iListening = ETrue;
            
        iErrorCount = 0;
        
        CreateAndSendRequest();
        }
            
    COMPONENT_TRACE( ( _L( "Sensrv Client - CSensrvConditionHandler::StartListening - Return" ) ) );
    }

// ---------------------------------------------------------------------------
// CSensrvConditionHandler::CreateAndSendRequestL
// ---------------------------------------------------------------------------
//
void CSensrvConditionHandler::CreateAndSendRequest()
    {
    COMPONENT_TRACE( ( _L( "Sensrv Client - CSensrvConditionHandler::CreateAndSendRequestL - Start" ) ) );
    
    if (iListening && !IsActive() )
        {
        iSensrvClient.SendReceiveAsync( ESensrvSrvReqAsyncConditionMet, 
                TIpcArgs( iChannelInfo.iChannelId, iWriteBufferPtr, &iMetSetIdBuf ), iStatus );
        
        SetActive();
        }
    
    COMPONENT_TRACE( ( _L( "Sensrv Client - CSensrvConditionHandler::CreateAndSendRequestL - Return" ) ) );
    }

