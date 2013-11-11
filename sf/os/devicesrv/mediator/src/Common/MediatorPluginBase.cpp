/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Pure virtual base class implementation for Mediator Plugins.
*
*/


// INCLUDE FILES
#include <e32svr.h>
#include "MediatorPluginBase.h"
#include "Debug.h"

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CMediatorPluginBase::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CMediatorPluginBase::ConstructL( )
    {
    iIdle = CIdle::NewL( CActive::EPriorityIdle );
   	iIdle->Start( TCallBack( StartPlugin, this ) ); 
    }
    
// -----------------------------------------------------------------------------
// CMediatorPluginBase::StartPlugin
// 
// -----------------------------------------------------------------------------
//
TInt CMediatorPluginBase::StartPlugin( TAny* aThis )
    {
    CMediatorPluginBase* me = (CMediatorPluginBase*) aThis;
    TRAP_IGNORE( me->StartPluginL() );
    // If we return error, idle is started again
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CMediatorPluginBase::StartPluginL
// 
// -----------------------------------------------------------------------------
//
void CMediatorPluginBase::StartPluginL()
    {
    // Derived class StartL is called here.
    StartL();
    }

// -----------------------------------------------------------------------------
// CMediatorPluginBase::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CMediatorPluginBase* CMediatorPluginBase::NewL( TUid aMediatorImplementationUid )
    {  
    TRACE(Print( _L("[Mediator Server]\t CMediatorPluginBase::NewL(0x%x)" ), aMediatorImplementationUid ) );
         
    TAny* tmpEcom = REComSession::CreateImplementationL(
                           aMediatorImplementationUid,
                           _FOFF( CMediatorPluginBase, iDtor_ID_Key ) );

    CMediatorPluginBase* newPlugin = REINTERPRET_CAST(
                                        CMediatorPluginBase*,
                                        tmpEcom );
    CleanupStack::PushL ( newPlugin );
    newPlugin->ConstructL();
    CleanupStack::Pop( newPlugin );
    return newPlugin;
    }
    

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
EXPORT_C CMediatorPluginBase::~CMediatorPluginBase()
    {
    delete iIdle;
    REComSession::DestroyedImplementation( iDtor_ID_Key );
    }
    
//  End of File
