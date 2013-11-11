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
* Description:  Implementation for the key event handling API.
*
*/



// INCLUDE FILES
#include    "RemConKeyEventConverterImpl.h"
#include    "RemConDebug.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CRemConKeyEventConverterImpl::CRemConKeyEventConverterImpl
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CRemConKeyEventConverterImpl::CRemConKeyEventConverterImpl()
    {
    COM_TRACE_( "[AccFW:RemConConverter] CRemConKeyEventConverterImpl::CRemConKeyEventConverterImpl()" );
    }

// -----------------------------------------------------------------------------
// CRemConKeyEventConverterImpl::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CRemConKeyEventConverterImpl::ConstructL()
    {
    COM_TRACE_( "[AccFW:RemConConverter] CRemConKeyEventConverterImpl::ConstructL()" );
	iKeyEventContainer = CRemConKeyEventContainer::NewL();
    COM_TRACE_( "[AccFW:RemConConverter] CRemConKeyEventConverterImpl::ConstructL() return void" );
    }

// -----------------------------------------------------------------------------
// CRemConKeyEventConverterImpl::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CRemConKeyEventConverterImpl* CRemConKeyEventConverterImpl::NewL()
    {
    COM_TRACE_( "[AccFW:RemConConverter] CRemConKeyEventConverterImpl::NewL()" );

    CRemConKeyEventConverterImpl* self = new( ELeave ) 
        CRemConKeyEventConverterImpl;
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    COM_TRACE_1( "[AccFW:RemConConverter] CRemConKeyEventConverterImpl::NewL() return %d", self );
    return self;
    }

// Destructor
CRemConKeyEventConverterImpl::~CRemConKeyEventConverterImpl()
    {
    COM_TRACE_( "[AccFW:RemConConverter] CRemConKeyEventConverterImpl::~CRemConKeyEventConverterImpl()" );

    delete iKeyEventContainer;
    iKeyEventContainer = 0;
    COM_TRACE_( "[AccFW:RemConConverter] CRemConKeyEventConverterImpl::~CRemConKeyEventConverterImpl() return void" );
    }

// -----------------------------------------------------------------------------
// CRemConKeyEventConverterImpl::ResolveOperationID()
// Handle key event received from accessory.
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CRemConKeyEventConverterImpl::ResolveOperationID( 
    const TRemConKeyEventData& aEvent, TUid& aInterfaceUID, 
    TUint& aOperationID, TDes8& aData ) const
    {
    COM_TRACE_1( "[AccFW:RemConConverter] CRemConKeyEventConverterImpl::ResolveOperationID(%d)", aEvent );
	COM_TRACE_4( "[AccFW:RemConConverter] Event: Code:%d, Mod:%d, State:%d, Type:%d", aEvent.iKeyCode, aEvent.iKeyModifiers, aEvent.iKeyState, aEvent.iKeyEventType );
    	
    TInt ret( KErrNotSupported ); 
    const RPointerArray<CRemConKeyEvent>& events( iKeyEventContainer->GetEvents() );
    
    // Go through all supported events
    for ( TInt i(0); i < events.Count(); ++i )
        {
        CRemConKeyEvent& event( * ( events[i] ) );
        if ( event.Compare( aEvent ) )
            {
            COM_TRACE_( "[AccFW:RemConConverter] Event was found !" );
            
            if ( aData.MaxSize() <= 0 )
            	{
                // No room for the key action -data
                COM_TRACE_2( "[AccFW:RemConConverter] The ref. descriptor from bearer has too small max size (%d) , should be at least %d", aData.MaxSize(), event.Parameter().Size() );
                return KErrGeneral;
                }
            
            aInterfaceUID = event.InterfaceUID();
            aOperationID = event.OperationID();
            aData.Zero(); // Just in case
            aData.Append( event.Parameter() );
            
            ret = KErrNone;
            
            COM_TRACE_1( "[AccFW:RemConConverter] Interface ID for this event is: %d", aInterfaceUID.iUid );
            COM_TRACE_1( "[AccFW:RemConConverter] Operation ID for this event is: %d", aOperationID );
            COM_TRACE_1( "[AccFW:RemConConverter] Parameter for this event is: %S", &aData );
                       
            break; // from for -loop
            }
        }
    
    COM_TRACE_1( "[AccFW:RemConConverter] CRemConKeyEventConverterImpl::ResolveOperationID() return %d", ret );
    return ret;
    }

//  End of File  
