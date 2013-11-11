/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Implementation of test server classes
*
*/


// INCLUDE FILES
#include "ctfstub.h"
#include "ctfremotestub.h"
#include "ctffactory.h"
#include "dsytesttooltrace.h"
#include "ctftestserver.h"

const TInt KTFStateRunning          = 1;
const TInt KTFStateCompleting       = 2;

// -----------------------------------------------------------------------------
// RTFTestServer::Connect
// -----------------------------------------------------------------------------
TInt RTFTestServer::Connect( TBool aLocal )
    {
    TInt retval;
    if ( aLocal ) 
        {
        retval = CreateSession( KTFLocalTestServerName, TVersion( 1, 0, 0 ) );
        }
    else
        {
        retval = CreateSession( KTFRemoteTestServerName, TVersion( 1, 0, 0 ) );
        }
    return retval;
    }


// -----------------------------------------------------------------------------
// RTFTestServer::PostMessage
// -----------------------------------------------------------------------------
void RTFTestServer::PostMessage( TInt aStubType, TAny* aMessageData1, TAny* aMessageData2, TAny* aMessageData3, TAny* aMessageData4, TRequestStatus& aStatus ) const
    {
    SendReceive( aStubType, TIpcArgs( aMessageData1, aMessageData2, aMessageData3, aMessageData4 ), aStatus );
    }


// -----------------------------------------------------------------------------
// RTFTestServer::PostMessage
// -----------------------------------------------------------------------------
TInt RTFTestServer::PostMessage( TInt aStubType, TAny* aMessageData1, TAny* aMessageData2, TAny* aMessageData3, TAny* aMessageData4 ) const
    {
    return SendReceive( aStubType, TIpcArgs( aMessageData1, aMessageData2, aMessageData3, aMessageData4 ) );
    }


// -----------------------------------------------------------------------------
// RTFTestServer::PostCompletionMessage
// -----------------------------------------------------------------------------
void RTFTestServer::PostCompletionMessage( TInt aStubType, TInt aResult, TRequestStatus& aStatus ) const
    {
    SendReceive( 0, TIpcArgs( aStubType, aResult ), aStatus );
    }


// -----------------------------------------------------------------------------
// RTFTestServer::PostShutdownMessage
// -----------------------------------------------------------------------------
void RTFTestServer::PostShutdownMessage() const
    {
    SendReceive( 0, TIpcArgs( -1 ) );
    }


// -----------------------------------------------------------------------------
// CTFTestServerWrapper::CTFTestServerWrapper
// -----------------------------------------------------------------------------
CTFTestServerWrapper::CTFTestServerWrapper( void )
    : CActive( EPriorityNormal )
    {
    CActiveScheduler::Add( this );
    }


// -----------------------------------------------------------------------------
// CTFTestServerWrapper::~CTFTestServerWrapper
// -----------------------------------------------------------------------------
CTFTestServerWrapper::~CTFTestServerWrapper( void )
    {
    iClient.Close();
    iCallerStatus = NULL;
    }


// -----------------------------------------------------------------------------
// CTFTestServerWrapper::PostMessageLD
// -----------------------------------------------------------------------------
void CTFTestServerWrapper::PostMessageLD( TInt aStubType, TAny* aMessageData1, TAny* aMessageData2, TAny* aMessageData3, TAny* aMessageData4, TRequestStatus& aStatus )
    {
    COMPONENT_TRACE( ( _L( "    DSYTESTTOOL - CTFTestServerWrapper::PostMessageLD(0x%x, 0x%x, 0x%x, 0x%x, 0x%x)" ), aStubType, aMessageData1, aMessageData2, aMessageData3, &aStatus ) );
    iStubType = aStubType;
    aStatus = KRequestPending;
    iCallerStatus = &aStatus;
    User::LeaveIfError( iClient.Connect( EFalse ) );
    iState = KTFStateCompleting;
    iClient.PostMessage( aStubType, aMessageData1, aMessageData2, aMessageData3, aMessageData4, iStatus );
    SetActive();
    COMPONENT_TRACE( ( _L( "    DSYTESTTOOL - CTFTestServerWrapper::PostMessageLD - return void" ) ) );
    }


// -----------------------------------------------------------------------------
// CTFTestServerWrapper::PostLocalMessageL
// -----------------------------------------------------------------------------
void CTFTestServerWrapper::PostLocalMessageL( TInt aStubType, TAny* aMessageData1, TAny* aMessageData2, TAny* aMessageData3, TAny* aMessageData4 )
    {
    COMPONENT_TRACE( ( _L( "    DSYTESTTOOL - CTFTestServerWrapper::PostLocalMessageL(0x%x, 0x%x, 0x%x, 0x%x, 0x%x)" ), aStubType, aMessageData1, aMessageData2, aMessageData3, aMessageData4 ) );
    iStubType = aStubType;
    User::LeaveIfError( iClient.Connect( ETrue ) );
    iState = KTFStateRunning;
    iClient.PostMessage( aStubType, aMessageData1, aMessageData2, aMessageData3, aMessageData4, iStatus );
    SetActive();
    COMPONENT_TRACE( ( _L( "    DSYTESTTOOL - CTFTestServerWrapper::PostMessageL - return void" ) ) );
    }


// -----------------------------------------------------------------------------
// CTFTestServerWrapper::RunL
// -----------------------------------------------------------------------------
void CTFTestServerWrapper::RunL( void )
    {
    COMPONENT_TRACE( ( _L( "    DSYTESTTOOL - CTFTestServerWrapper::RunL()" ) ) );
    if ( iState == KTFStateRunning )
        {
        iState = KTFStateCompleting;
        iClient.PostCompletionMessage( iStubType, iStatus.Int(), iStatus );
        SetActive();
        }
    else
        {
        if ( iCallerStatus != NULL )
            {
            User::RequestComplete( iCallerStatus, iStatus.Int() );
            delete this;
            }
        else
            {
            CActiveScheduler::Stop();
            }
        }
    COMPONENT_TRACE( ( _L( "    DSYTESTTOOL - CTFTestServerWrapper::RunL - return void" ) ) );
    }


// -----------------------------------------------------------------------------
// CTFTestServerWrapper::DoCancel
// -----------------------------------------------------------------------------
void CTFTestServerWrapper::DoCancel( void )
    {
    }


// -----------------------------------------------------------------------------
// CTFTestSession::CTFTestSession
// -----------------------------------------------------------------------------
CTFTestSession::CTFTestSession( CTFFactory* aFactory )
    : iFactory( aFactory )
    {
    }


// -----------------------------------------------------------------------------
// CTFTestSession::ServiceL
// -----------------------------------------------------------------------------
void CTFTestSession::ServiceL( const RMessage2& aMessage )
    {
    COMPONENT_TRACE( ( _L( "    DSYTESTTOOL - CTFTestSession::ServiceL(0x%x)" ), &aMessage ) );
    if ( aMessage.Function() != 0 )
        {
        CTFStub* stub = iFactory->InternalStub( aMessage.Function() );
        if ( stub != NULL && stub->IsRemoteStub() )
            {
            TRAPD( err, STATIC_CAST( CTFRemoteStub*, stub )->ProcessMessageAndCompleteL( aMessage ) );
            if ( err != KErrNone )
                {
                aMessage.Complete( err );
                }
            }
        else
            {
            aMessage.Complete( KErrNotSupported );
            }
        }
    else
        {
        if ( aMessage.Int0() == -1 )
            {
            CActiveScheduler::Stop();
            aMessage.Complete( KErrNone );
            }
        else
            {
            CTFStub* stub = iFactory->InternalStub( aMessage.Int0() );
            if ( stub && stub->IsRemoteStub() )
                {
                TRAPD( err, STATIC_CAST( CTFRemoteStub*, stub )->MessageCompleteL( aMessage.Int1() ) );
                aMessage.Complete( err );
                }
            else
                {
                aMessage.Complete( KErrNotSupported );
                }
            }
        }
    COMPONENT_TRACE( ( _L( "    DSYTESTTOOL - CTFTestSession::ServiceL - return void" ) ) );
    }


// -----------------------------------------------------------------------------
// CTFTestServer::CTFTestServer
// -----------------------------------------------------------------------------
CTFTestServer::CTFTestServer( CTFFactory* aFactory )
    : CServer2( EPriorityNormal )
    , iFactory( aFactory )
    {
    }


// -----------------------------------------------------------------------------
// CTFTestServer::NewSessionL
// -----------------------------------------------------------------------------
CSession2* CTFTestServer::NewSessionL( const TVersion& /*aVersion*/, const RMessage2& /*aMessage*/ ) const
    {
    COMPONENT_TRACE( ( _L( "    DSYTESTTOOL - CTFTestServer::NewSessionL()" ) ) );
    CSession2* retval = new ( ELeave ) CTFTestSession( iFactory );
    COMPONENT_TRACE( ( _L( "    DSYTESTTOOL - CTFTestServer::NewSessionL - return 0x%x" ), retval ) );
    return retval;
    }


