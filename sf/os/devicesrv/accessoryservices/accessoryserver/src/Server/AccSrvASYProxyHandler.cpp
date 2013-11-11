/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  ASY Proxy Handler
*
*/



// INCLUDE FILES
#include "AccSrvASYProxyHandler.h"
#include "acc_debug.h"
#include "AccSrvConnectionController.h"
#include "AccSrvServerModel.h"
#include "AccPolAccessoryPolicy.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS
const TInt KAccSrvModelTrIdMaxCount = 10000;

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CAccSrvASYProxyHandler::CAccSrvASYProxyHandler
// -----------------------------------------------------------------------------
//
CAccSrvASYProxyHandler::CAccSrvASYProxyHandler(
    CAccSrvConnectionController* aConnectionController ) :
    iConnectionController( aConnectionController ),
    iTransactionId( 0 )
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvASYProxyHandler::CAccSrvASYProxyHandler()" );
    }

// -----------------------------------------------------------------------------
// CAccSrvASYProxyHandler::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CAccSrvASYProxyHandler::ConstructL()
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvASYProxyHandler::ConstructL" );

    iOngoingTransactions.Reset();

    COM_TRACE_( "[AccFW:AccServer] CAccSrvASYProxyHandler::ConstructL - return void" );
    }

// -----------------------------------------------------------------------------
// CAccSrvASYProxyHandler::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CAccSrvASYProxyHandler* CAccSrvASYProxyHandler::NewL(
    CAccSrvConnectionController* aConnectionController )
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvASYProxyHandler::NewL()" );

    CAccSrvASYProxyHandler* self = new( ELeave ) CAccSrvASYProxyHandler( aConnectionController );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    COM_TRACE_( "[AccFW:AccServer] CAccSrvASYProxyHandler::NewL - return self" );
    return self;
    }


// Destructor
CAccSrvASYProxyHandler::~CAccSrvASYProxyHandler()
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvASYProxyHandler::~CAccSrvASYProxyHandler()" );

    iOngoingTransactions.Close();

    COM_TRACE_( "[AccFW:AccServer] CAccSrvASYProxyHandler::~CAccSrvASYProxyHandler - return" );
    }

// -----------------------------------------------------------------------------
// CAccSrvASYProxyHandler::HandleASYCommsL
// -----------------------------------------------------------------------------
//
TInt CAccSrvASYProxyHandler::HandleASYCommsL(
    TProcessCmdId aCmdId,
    TASYCommandParamRecord& aASYCommand,
    TPtr8* aPtrBuf )
    {
    COM_TRACE_1( "[AccFW:AccServer] CAccSrvASYProxyHandler::HandleASYCommsL(%d)", aCmdId );
    TInt trID( 0 );

    if ( ECmdProcessCommandInit == aCmdId )
        {
        //No need to store transaction, only a new transaction ID is generated
        trID = GenerateNewTransactionID();
        //Complete outstanding NotifyProcessCommand()
        iConnectionController->NotificationQueue().CompleteASYCmdL( ENotifyProcessCommand,
                                                                    aCmdId,
                                                                    trID,
                                                                    aASYCommand,
                                                                    aPtrBuf );
        }
    else
        {
        trID = StoreTransactionIDL( aCmdId, aASYCommand, aPtrBuf );
        }

    COM_TRACE_1( "[AccFW:AccServer] CAccSrvASYProxyHandler::HandleASYCommsL - return TrID=%d", trID );
    return trID;
    }

// -----------------------------------------------------------------------------
// CAccSrvASYProxyHandler::StoreTransactionIDL
// -----------------------------------------------------------------------------
//
TInt CAccSrvASYProxyHandler::StoreTransactionIDL(
    TProcessCmdId aCmdId,
    TASYCommandParamRecord& aASYCommand,
    TPtr8* aPtrBuf )
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvASYProxyHandler::StoreTransactionIDL()" );

    TInt trID( KErrNotFound );
    TInt count( iOngoingTransactions.Count() );
    TUint32 nameFromOngoingTransactions;
    TUint32 nameFromRequest;

    COM_TRACE_1( "[AccFW:AccServer] CAccSrvASYProxyHandler::StoreTransactionIDL - current count = %d", count );

    for ( TInt i( 0 ); i < count; ++i )
        {
        iOngoingTransactions.operator[](i).iNameRecord.GetName( nameFromOngoingTransactions );
        aASYCommand.iNameRecord.GetName( nameFromRequest );

        if ( nameFromOngoingTransactions == nameFromRequest && 
             iOngoingTransactions.operator[](i).iCommand == aCmdId && 
             iOngoingTransactions.operator[](i).iUniqueID == aASYCommand.iGenericID.UniqueID() )
            {
            //Same command is under processing in ASY -> use existing trid
            trID = iOngoingTransactions.operator[](i).iTransactionID;
            COM_TRACE_1( "[AccFW:AccServer] CAccSrvASYProxyHandler::StoreTransactionIDL - found trid = %d", trID );
            break;
            }
        else
            {
            //keep on searching
            }
        }

    if ( KErrNotFound == trID )
        {
        TAccSrvTrIDMatchingRecord trIDRecord;
        trID = GenerateNewTransactionID();
        trIDRecord.iUniqueID      = aASYCommand.iGenericID.UniqueID();
        trIDRecord.iNameRecord    = aASYCommand.iNameRecord;
        trIDRecord.iCommand       = aASYCommand.iCmdValue;
        trIDRecord.iTransactionID = trID;
        iOngoingTransactions.AppendL( trIDRecord );
        
        //Complete outstanding NotifyProcessCommand()
        TRAP_IGNORE( iConnectionController->NotificationQueue().CompleteASYCmdL( 
                                                                    ENotifyProcessCommand,
                                                                    aCmdId,
                                                                    trID,
                                                                    aASYCommand,
                                                                    aPtrBuf ) );
        }
    else
        {
        //No need to generate a new trid, existing is used.
        //This means also that ASY is not informed about this command.
        }

    COM_TRACE_1( "[AccFW:AccServer] CAccSrvASYProxyHandler::StoreTransactionIDL - return %d", trID );
    return trID;
    }

// -----------------------------------------------------------------------------
// CAccSrvASYProxyHandler::RemoveTransactionID
// -----------------------------------------------------------------------------
//
void CAccSrvASYProxyHandler::RemoveTransactionID( TInt aTransactionID )
    {
    COM_TRACE_1( "[AccFW:AccServer] CAccSrvASYProxyHandler::RemoveTransactionID(%d)", aTransactionID );

    TInt count( iOngoingTransactions.Count() );

    COM_TRACE_1( "[AccFW:AccServer] CAccSrvASYProxyHandler::RemoveTransactionID - current count = %d", count );

    for ( TInt i( 0 ); i < count; ++i )
        {
        if ( iOngoingTransactions.operator[]( i ).iTransactionID == aTransactionID )
            {
            iOngoingTransactions.Remove( i );
            COM_TRACE_1( "[AccFW:AccServer] CAccSrvASYProxyHandler::StoreTransactionIDL - removed index = %d",i );
            break;
            }
        else
            {
            // Keep on searching.
            // Not found is not error situation because
            // 1) One Transaction can handle several ASY commands i.e. it may be allready removed
            // 2) ECmdProcessCommandInit is not filtered
            }
        }

    COM_TRACE_( "[AccFW:AccServer] CAccSrvASYProxyHandler::RemoveTransactionID - return void" );
    }

// -----------------------------------------------------------------------------
// CAccSrvASYProxyHandler::GenerateNewTransactionID
//
// Increase id by one until max limit is reached.
// -----------------------------------------------------------------------------
//
TInt CAccSrvASYProxyHandler::GenerateNewTransactionID()
    {
    COM_TRACE_( "[AccFW:AccServer] CAccSrvASYProxyHandler::GenerateNewTransactionID()" );

    if ( iTransactionId <= KAccSrvModelTrIdMaxCount )
        {
        iTransactionId++;
        }
    else
        {
        //Zero is reserved for signalling no Transaction ID situation
        iTransactionId = 1;
        }

    COM_TRACE_1( "[AccFW:AccServer] CAccSrvASYProxyHandler::GenerateNewTransactionID - return 0x%x", iTransactionId );

    return iTransactionId;
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================

// End of File
