/*
* Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Power save mode client class
*
*/


#include <psmclientobserver.h>
#include "psmclientimpl.h"
#include "psmsettingsimpl.h"
#include "psmclientactive.h"
#include "psmtrace.h"

// -----------------------------------------------------------------------------
// CPsmClientImpl::CPsmClientImpl
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPsmClientImpl::CPsmClientImpl( MPsmClientObserver& aObserver ) :
    iObserver( aObserver )
    {
    COMPONENT_TRACE( ( _L( "PSM Client - CPsmClientImpl::CPsmClientImpl()" ) ) );
    COMPONENT_TRACE( ( _L( "PSM Client - CPsmClientImpl::CPsmClientImpl - return" ) ) );
    }

// -----------------------------------------------------------------------------
// CPsmClientImpl::ConstructL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
void CPsmClientImpl::ConstructL()
    {
    COMPONENT_TRACE( ( _L( "PSM Client - CPsmClientImpl::ConstructL()" ) ) );
    
    // Connect to PSM server
    User::LeaveIfError( iPsmServer.Connect() );

    // Create an instance of settings provider
    iSettings = new ( ELeave ) CPsmSettingsImpl( iPsmServer );

    // Create active object
    iActive = new ( ELeave ) CPsmClientActive( iObserver );

    COMPONENT_TRACE( ( _L( "PSM Client - CPsmClientImpl::ConstructL - return" ) ) );
    }

// -----------------------------------------------------------------------------
// CPsmClientImpl::~CPsmClientImpl
// Destructor
// -----------------------------------------------------------------------------
//
CPsmClientImpl::~CPsmClientImpl()
    {
    COMPONENT_TRACE( ( _L( "PSM Client - CPsmClientImpl::~CPsmClientImpl()" ) ) );
    
    CancelPowerSaveModeChange();
    delete iActive;
    delete iSettings;
    iPsmServer.Close();

    COMPONENT_TRACE( ( _L( "PSM Client - CPsmClientImpl::~CPsmClientImpl - return" ) ) );
    }

// -----------------------------------------------------------------------------
// CPsmClientImpl::ChangePowerSaveMode
// -----------------------------------------------------------------------------
//
void CPsmClientImpl::ChangePowerSaveMode( const TInt aPsmMode )
    {
    COMPONENT_TRACE( ( _L( "PSM Client - CPsmClientImpl::ChangePowerSaveMode()" ) ) );

    if ( iActive->IsActive() )
        {
        COMPONENT_TRACE( ( _L( "PSM Client - CPsmClientImpl::ChangePowerSaveMode() - Already active!" ) ) );
        iActive->Cancel();
        }

    // Send asynchronous message and set this object to active
    iPsmServer.SendReceiveAsync( 
                    EPsmServerNotifyModeChange, 
                    TIpcArgs( aPsmMode ), 
                    iActive->RequestStatus() );

    iActive->SetActive();

    COMPONENT_TRACE( ( _L( "PSM Client - CPsmClientImpl::ChangePowerSaveMode - return" ) ) );
    }

// -----------------------------------------------------------------------------
// CPsmClientImpl::CancelPowerSaveModeChange
// -----------------------------------------------------------------------------
//
void CPsmClientImpl::CancelPowerSaveModeChange()
    {
    COMPONENT_TRACE( ( _L( "PSM Client - CPsmClientImpl::CancelPowerSaveModeChange()" ) ) );

    if ( iActive->IsActive() )
        {
        // Send cancel asynchronous message
        iPsmServer.SendReceiveSync( EPsmServerCancelModeChange, TIpcArgs() );
        }

    COMPONENT_TRACE( ( _L( "PSM Client - CPsmClientImpl::CancelPowerSaveModeChange - return" ) ) );
    }

// -----------------------------------------------------------------------------
// CPsmClientImpl::RequestPowerSaveModeNotification
// -----------------------------------------------------------------------------
//
void CPsmClientImpl::RequestPowerSaveModeNotification()
    {
    COMPONENT_TRACE( ( _L( "PSM Client - CPsmClientImpl::RequestPowerSaveModeNotification()" ) ) );

    if ( iActive->IsActive() )
        {
        COMPONENT_TRACE( ( _L( "PSM Client - CPsmClientImpl::RequestPowerSaveModeNotification() - Already active!" ) ) );
        iActive->Cancel();
        }

    // Send asynchronous message and set this object to active
    iPsmServer.SendReceiveAsync( 
                    EPsmServerModeChangeNotificationRequest, 
                    TIpcArgs(), 
                    iActive->RequestStatus() );

    iActive->SetActive();

    COMPONENT_TRACE( ( _L( "PSM Client - CPsmClientImpl::RequestPowerSaveModeNotification - return" ) ) );
    }

// -----------------------------------------------------------------------------
// CPsmClientImpl::CancelPowerSaveModeNotificationRequest
// -----------------------------------------------------------------------------
//
void CPsmClientImpl::CancelPowerSaveModeNotificationRequest()
    {
    COMPONENT_TRACE( ( _L( "PSM Client - CPsmClientImpl::CancelPowerSaveModeNotificationRequest()" ) ) );

    if ( iActive->IsActive() )
        {
        // Send cancel asynchronous message
        iPsmServer.SendReceiveSync( EPsmServerCancelModeChangeNotificationRequest, TIpcArgs() );
        }

    COMPONENT_TRACE( ( _L( "PSM Client - CPsmClientImpl::CancelPowerSaveModeNotificationRequest - return" ) ) );
    }

// -----------------------------------------------------------------------------
// CPsmClientImpl::PsmSettings
// -----------------------------------------------------------------------------
//
CPsmSettings& CPsmClientImpl::PsmSettings() const
    {
    COMPONENT_TRACE( ( _L( "PSM Client - CPsmClientImpl::PsmSettings()" ) ) );
    COMPONENT_TRACE( ( _L( "PSM Client - CPsmClientImpl::PsmSettings - return" ) ) );
    return *iSettings;
    }

#ifdef _DEBUG     
TInt CPsmClientImpl::HeapMark()
    {
    COMPONENT_TRACE( ( _L( "PSM Client - CPsmClientImpl::HeapMark()" ) ) );
    TInt err = iPsmServer.SendReceiveSync( EDebugHeapMark, TIpcArgs() );
    COMPONENT_TRACE( ( _L( "PSM Client - CPsmClientImpl::HeapMark - return" ) ) );   
    return err;
    }

TInt CPsmClientImpl::HeapMarkEnd()
    {
    COMPONENT_TRACE( ( _L( "PSM Client - CPsmClientImpl::HeapMarkEnd()" ) ) );
    TInt err = iPsmServer.SendReceiveSync( EDebugHeapMarkEnd, TIpcArgs() );   
    COMPONENT_TRACE( ( _L( "PSM Client - CPsmClientImpl::HeapMarkEnd - return" ) ) );   
    return err;    
    }

TInt CPsmClientImpl::SetHeapFailure(TInt aFailureRate)
    {
    COMPONENT_TRACE( ( _L( "PSM Client - CPsmClientImpl::SetHeapFailure()" ) ) );
    TInt err = iPsmServer.SendReceiveSync( EDebugSetHeapFailure, TIpcArgs(aFailureRate) );
    COMPONENT_TRACE( ( _L( "PSM Client - CPsmClientImpl::SetHeapFailure - return" ) ) );   
    return err; 
    }

TInt CPsmClientImpl::HeapReset()
    {
    COMPONENT_TRACE( ( _L( "PSM Client - CPsmClientImpl::HeapReset()" ) ) );
    TInt err = iPsmServer.SendReceiveSync( EDebugHeapReset, TIpcArgs() );
    COMPONENT_TRACE( ( _L( "PSM Client - CPsmClientImpl::HeapReset - return" ) ) );   
    return err; 
    }

#endif //_DEBUG     

// End of file
