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
* Description:  Implementation of CCDSYEventManager class
*
*/




// INCLUDE FILES 
#include <doseventmanager.h>
#include "CDSYEventManager.h"
#include "CDSYMessage.h"
#include "CDSYController.h"
#include "CDSYMessageIDs.h"
#include "CDSYMessageStructures.h"
#include "CDSYFactory.h"
#include "commondsy_debug.h"



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
// CCDSYEventManager::CCDSYEventManager
// -----------------------------------------------------------------------------
CCDSYEventManager::CCDSYEventManager( CCDSYController& aCDSYController, CCDSYFactory& aCDSYFactory )
: iCDSYController( aCDSYController )
, iCDSYFactory( aCDSYFactory )
    {
    COM_TRACE_2( "CDSY - CCDSYEventManager::CCDSYEventManager(0x%x, 0x%x)", &aCDSYController, &aCDSYFactory );

    COM_TRACE_( "CDSY - CCDSYEventManager::CCDSYEventManager - return void" );
    }



// -----------------------------------------------------------------------------
// CCDSYEventManager::~CCDSYEventManager
// -----------------------------------------------------------------------------
CCDSYEventManager::~CCDSYEventManager()
    {
    COM_TRACE_( "CDSY - CCDSYEventManager::~CCDSYEventManager()" );
    iCDSYController.UnregisterServiceLogic( KCDSYMsgGroup, KCDSYEventGeneral, this );
    COM_TRACE_( "CDSY - CCDSYEventManager::~CCDSYEventManager - return void" );
    }

// -----------------------------------------------------------------------------
// CCDSYEventManager::ConstructL
// -----------------------------------------------------------------------------
void CCDSYEventManager::ConstructL()
    {
    COM_TRACE_( "CDSY - CCDSYEventManager::ConstructL()" );
    iCDSYController.RegisterServiceLogicL( KCDSYMsgGroup, KCDSYEventGeneral, this );
    COM_TRACE_( "CDSY - CCDSYEventManager::ConstructL - return void" );
    }

// -----------------------------------------------------------------------------
// CCDSYEventManager::NewL
// -----------------------------------------------------------------------------
CCDSYEventManager* CCDSYEventManager::NewL( CCDSYController& aCDSYController, CCDSYFactory& aCDSYFactory )
    {
    COM_TRACE_2( "CDSY - CCDSYEventManager::NewL(0x%x, 0x%x)", &aCDSYController, &aCDSYFactory );

    CCDSYEventManager* dsyEventManager = new ( ELeave ) CCDSYEventManager( aCDSYController, aCDSYFactory );
    CleanupStack::PushL( dsyEventManager );
    dsyEventManager->ConstructL();
    CleanupStack::Pop( dsyEventManager );
        
    COM_TRACE_1( "CDSY - CCDSYEventManager::NewL - return 0x%x", dsyEventManager );
    return dsyEventManager;
    }

// -----------------------------------------------------------------------------
// CCDSYEventManager::ProcessMessageL
// -----------------------------------------------------------------------------
void CCDSYEventManager::ProcessMessageL( TCDSYMessage& aCDSYMessage )
    {
    COM_TRACE_1( "CDSY - CCDSYEventManager::ProcessMessageL(0x%x)", &aCDSYMessage );

    TRACE_ASSERT( aCDSYMessage.Group() == KCDSYMsgGroup );
    TRACE_ASSERT( iCDSYFactory.EventManager() != NULL );

    if ( iCDSYFactory.EventManager() != NULL )
        {
        switch ( aCDSYMessage.ID() )
            {
            case KCDSYEventGeneral:

                TRACE_ASSERT( aCDSYMessage.Data()->Length() == sizeof ( TCDSYMsgDataGeneralEvent ) );
                API_TRACE_3( "CDSY - CCDSYEventManager::ProcessMessageL - DSY event fired:GeneralEvent(0x%x, 0x%x, 0x%x)", ( ( ( TCDSYMsgDataGeneralEvent* )( aCDSYMessage.Data()->Ptr() ) )->iEvent ),
                                                                                                                        ( ( TAny* )( ( TCDSYMsgDataGeneralEvent* )( aCDSYMessage.Data() )->Ptr() )->iParameter ),
                                                                                                                        ( ( ( TCDSYMsgDataGeneralEvent* )( aCDSYMessage.Data()->Ptr() ) )->iParameterSize ) );
                TRACE_ASSERT_RETURN( iCDSYFactory.EventManager()->GeneralEvent( ( ( ( TCDSYMsgDataGeneralEvent* )( aCDSYMessage.Data()->Ptr() ) )->iEvent ),
                                                                        ( ( TAny* )( ( TCDSYMsgDataGeneralEvent* )( aCDSYMessage.Data() )->Ptr() )->iParameter ),
                                                                        ( ( ( TCDSYMsgDataGeneralEvent* )( aCDSYMessage.Data()->Ptr() ) )->iParameterSize ) ) );

            break;

            default:

                TRACE_ASSERT_ALWAYS;

            break;
            }
        }

    COM_TRACE_( "CDSY - CCDSYEventManager::ProcessMessageL - return void" );
    }
    
