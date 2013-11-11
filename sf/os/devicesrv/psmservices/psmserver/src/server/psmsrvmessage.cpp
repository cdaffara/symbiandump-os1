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
* Description:  PSM Server message
*
*/


#include <psmtypes.h>
#include "psmsrvmessage.h"
#include "psmmanager.h"
#include "psmsettingsprovider.h"
#include "psmclientserver.h"
#include "psmtrace.h"

// -----------------------------------------------------------------------------
// CPsmMessage::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPsmMessage* CPsmMessage::NewL( CPsmManager& aPsmManager )
    {
    COMPONENT_TRACE( ( _L( "PSM Server - CPsmMessage::NewL()" ) ) );

    CPsmMessage* self = CPsmMessage::NewLC( aPsmManager );
    CleanupStack::Pop( self );

    COMPONENT_TRACE( ( _L( "PSM Server - CPsmMessage::NewL - return 0x%x" ), self ) );

    return self;
    }

// -----------------------------------------------------------------------------
// CPsmMessage::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPsmMessage* CPsmMessage::NewLC( CPsmManager& aPsmManager )
    {
    COMPONENT_TRACE( ( _L( "PSM Server - CPsmMessage::NewLC()" ) ) );

    CPsmMessage* self = new( ELeave ) CPsmMessage( aPsmManager );

    CleanupStack::PushL( self );
    self->ConstructL();

    COMPONENT_TRACE( ( _L( "PSM Server - CPsmMessage::NewLC - return 0x%x" ), self ) );

    return self;
    }

// -----------------------------------------------------------------------------
// CPsmMessage::CPsmMessage
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPsmMessage::CPsmMessage( CPsmManager& aPsmManager ) :
    iPsmManager( aPsmManager )
    {
    // Nothing to do
    }

// -----------------------------------------------------------------------------
// CPsmMessage::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CPsmMessage::ConstructL()
    {
    COMPONENT_TRACE( ( _L( "PSM Server - CPsmMessage::ConstructL()" ) ) );
    COMPONENT_TRACE( ( _L( "PSM Server - CPsmMessage::ConstructL - return" ) ) );
    }

// -----------------------------------------------------------------------------
// Initializes the message
// -----------------------------------------------------------------------------
//
void CPsmMessage::Initialize(const RMessage2& aMessage)
    {
    COMPONENT_TRACE( ( _L( "PSM Server - CPsmMessage::Initialize()" ) ) );

    if ( iMessage.Handle() )
        {
        ERROR_TRACE( ( _L( "PSM Server - CPsmMessage::Initialize - ERROR: Previous message not completed, canceling it" ) ) );
        iMessage.Complete( KErrCancel );
        }

    iMessage = aMessage;

    COMPONENT_TRACE( ( _L( "PSM Server - CPsmMessage::Initialize - return") ) );
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
CPsmMessage::~CPsmMessage()
    {
    COMPONENT_TRACE( ( _L( "PSM Server - CPsmMessage::~CPsmMessage()" ) ) );
    COMPONENT_TRACE( ( _L( "PSM Server - CPsmMessage::~CPsmMessage - return") ) );
    }

// -----------------------------------------------------------------------------
// Completes the message
// -----------------------------------------------------------------------------
//
void CPsmMessage::Complete( TInt aReason ) const
    {
    COMPONENT_TRACE( ( _L( "PSM Server - CPsmMessage::Complete(%d)" ), aReason ) );

    if ( iMessage.Handle() )
        {
        iMessage.Complete( aReason );
        }
    else
        {
        COMPONENT_TRACE( ( _L( "PSM Server - CPsmMessage::Complete - No handle!" ) ) );
        }

    COMPONENT_TRACE( ( _L( "PSM Server - CPsmMessage::Complete - return" ) ) );
    }

// -----------------------------------------------------------------------------
// CPsmMessage::NotifyPsmModeChangeComplete
// -----------------------------------------------------------------------------
//
void CPsmMessage::NotifyPsmModeChangeComplete( const TInt aError )
    {
    COMPONENT_TRACE( ( _L( "PSM Server - CPsmMessage::NotifyPsmModeChangeComplete()" ) ) );

    // Complete message with error code if request is pending for PSM change to 
    // complete or if error code is other than KErrNone
    if ( KErrNone != aError )
        {
        Complete( aError );
        }
    else
        {
        // Request is pending notification about PSM change, complete with mode
        Complete( iPsmManager.SettingsProvider().Mode() );
        }

    COMPONENT_TRACE( ( _L( "PSM Server - CPsmMessage::NotifyPsmModeChangeComplete - return" ) ) );
    }

// -----------------------------------------------------------------------------
// CPsmMessage::MessageHandle
// -----------------------------------------------------------------------------
//
TInt CPsmMessage::MessageHandle() const
        {
        return iMessage.Handle();
        }

// End of file
