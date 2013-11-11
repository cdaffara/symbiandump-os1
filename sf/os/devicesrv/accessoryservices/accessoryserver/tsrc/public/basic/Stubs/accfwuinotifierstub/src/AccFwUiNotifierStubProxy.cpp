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
* Description:  Entry point to notifier server
*
*/


// INCLUDE FILES
#include <e32std.h>
#include <ecom/ImplementationProxy.h>

#include "AccFwUiNoteNotifierStub.h"
#include "AccFwUiDialogNotifierStub.h"
#include "acc_debug.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS

const TInt KNumberOfImplementations(2);

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================

// Creates notifiers on the array
void CreateNotifiersL( CArrayPtrFlat<MEikSrvNotifierBase2>* aSubjects )
    {
    
    API_TRACE_( "[AccFW: ACCFWUINOTIFIER] CreateNotifiersL() - enter" );
    MEikSrvNotifierBase2* note = CAccFwUiNoteNotifierStub::NewL();
    CleanupStack::PushL( note );
    
    MEikSrvNotifierBase2* dialog = CAccFwUiDialogNotifierStub::NewL();
    CleanupStack::PushL( dialog );        
    
    aSubjects->AppendL( note );
    aSubjects->AppendL( dialog );
    
    CleanupStack::Pop( dialog );
    CleanupStack::Pop( note );
    API_TRACE_( "[AccFW: ACCFWUINOTIFIER] CreateNotifiersL() - return void" );
    
    }
    
CArrayPtr<MEikSrvNotifierBase2>* NotifierArray()
    {
    CArrayPtrFlat<MEikSrvNotifierBase2>* subjects =
        new CArrayPtrFlat<MEikSrvNotifierBase2>(KNumberOfImplementations);
    
    API_TRACE_( "[AccFW: ACCFWUINOTIFIER] NotifierArray() - enter" );
    // if allocation succeeded
    if( subjects )
        {
        TRAPD( err, CreateNotifiersL( subjects ) );
        
        // release if creation failed
        if( err != KErrNone )
            {
            TInt count = subjects->Count();
            
            while( count-- )
                {
                // calling release on a MEikSrvNotifierBase2 will delete the instance
                (*subjects)[count]->Release();
                }
                
            delete subjects;
            subjects = NULL;
            }
        }        
    API_TRACE_( "[AccFW: ACCFWUINOTIFIER] NotifierArray() - return" );
    return subjects;
    }

const TImplementationProxy ImplementationTable[] =
	{
	{{0x10205060},  (TFuncPtr)NotifierArray}
	};

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(TInt& aTableCount)
	{
	aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy) ;
	return ImplementationTable;
	}
    
// ============================ MEMBER FUNCTIONS ===============================

//End of file
