/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Implementation of Accessory selection dialog.
*
*/


// INCLUDE FILES
#include "AccFwUiDialogNotifierStub.h"
#include "AccFwUiNotifierStubObserver.h"
#include "TfAccessoryTestControlTypes.h"
#include "acc_debug.h"

//#include <accfwnoteuinotifier.rsg>
#include <aknlistquerydialog.h> 
#include <aknlists.h> 
#include <bautils.h>
#include <data_caging_path_literals.hrh> 

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS

const TUid KScreenOutputChannel = {0x00000666};

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================
   
// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CAccFwUiDialogNotifierStub::CAccFwUiDialogNotifier()
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CAccFwUiDialogNotifierStub::CAccFwUiDialogNotifierStub()
	//: CActive( EPriorityStandard)	
    {
    API_TRACE_( "[AccFW: ACCFWUINOTIFIER] CAccFwUiDialogNotifierStub::CAccFwUiDialogNotifier() - enter" );
    
    API_TRACE_( "[AccFW: ACCFWUINOTIFIER] CAccFwUiDialogNotifierStub::CAccFwUiDialogNotifier() - return void" );
    }
   
// Destructor
CAccFwUiDialogNotifierStub::~CAccFwUiDialogNotifierStub()
    {
    API_TRACE_( "[AccFW: ACCFWUINOTIFIER] CAccFwUiDialogNotifierStub::~CAccFwUiNoteNotifier() - enter" );
    
    //delete iObserver;
    
    API_TRACE_( "[AccFW: ACCFWUINOTIFIER] CAccFwUiDialogNotifierStub::~CAccFwUiNoteNotifier() - return void" );
    }

// -----------------------------------------------------------------------------
// CAccFwUiDialogNotifierStub::ConstructL()
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CAccFwUiDialogNotifierStub::ConstructL()
    {
    API_TRACE_( "[AccFW: ACCFWUINOTIFIER] CAccFwUiDialogNotifierStub::ConstructL() - enter" );

//	iObserver = CAccFWUINotifierStubObserver::NewL( this);


    API_TRACE_( "[AccFW: ACCFWUINOTIFIER] CAccFwUiDialogNotifierStub::ConstructL() - return void" );
    }

// -----------------------------------------------------------------------------
// CAccFwUiDialogNotifierStub::NewL()
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CAccFwUiDialogNotifierStub* CAccFwUiDialogNotifierStub::NewL()
    {
    API_TRACE_( "[AccFW: ACCFWUINOTIFIER] CAccFwUiDialogNotifier::NewL() - enter" );

    CAccFwUiDialogNotifierStub* self = new( ELeave ) CAccFwUiDialogNotifierStub;
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    API_TRACE_( "[AccFW: ACCFWUINOTIFIER] CAccFwUiDialogNotifierStub::CAccFwUiDialogNotifier::NewL() - return" );
    return self;
    }

// -----------------------------------------------------------------------------
// CAccFwUiDialogNotifierStub::Release()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CAccFwUiDialogNotifierStub::Release()
    {
    API_TRACE_( "[AccFW: ACCFWUINOTIFIER] CAccFwUiDialogNotifierStub::Release() - enter" );
    
	delete this;

    API_TRACE_( "[AccFW: ACCFWUINOTIFIER] CAccFwUiDialogNotifierStub::Release() - return void" );
    }

// -----------------------------------------------------------------------------
// CAccFwUiDialogNotifierStub::RegisterL()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
CAccFwUiDialogNotifierStub::TNotifierInfo CAccFwUiDialogNotifierStub::RegisterL()
    {
    API_TRACE_( "[AccFW: ACCFWUINOTIFIER] CAccFwUiDialogNotifierStub::RegisterL() - enter" );
    
    iInfo.iUid=KAccFwUiDialogNotifierUid;
    iInfo.iChannel=KScreenOutputChannel;
    iInfo.iPriority=ENotifierPriorityHigh;
    
    API_TRACE_( "[AccFW: ACCFWUINOTIFIER] CAccFwUiDialogNotifierStub::CAccFwUiDialogNotifier::RegisterL() - return" );
    return iInfo;
    }

// -----------------------------------------------------------------------------
// CAccFwUiDialogNotifierStub::Info()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
CAccFwUiDialogNotifierStub::TNotifierInfo CAccFwUiDialogNotifierStub::Info() const
    {
    API_TRACE_( "[AccFW: ACCFWUINOTIFIER] CAccFwUiDialogNotifierStub::Info() - enter" );
    API_TRACE_( "[AccFW: ACCFWUINOTIFIER] CAccFwUiDialogNotifierStub::Info() - return void" );
    return iInfo;
    }

// -----------------------------------------------------------------------------
// CAccFwUiDialogNotifierStub::StartL()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
TPtrC8 CAccFwUiDialogNotifierStub::StartL( const TDesC8& /*aBuffer*/ )
    {
    API_TRACE_( "[AccFW: ACCFWUINOTIFIER] CAccFwUiDialogNotifierStub::StartL() - enter" );
    API_TRACE_( "[AccFW: ACCFWUINOTIFIER] CAccFwUiDialogNotifierStub::StartL() - return void" );
    return KNullDesC8();
    }
    
// -----------------------------------------------------------------------------
// CAccFwUiDialogNotifierStub::StartL()
// (other items were commented in a header).
// -----------------------------------------------------------------------------  
void CAccFwUiDialogNotifierStub::SetObserverSelection( RArray<TInt> aSelection, TBool aCancelPressed)
{
	iSelections = aSelection;
	iIsCancelled = aCancelPressed;
}
    

// -----------------------------------------------------------------------------
// CAccFwUiDialogNotifierStub::StartL()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CAccFwUiDialogNotifierStub::StartL(
    const TDesC8& /*aBuffer*/, 
    TInt aReplySlot, 
    const RMessagePtr2& aMessage)
    {
    API_TRACE_( "[AccFW: ACCFWUINOTIFIER] CAccFwUiDialogNotifierStub::StartL() - enter" );
    
    TInt selVal(0);
    iCancel = EFalse;
    iMessage = aMessage;
    iReplySlot = aReplySlot;
    
	TInt retval(KErrNone);	
    RProperty prop;
	retval = prop.Get( KTFAccessoryTestProperty, KTFAccessoryNotifierMethod, selVal );	
	User::LeaveIfError( retval );			
	
	if( selVal )
	{
		TPckg<TInt> valuePck( selVal );

		//complete client requests
		aMessage.Write( aReplySlot, valuePck, 0 );	
	 	aMessage.Complete( KErrNone );	
	}
	else
	{
		iCancel = ETrue;
		//complete client requests
		//TPckg<TInt> valuePck( 0 );
		//aMessage.Write( aReplySlot, valuePck, 0 );	
	 	//aMessage.Complete( KErrNone );	
	}
    
    API_TRACE_( "[AccFW: ACCFWUINOTIFIER] CAccFwUiDialogNotifierStub::StartL() - return void" );
    }
    
    


// -----------------------------------------------------------------------------
// CAccFwUiDialogNotifierStub::DoCancel()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CAccFwUiDialogNotifierStub::DoCancel()
{
}
    

// -----------------------------------------------------------------------------
// CAccFwUiDialogNotifierStub::Cancel()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
void CAccFwUiDialogNotifierStub::Cancel()
    {
    API_TRACE_( "[AccFW: ACCFWUINOTIFIER] CAccFwUiDialogNotifierStub::Cancel() - enter" );    
    TBool booli = iMessage.IsNull();
    
    
    if( iCancel )
    {
    	TPckg<TInt> valuePck( 0 );
    	iMessage.Write( iReplySlot, valuePck, 0 );	
		iMessage.Complete( KErrCancel );
		iCancel = EFalse;	 
    }
    
    
    API_TRACE_( "[AccFW: ACCFWUINOTIFIER] CAccFwUiDialogNotifierStub::Cancel() - return void" );
    }

// -----------------------------------------------------------------------------
// CAccFwUiDialogNotifierStub::UpdateL()
// (other items were commented in a header).
// -----------------------------------------------------------------------------
TPtrC8 CAccFwUiDialogNotifierStub::UpdateL(
    const TDesC8& /*aBuffer*/ )
    {
    API_TRACE_( "[AccFW: ACCFWUINOTIFIER] CAccFwUiDialogNotifierStub::UpdateL() - enter" );
    API_TRACE_( "[AccFW: ACCFWUINOTIFIER] CAccFwUiDialogNotifierStub::UpdateL() - return void" );
    
    return KNullDesC8();
    }

//  End of File
