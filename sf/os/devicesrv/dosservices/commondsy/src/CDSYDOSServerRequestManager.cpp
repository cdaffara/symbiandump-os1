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
* Description:  Implementation of CCDSYDOSServerRequestManager class
*
*/


// INCLUDE FILES 
#include "CDSYDOSServerRequestManager.h"
#include "CDSYMessageBufferItem.h"
#include "CDSYMessage.h"
#include "CDSYController.h"
#include "CDSYMessageIDs.h"
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
// CCDSYDOSServerRequestManager::CCDSYDOSServerRequestManager
// -----------------------------------------------------------------------------
CCDSYDOSServerRequestManager::CCDSYDOSServerRequestManager( CCDSYController& aCDSYController )
: iCDSYController( aCDSYController )
, iLDSYFind( ETrue )
    {
    COM_TRACE_1( "CDSY - CCDSYDOSServerRequestManager::CCDSYDOSServerRequestManager(0x%x)", &aCDSYController );

    COM_TRACE_( "CDSY - CCDSYDOSServerRequestManager::CCDSYDOSServerRequestManager - return void" );
    }



// -----------------------------------------------------------------------------
// CCDSYDOSServerRequestManager::~CCDSYDOSServerRequestManager
// -----------------------------------------------------------------------------
CCDSYDOSServerRequestManager::~CCDSYDOSServerRequestManager()
    {
    COM_TRACE_( "CDSY - CCDSYDOSServerRequestManager::~CCDSYDOSServerRequestManager()" );

#ifndef RD_STARTUP_CHANGE
    iCDSYController.UnregisterServiceLogic( KCDSYMsgGroup, KCDSYMsgGetStartupReasonReturn, this );
    iCDSYController.UnregisterServiceLogic( KCDSYMsgGroup, KCDSYMsgInitiateDSYStartupReturn, this );
    iCDSYController.UnregisterServiceLogic( KCDSYMsgGroup, KCDSYMsgExecuteSelfTestReturn, this );
    iCDSYController.UnregisterServiceLogic( KCDSYMsgGroup, KCDSYMsgGetSoftwareResetReasonReturn, this );
    iCDSYController.UnregisterServiceLogic( KCDSYMsgGroup, KCDSYMsgSetSoftwareResetReasonReturn, this );
    iCDSYController.UnregisterServiceLogic( KCDSYMsgGroup, KCDSYMsgIndicatePowerOnReturn, this );
    iCDSYController.UnregisterServiceLogic( KCDSYMsgGroup, KCDSYMsgSwitchOffPowerReturn, this );
    iCDSYController.UnregisterServiceLogic( KCDSYMsgGroup, KCDSYMsgGetSoftwareResetStatusReturn, this );
    iCDSYController.UnregisterServiceLogic( KCDSYMsgGroup, KCDSYMsgExecuteSoftwareResetReturn, this );
    iCDSYController.UnregisterServiceLogic( KCDSYMsgGroup, KCDSYMsgSendStartupSyncReturn, this );

    iCDSYController.UnregisterServiceLogic( KCDSYMsgGroup, KCDSYMsgExecuteDeepRfsReturn, this );
    iCDSYController.UnregisterServiceLogic( KCDSYMsgGroup, KCDSYMsgExecuteNormalRfsReturn, this );
    iCDSYController.UnregisterServiceLogic( KCDSYMsgGroup, KCDSYMsgCheckTimeValidityReturn, this );
    iCDSYController.UnregisterServiceLogic( KCDSYMsgGroup, KCDSYMsgSetWakeupAlarmReturn, this );
    iCDSYController.UnregisterServiceLogic( KCDSYMsgGroup, KCDSYMsgCancelWakeupAlarmReturn, this );
    iCDSYController.UnregisterServiceLogic( KCDSYMsgGroup, KCDSYMsgSendShutdownSyncReturn, this );
    iCDSYController.UnregisterServiceLogic( KCDSYMsgGroup, KCDSYMsgSetDomesticOSStatusReturn, this );
    iCDSYController.UnregisterServiceLogic( KCDSYMsgGroup, KCDSYMsgSetOfflineStatusReturn, this );
    iCDSYController.UnregisterServiceLogic( KCDSYMsgGroup, KCDSYMsgGetOfflineStatusReturn, this );
    iCDSYController.UnregisterServiceLogic( KCDSYMsgGroup, KCDSYMsgGetPreferredLanguageReturn, this );

    iCDSYController.UnregisterServiceLogic( KCDSYMsgGroup, KCDSYMsgGenerateGripEventReturn, this );
#endif //RD_STARTUP_CHANGE    
    iCDSYController.UnregisterServiceLogic( KCDSYMsgGroup, KCDSYMsgInitializeExtensionReturn, this );

    iMessages.ResetAndDestroy();
    
    COM_TRACE_( "CDSY - CCDSYDOSServerRequestManager::~CCDSYDOSServerRequestManager - return void" );
    }



// -----------------------------------------------------------------------------
// CCDSYDOSServerRequestManager::ConstructL
// -----------------------------------------------------------------------------
void CCDSYDOSServerRequestManager::ConstructL()
    {
    COM_TRACE_( "CDSY - CCDSYDOSServerRequestManager::ConstructL()" );

#ifndef RD_STARTUP_CHANGE
    iCDSYController.RegisterServiceLogicL( KCDSYMsgGroup, KCDSYMsgGetStartupReasonReturn, this );

    iCDSYController.RegisterServiceLogicL( KCDSYMsgGroup, KCDSYMsgInitiateDSYStartupReturn, this );
    iCDSYController.RegisterServiceLogicL( KCDSYMsgGroup, KCDSYMsgExecuteSelfTestReturn, this );
    iCDSYController.RegisterServiceLogicL( KCDSYMsgGroup, KCDSYMsgGetSoftwareResetReasonReturn, this );
    iCDSYController.RegisterServiceLogicL( KCDSYMsgGroup, KCDSYMsgSetSoftwareResetReasonReturn, this );
    iCDSYController.RegisterServiceLogicL( KCDSYMsgGroup, KCDSYMsgIndicatePowerOnReturn, this );
    iCDSYController.RegisterServiceLogicL( KCDSYMsgGroup, KCDSYMsgSwitchOffPowerReturn, this );
    iCDSYController.RegisterServiceLogicL( KCDSYMsgGroup, KCDSYMsgGetSoftwareResetStatusReturn, this );
    iCDSYController.RegisterServiceLogicL( KCDSYMsgGroup, KCDSYMsgExecuteSoftwareResetReturn, this );
    iCDSYController.RegisterServiceLogicL( KCDSYMsgGroup, KCDSYMsgSendStartupSyncReturn, this );

    iCDSYController.RegisterServiceLogicL( KCDSYMsgGroup, KCDSYMsgExecuteDeepRfsReturn, this );
    iCDSYController.RegisterServiceLogicL( KCDSYMsgGroup, KCDSYMsgExecuteNormalRfsReturn, this );
    iCDSYController.RegisterServiceLogicL( KCDSYMsgGroup, KCDSYMsgCheckTimeValidityReturn, this );
    iCDSYController.RegisterServiceLogicL( KCDSYMsgGroup, KCDSYMsgSetWakeupAlarmReturn, this );
    iCDSYController.RegisterServiceLogicL( KCDSYMsgGroup, KCDSYMsgCancelWakeupAlarmReturn, this );
    iCDSYController.RegisterServiceLogicL( KCDSYMsgGroup, KCDSYMsgSendShutdownSyncReturn, this );
    iCDSYController.RegisterServiceLogicL( KCDSYMsgGroup, KCDSYMsgSetDomesticOSStatusReturn, this );
    iCDSYController.RegisterServiceLogicL( KCDSYMsgGroup, KCDSYMsgSetOfflineStatusReturn, this );
    iCDSYController.RegisterServiceLogicL( KCDSYMsgGroup, KCDSYMsgGetOfflineStatusReturn, this );
    iCDSYController.RegisterServiceLogicL( KCDSYMsgGroup, KCDSYMsgGetPreferredLanguageReturn, this );
    
    iCDSYController.RegisterServiceLogicL( KCDSYMsgGroup, KCDSYMsgGenerateGripEventReturn, this );
        
#endif // RD_STARTUP_CHANGE
    iCDSYController.RegisterServiceLogicL( KCDSYMsgGroup, KCDSYMsgInitializeExtensionReturn, this );


    COM_TRACE_( "CDSY - CCDSYDOSServerRequestManager::ConstructL - return void" );
    }



// -----------------------------------------------------------------------------
// CCDSYDOSServerRequestManager::NewL
// -----------------------------------------------------------------------------
CCDSYDOSServerRequestManager* CCDSYDOSServerRequestManager::NewL( CCDSYController& aCDSYController )
    {
    COM_TRACE_1( "CDSY - CCDSYDOSServerRequestManager::NewL(0x%x)", &aCDSYController );

    CCDSYDOSServerRequestManager* requestManager = new ( ELeave ) CCDSYDOSServerRequestManager( aCDSYController );
    CleanupStack::PushL( requestManager );
    requestManager->ConstructL();
    CleanupStack::Pop( requestManager );

    COM_TRACE_1( "CDSY - CCDSYDOSServerRequestManager::NewL - return 0x%x", requestManager );
    return requestManager;
    }



// -----------------------------------------------------------------------------
// CCDSYDOSServerRequestManager::ProcessMessageL
// -----------------------------------------------------------------------------
void CCDSYDOSServerRequestManager::ProcessMessageL( TCDSYMessage& aCDSYMessage )
    {
    COM_TRACE_1( "CDSY - CCDSYDOSServerRequestManager::ProcessMessageL(0x%x)", &aCDSYMessage );

    TUint32 count = 0;
    TBool asyncMessage = EFalse;
    for ( TInt i = 0; i < iMessages.Count(); i++ )
        {
        // Check the transaction ID's
        if ( ( iMessages[i] )->TransactionID() == aCDSYMessage.TransactionID() )
            {
            // The asyncMessage check is performed before ProcessResponseL
            // is called because ProcessResponseL might clear the async flag
            asyncMessage = iMessages[i]->IsAsyncMessage();
            ( iMessages[i] )->ProcessResponseL( aCDSYMessage );
            count++;
            // Delete the message from the message list
            if ( asyncMessage )
                {
                delete iMessages[i];
                iMessages.Remove( i );
                i--;
                }
            }
        }

    TRACE_ASSERT( count == 1 );

    COM_TRACE_( "CDSY - CCDSYDOSServerRequestManager::ProcessMessageL - return void" );
    }



// -----------------------------------------------------------------------------
// CCDSYDOSServerRequestManager::SendMessageL
// -----------------------------------------------------------------------------
void CCDSYDOSServerRequestManager::SendMessageL( TCDSYMessage& aCDSYMessage )
    {
    COM_TRACE_1( "CDSY - CCDSYDOSServerRequestManager::SendMessageL(0x%x)", &aCDSYMessage );

    aCDSYMessage.SetTransactionID( iCDSYController.GetNewTransactionID() );
    CCDSYMessageBufferItem* messageBufferItem = CCDSYMessageBufferItem::NewL( iCDSYController, *this, aCDSYMessage );

    TUint32 count = 0;

#ifdef _DEBUG
    
    // Check that same transaction id is not in the buffer.
    for ( TInt j = 0; j < iMessages.Count(); j++ )
        {
        if ( ( iMessages[j] )->TransactionID() == aCDSYMessage.TransactionID() )
            {
            count++;
            TRACE_ASSERT( count == 0 );
            }
        }

#endif //#ifdef _DEBUG
    
    TInt err = iMessages.Append( messageBufferItem );
    if ( err != KErrNone )
        {
        TRACE_ASSERT_ALWAYS;
        delete messageBufferItem;
        User::Leave( err );
        }
    
    TRAP( err, messageBufferItem->SendMessageL() );

    count = 0;
    // Remove the message from the list.
    for ( TInt i = 0; i < iMessages.Count(); i++ )
        {
        if ( ( iMessages[i] )->TransactionID() == aCDSYMessage.TransactionID() )
            {
            delete iMessages[i];
            iMessages.Remove( i );
            i--;
            count++;
            }
        }
    TRACE_ASSERT( count <= 1 );

    User::LeaveIfError( err );

    COM_TRACE_( "CDSY - CCDSYDOSServerRequestManager::SendMessageL - return void" );
    }



// -----------------------------------------------------------------------------
// CCDSYDOSServerRequestManager::SendMessageL
// -----------------------------------------------------------------------------
void CCDSYDOSServerRequestManager::SendMessageL( TCDSYMessage& aCDSYMessage, RMessagePtr2 aMessage )
    {
    COM_TRACE_2( "CDSY - CCDSYDOSServerRequestManager::SendMessageL(0x%x, 0x%x)", &aCDSYMessage, &aMessage );

    aCDSYMessage.SetTransactionID( iCDSYController.GetNewTransactionID() );
    CCDSYMessageBufferItem* messageBufferItem = CCDSYMessageBufferItem::NewL( iCDSYController, *this, aCDSYMessage, aMessage );

    TUint32 count = 0;

#ifdef _DEBUG
    
    // Check that same transaction id is not in the buffer.
    for ( TInt j = 0; j < iMessages.Count(); j++ )
        {
        if ( ( iMessages[j] )->TransactionID() == aCDSYMessage.TransactionID() )
            {
            count++;
            TRACE_ASSERT( count == 0 );
            }
        }

#endif //#ifdef _DEBUG
    
    TInt err = iMessages.Append( messageBufferItem );
    if ( err != KErrNone )
        {
        TRACE_ASSERT_ALWAYS;
        delete messageBufferItem;
        User::Leave( err );
        }
    
    
    TRAP( err, messageBufferItem->SendMessageL() );

    if ( err != KErrNone )
        {
        // Remove the message from the list.
        count = 0;
        for ( TInt i = 0; i < iMessages.Count(); i++ )
            {
            if ( ( iMessages[i] )->TransactionID() == aCDSYMessage.TransactionID() )
                {
                delete iMessages[i];
                iMessages.Remove( i );
                i--;
                count++;
                }
            }
        TRACE_ASSERT( count <= 1 );
        }

    User::LeaveIfError( err );

    COM_TRACE_( "CDSY - CCDSYDOSServerRequestManager::SendMessageL - return void" );
    }



// -----------------------------------------------------------------------------
// CCDSYDOSServerRequestManager::Cancel
// -----------------------------------------------------------------------------
void CCDSYDOSServerRequestManager::Cancel( TUint32 aTransactionID, TInt aReason )
    {
    COM_TRACE_2( "CDSY - CCDSYDOSServerRequestManager::Cancel(0x%x, 0x%x)", aTransactionID, aReason );

    for ( TInt i = 0; i < iMessages.Count(); i++ )
        {
        if ( ( iMessages[i] )->TransactionID() == aTransactionID )
            {
            iMessages[i]->CancelAsyncCommand( aReason );
            delete iMessages[i];
            iMessages.Remove( i );
            i = iMessages.Count();
            }
        }
    
    COM_TRACE_( "CDSY - CCDSYDOSServerRequestManager::Cancel - return void" );
    }
    


// -----------------------------------------------------------------------------
// CCDSYDOSServerRequestManager::ExpireMessageL
// -----------------------------------------------------------------------------
void CCDSYDOSServerRequestManager::ExpireMessageL( TCDSYMessage& /*aCDSYMessage*/ )
    {
    }
    
