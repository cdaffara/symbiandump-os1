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
* Description:  Implementation of observer class for remote control keys
*
*/


// INCLUDE FILES
#include <remconcoreapitarget.h>
#include <remconinterfaceselector.h>
#include <e32property.h>
#include <e32debug.h>
#include "audioclientstubremconobserver.h"
#include "accremconpstypes.h"


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CAudioClientStubRemConObserver::CAudioClientStubRemConObserver
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CAudioClientStubRemConObserver::CAudioClientStubRemConObserver( ) 
 : iInterfaceSelector( NULL ),
   iCoreTarget( NULL )
   
    {
    // Nothing
    }

// -----------------------------------------------------------------------------
// CAudioClientStubRemConObserver::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CAudioClientStubRemConObserver::ConstructL()
    {
   RDebug::Print( _L("CAudioClientStubRemConObserver::ConstructL") ) ;  
    
    iInterfaceSelector = CRemConInterfaceSelector::NewL();
    
    iCoreTarget = CRemConCoreApiTarget::NewL( *iInterfaceSelector, *this );
    
    iInterfaceSelector->OpenTargetL();
    }

// -----------------------------------------------------------------------------
// CAudioClientStubRemConObserver::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CAudioClientStubRemConObserver* CAudioClientStubRemConObserver::NewL()
    {   
    RDebug::Print( _L("CAudioClientStubRemConObserver::NewL") ) ;
    
    CAudioClientStubRemConObserver* self = new( ELeave ) CAudioClientStubRemConObserver();
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

// -----------------------------------------------------------------------------
// CAudioClientStubRemConObserver::~CSysApRemConObserver
// Destructor
// -----------------------------------------------------------------------------
//
CAudioClientStubRemConObserver::~CAudioClientStubRemConObserver()
    {
    RDebug::Print( _L("CAudioClientStubRemConObserver::~CAudioClientStubRemConObserver") ) ;
    delete iInterfaceSelector; // Internally deletes iCoreTarget 
    }
    
// -----------------------------------------------------------------------------
// CAudioClientStubRemConObserver::MrccatoCommand
// Handles remote key event
// -----------------------------------------------------------------------------
//
void CAudioClientStubRemConObserver::MrccatoCommand( TRemConCoreApiOperationId aOperationId, 
                                        TRemConCoreApiButtonAction aButtonAct )
	{
    RDebug::Print( _L("CAudioClientStubRemConObserver::MrccatoCommand: aOperationId=%d, aButtonAct=%d"), aOperationId, aButtonAct );
	
    TRemConClientKeyDataBuf keyDatabuf;     
    TRemConClientKeyData keyData;
    keyData.iOperationId = aOperationId;
    keyData.iButtonAct = aButtonAct;
    keyDatabuf = keyData;
    
    TInt err = RProperty::Set( KTFRemConTestProperty, KTFRemConClientKey, keyDatabuf );
    
    if ( err != KErrNone )
        {
        RDebug::Print( _L("CAudioClientStubRemConObserver::MrccatoCommand: err=%d"), err );
        }
	}

//  End of File 
