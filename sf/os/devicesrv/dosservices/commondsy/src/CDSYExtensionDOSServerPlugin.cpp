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
* Description:  Implementation of CCDSYExtensionDOSServerPlugin class
*
*/




// INCLUDE FILES 
#include "CDSYExtensionDOSServerPlugin.h"
#include "commondsy_debug.h"
#include "CDSYMessage.h"
#include "CDSYMessageStructures.h"
#include "CDSYMessageIDs.h"
#include "CDSYDOSServerRequestManager.h"



// EXTERNAL DATA STRUCTURES
// EXTERNAL FUNCTION PROTOTYPES  
// CONSTANTS
// MACROS
// LOCAL CONSTANTS AND MACROS
// MODULE DATA STRUCTURES
// LOCAL FUNCTION PROTOTYPES
// FORWARD DECLARATIONS



// ============================= LOCAL FUNCTIONS ===============================



// ============================ MEMBER FUNCTIONS ===============================



// -----------------------------------------------------------------------------
// CCDSYExtensionDOSServerPlugin::CCDSYExtensionDOSServerPlugin
// -----------------------------------------------------------------------------
CCDSYExtensionDOSServerPlugin::CCDSYExtensionDOSServerPlugin( CCDSYDOSServerRequestManager& aCDSYDOSServerRequestManager )
: iCDSYDOSServerRequestManager( aCDSYDOSServerRequestManager )
, iDosExtensionBaseDSY( NULL )
    {
    COM_TRACE_1( "CDSY - CCDSYExtensionDOSServerPlugin::CCDSYExtensionDOSServerPlugin(0x%x)", &aCDSYDOSServerRequestManager );

    COM_TRACE_( "CDSY - CCDSYExtensionDOSServerPlugin::CCDSYExtensionDOSServerPlugin - return void" );
    }



// -----------------------------------------------------------------------------
// CCDSYExtensionDOSServerPlugin::~CCDSYExtensionDOSServerPlugin
// -----------------------------------------------------------------------------
CCDSYExtensionDOSServerPlugin::~CCDSYExtensionDOSServerPlugin()
    {
    COM_TRACE_( "CDSY - CCDSYExtensionDOSServerPlugin::~CCDSYExtensionDOSServerPlugin()" );

    iDosExtensionBaseDSY = NULL;

    COM_TRACE_( "CDSY - CCDSYExtensionDOSServerPlugin::~CCDSYExtensionDOSServerPlugin - return void" );
    }



// -----------------------------------------------------------------------------
// CCDSYFactory::ConstructL
// -----------------------------------------------------------------------------
void CCDSYExtensionDOSServerPlugin::ConstructL()
    {
    COM_TRACE_( "CDSY - CCDSYExtensionDOSServerPlugin::ConstructL()" );
    
    TCDSYMessage dsyMessage( KCDSYMsgGroup, KCDSYMsgInitializeExtensionCommand, NULL );

    iCDSYDOSServerRequestManager.SendMessageL( dsyMessage );

    TRACE_ASSERT( dsyMessage.ReturnData() != NULL );
    TRACE_ASSERT( dsyMessage.ReturnData()->Length() == sizeof ( TCDSYMsgDataExtensionInitialization ) );

    const TCDSYMsgDataExtensionInitialization* returnValue = REINTERPRET_CAST( const TCDSYMsgDataExtensionInitialization*, dsyMessage.ReturnData()->Ptr() );

    TRACE_ASSERT( returnValue->iDosExtensionBaseDSY != NULL );

    iDosExtensionBaseDSY = returnValue->iDosExtensionBaseDSY;

    COM_TRACE_( "CDSY - CCDSYExtensionDOSServerPlugin::ConstructL - return void" );
    }



// -----------------------------------------------------------------------------
// CCDSYExtensionDOSServerPlugin::NewL
// -----------------------------------------------------------------------------
CCDSYExtensionDOSServerPlugin* CCDSYExtensionDOSServerPlugin::NewL( CCDSYDOSServerRequestManager& aCDSYDOSServerRequestManager )
    {
    COM_TRACE_1( "CDSY - CCDSYExtensionDOSServerPlugin::NewL(0x%x)", &aCDSYDOSServerRequestManager );
    
    CCDSYExtensionDOSServerPlugin* extensionDOSServerPlugin = new ( ELeave ) CCDSYExtensionDOSServerPlugin( aCDSYDOSServerRequestManager );
    CleanupClosePushL( *extensionDOSServerPlugin );
    extensionDOSServerPlugin->ConstructL();
    CleanupStack::Pop( extensionDOSServerPlugin );

    COM_TRACE_1( "CDSY - CCDSYExtensionDOSServerPlugin::NewL - return 0x%x", extensionDOSServerPlugin );
    return extensionDOSServerPlugin;
    }



// -----------------------------------------------------------------------------
// CCDSYExtensionDOSServerPlugin::CallFunctionL
// -----------------------------------------------------------------------------
TInt CCDSYExtensionDOSServerPlugin::CallFunctionL( TInt aFunc, TAny* aParameter, TInt aParLength, TBool& aParameterModified )
    {
    API_TRACE_4( "CDSY - CCDSYExtensionDOSServerPlugin::CallFunctionL(0x%x, 0x%x, 0x%x, 0x%x)", aFunc, aParameter, aParLength, &aParameterModified );
    
    TRACE_ASSERT( iDosExtensionBaseDSY != NULL );

    if ( iDosExtensionBaseDSY == NULL )
        {
        User::Leave( KErrGeneral );
        }
    
    TInt err = iDosExtensionBaseDSY->CallFunctionL( aFunc, aParameter, aParLength, aParameterModified );

    API_TRACE_1( "CDSY - CCDSYExtensionDOSServerPlugin::CallFunctionL - return 0x%x", err );
    return err;
    }



// -----------------------------------------------------------------------------
// CCDSYExtensionDOSServerPlugin::CallFunctionAndCompleteL
// -----------------------------------------------------------------------------
void CCDSYExtensionDOSServerPlugin::CallFunctionAndCompleteL( TInt aFunc, TAny* aParameter, TInt aParLength, const RMessage2& aMessage )
    {
    API_TRACE_4( "CDSY - CCDSYExtensionDOSServerPlugin::CallFunctionAndCompleteL(0x%x, 0x%x, 0x%x, 0x%x)", aFunc, aParameter, aParLength, &aMessage );

    TRACE_ASSERT( iDosExtensionBaseDSY != NULL );

    if ( iDosExtensionBaseDSY == NULL )
        {
        User::Leave( KErrGeneral );
        }

    iDosExtensionBaseDSY->CallFunctionAndCompleteL( aFunc, aParameter, aParLength, aMessage );

    API_TRACE_( "CDSY - CCDSYExtensionDOSServerPlugin::CallFunctionAndCompleteL - return void" );
    }
    
