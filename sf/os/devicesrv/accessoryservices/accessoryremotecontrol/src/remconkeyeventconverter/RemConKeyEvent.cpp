/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Key event class for internal use in converter. Maps key 
*                actions, operation IDs, event prerequisities etc. to
*                TRemConKeyEventData -instances.
*
*/


// INCLUDE FILES
#include "RemConKeyEvent.h"
#include "RemConDebug.h"

// LOCAL CONSTANTS AND MACROS 

// Length of KCoreApiParameterLength is sufficient as long as values of 
// TRemConCoreApiButtonAction are from 0 to 2^32
const TInt KCoreApiParameterLength = 10;

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CRemConKeyEvent::CRemConKeyEvent
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CRemConKeyEvent::CRemConKeyEvent()
    {
       COM_TRACE_( "[AccFW:RemConConverter] CRemConKeyEvent::CRemConKeyEvent()" );
    }

// -----------------------------------------------------------------------------
// CRemConKeyEvent::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CRemConKeyEvent::ConstructL( const TRemConKeyEventData& aKeyEvent, 
    const TUid& aInterfaceUID,
    const TUint& aOperationID,
    TDesC8& aParameter )
    {
    COM_TRACE_2( "[AccFW:RemConConverter] CRemConKeyEvent::ConstructL( %d, %d )", aKeyEvent, aOperationID );

	iKeyEvent = aKeyEvent;
    iInterfaceUID = aInterfaceUID;
    iOperationID = aOperationID;
    iParameter = HBufC8::NewL( aParameter.Size() );
    iParameter->Des().Copy( aParameter );
    }

// -----------------------------------------------------------------------------
// CRemConKeyEvent::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CRemConKeyEvent* CRemConKeyEvent::NewL( const TRemConKeyEventData& aKeyEvent, 
    const TUid& aInterfaceUid, 
    const TUint& aOperationID,
    const TRemConCoreApiButtonAction& aButtonAction )
    {
    COM_TRACE_2( "[AccFW:RemConConverter] CRemConKeyEvent::NewL( %d, %d )", aKeyEvent, aOperationID );
		
    CRemConKeyEvent* self = new( ELeave ) CRemConKeyEvent;
    CleanupStack::PushL( self );
    
    TBuf8< KCoreApiParameterLength > parameter;
    parameter.Append( aButtonAction );
    self->ConstructL( aKeyEvent, aInterfaceUid, aOperationID, parameter );
    
    CleanupStack::Pop( self );
    return self;
    }
    
// -----------------------------------------------------------------------------
// CRemConKeyEvent::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CRemConKeyEvent* CRemConKeyEvent::NewL( const TRemConKeyEventData& aKeyEvent, 
    const TUid& aInterfaceUid, 
    const TUint& aOperationID,
    TDesC8& aParameter )
    {
    COM_TRACE_2( "[AccFW:RemConConverter] CRemConKeyEvent::NewL( %d, %d )", aKeyEvent, aOperationID );
	
	CRemConKeyEvent* self = new( ELeave ) CRemConKeyEvent;
    
    CleanupStack::PushL( self );
    self->ConstructL( aKeyEvent, aInterfaceUid, aOperationID, aParameter );
    
    CleanupStack::Pop( self );
    return self;
    }
    
// Destructor
CRemConKeyEvent::~CRemConKeyEvent()
    {
    COM_TRACE_( "[AccFW:RemConConverter] CRemConKeyEvent::~CRemConKeyEvent()" );
    
    delete iParameter;
    
    COM_TRACE_( "[AccFW:RemConConverter] CRemConKeyEvent::~CRemConKeyEvent() return void." );
    }

// -----------------------------------------------------------------------------
// CRemConKeyEvent::Compare
// Compare two TRemConKeyEventData instances for equality.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TBool CRemConKeyEvent::Compare( const TRemConKeyEventData& aKeyEvent ) const
    {
    COM_TRACE_1( "[AccFW:RemConConverter] CRemConKeyEvent::Compare( %d )", aKeyEvent );
    COM_TRACE_2( "[AccFW:RemConConverter] key code     : l:%d r:%d ", iKeyEvent.iKeyCode, aKeyEvent.iKeyCode );
    COM_TRACE_2( "[AccFW:RemConConverter] key modifiers: l:%d r:%d ", iKeyEvent.iKeyModifiers, aKeyEvent.iKeyModifiers );
    COM_TRACE_2( "[AccFW:RemConConverter] key state    : l:%d r:%d ", iKeyEvent.iKeyState, aKeyEvent.iKeyState );
    COM_TRACE_2( "[AccFW:RemConConverter] key event    : l:%d r:%d ", iKeyEvent.iKeyEventType, aKeyEvent.iKeyEventType );

    TBool match( ( aKeyEvent.iKeyCode == iKeyEvent.iKeyCode ) &&
        ( aKeyEvent.iKeyEventType == iKeyEvent.iKeyEventType ) &&
        ( aKeyEvent.iKeyModifiers == iKeyEvent.iKeyModifiers ) &&
        ( aKeyEvent.iKeyState == iKeyEvent.iKeyState ) );

    COM_TRACE_1( "[AccFW:RemConConverter] Match: %d. ",match );
    return ( match );
    }

// -----------------------------------------------------------------------------
// CRemConKeyEvent::InterfaceUID
// Return the interface UID of a certain key-event.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TUid CRemConKeyEvent::InterfaceUID() const
    {
    COM_TRACE_( "[AccFW:RemConConverter] CRemConKeyEvent::OperationID()" );
    return iInterfaceUID;
    }
    
// -----------------------------------------------------------------------------
// CRemConKeyEvent::OperationID
// Return the operation ID of a certain key-event.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TUint CRemConKeyEvent::OperationID() const
    {
    COM_TRACE_( "[AccFW:RemConConverter] CRemConKeyEvent::OperationID()" );
    return iOperationID;
    }

// -----------------------------------------------------------------------------
// CRemConKeyEvent::Parameter
// Returns parameter of a certain key-event.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TDesC8& CRemConKeyEvent::Parameter() const
    {
    COM_TRACE_( "[AccFW:RemConConverter] CRemConKeyEvent::Parameter()" );
    return *iParameter;
    //return iParameter;
    }

//  End of File  
