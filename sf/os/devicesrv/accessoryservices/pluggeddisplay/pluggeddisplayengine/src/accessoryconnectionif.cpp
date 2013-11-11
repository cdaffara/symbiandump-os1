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
* Description:  Accessory Connection Interface class for Composite Cable Status FSM.
*
*/


#include "accessoryconnectionif.h"
#include "multifinitestatemachine.h"
#include "pdeconstants.h"
#include "trace.h"
#include "compositecablestatusfsm.h"
#include <AccessoryServer.h>
#include <AccessoryConnection.h>
#include <AsyCmdTypes.h>
#include <AccPolCommonNameValuePairs.h>
#include <AccPolSubblockNameArray.h>
#include <e32cmn.h>

// ======== LOCAL FUNCTIONS ========


// ======== MEMBER FUNCTIONS ========


// ---------------------------------------------------------------------------
// Symbian two-phased constructor
// ---------------------------------------------------------------------------
//
CAccessoryConnectionIf* CAccessoryConnectionIf::NewL(
        MFSMForBody& aFSM,
        RAccessoryServer& aAccessoryServer )
    {
    FUNC_LOG;
    CAccessoryConnectionIf* self = new ( ELeave ) CAccessoryConnectionIf( aFSM );
    CleanupStack::PushL( self );
    self->ConstructL( aAccessoryServer );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CAccessoryConnectionIf::~CAccessoryConnectionIf()
    {
    FUNC_LOG;
    Cancel();
    iAccessoryConnection.CloseSubSession();
    delete iAccPolGenericIDPtr;
    }

// ----------------------------------------------------------------------------
// RunL
//
// ----------------------------------------------------------------------------
//
void CAccessoryConnectionIf::RunL()
    {
    FUNC_LOG;
    TInt retVal( iStatus.Int() );
    if ( retVal == KErrNone )  
        {
        TAccPolNameRecord nameRecord;
        CAccPolSubblockNameArray* nameArray = CAccPolSubblockNameArray::NewL();
        CleanupStack::PushL( nameArray );
        
        const TInt count = iGenericIDarray.Count();
        
        TBool accVideoOutCababiltyFound = EFalse;
        for ( TInt index = 0; index < count; index++ )
            {
            //Get all supported capabilities for the connected accessory.
            iAccessoryConnection.GetSubblockNameArrayL( 
                              iGenericIDarray.GetGenericIDL( index ), 
                              *nameArray );
            
            TAccPolGenericID genericID = iGenericIDarray.GetGenericIDL( index );
            TBool wiredPC = genericID.PhysicalConnectionCaps( KPCWired );
            TBool avDevice = genericID.DeviceTypeCaps( KDTAVDevice );
            TBool videoOut = nameArray->HasName( KAccVideoOut );            
            if( ( videoOut ) &&
                ( wiredPC ) &&
                ( avDevice ) )
                {
                nameRecord.SetName( KAccVideoOut );
                TAccValueTypeTInt value;
                if( iAccessoryConnection.GetValueL( genericID, nameRecord, value ) == KErrNone )
                    {
                    if( value.iValue == EAccVideoOutCVideo )
                        {
                        // Video/Composite cable is connected
                        accVideoOutCababiltyFound = ETrue;
                        TAccPolGenericID* genId = new (ELeave) TAccPolGenericID;
                        CleanupStack::PushL( genId );
                        *genId = genericID;
                        iAccPolGenericIDPtr = genId;
                        CleanupStack::Pop( genId );
                        }
                    }
                }
            }
            
            
        // renew request for listening cable events
        iAccessoryConnection.NotifyAccessoryConnectionStatusChanged ( iStatus, 
                                                             iGenericIDarray );
        SetActive();
        if ( accVideoOutCababiltyFound )
            {
            // Video/Composite cable is connected
            ((CCompositeCableStatusFSM*)&iFSM)->GetPDEngine()->Input( EPDEFSMIdCompositeCableStatus, EPDEIfAccessoryConnection, EPDEIfAccessoryConnectionEventCableConnected );
            }
        else
            {
            delete iAccPolGenericIDPtr;
            iAccPolGenericIDPtr = NULL;
            // Video/Composite cable is disconnected
            ((CCompositeCableStatusFSM*)&iFSM)->GetPDEngine()->Input( EPDEFSMIdCompositeCableStatus, EPDEIfAccessoryConnection, EPDEIfAccessoryConnectionEventCableDisconnected );
            }
        CleanupStack::PopAndDestroy( nameArray );
        }   
    }

// ----------------------------------------------------------------------------
// DoCancel
//
// ----------------------------------------------------------------------------
//
void CAccessoryConnectionIf::DoCancel()
    {
    FUNC_LOG;
    iAccessoryConnection.CancelGetAccessoryConnectionStatus();
    iAccessoryConnection.CancelNotifyAccessoryConnectionStatusChanged();
    }

// ----------------------------------------------------------------------------
// DoCancel
//
// ----------------------------------------------------------------------------
//
TInt CAccessoryConnectionIf::RunError( TInt aError )
    {
    FUNC_LOG;
    // Make use of aError argument top get rid of compiler warning.
    TInt error( aError );
    if ( KErrNone != error )
        {
        INFO_1("Accessory Status request failed with error code: %d", error );
        }
    INFO("Issuing new request for accessory status");
    iAccessoryConnection.NotifyAccessoryConnectionStatusChanged ( iStatus, 
                                                             iGenericIDarray );
    SetActive();
    
    return KErrNone;
    }

// ---------------------------------------------------------
// C++ constructor
// ---------------------------------------------------------
//
CAccessoryConnectionIf::CAccessoryConnectionIf(
        MFSMForBody& aFSM )
    : CActive( CActive::EPriorityLow ),
    iFSM( aFSM )
    {
    FUNC_LOG;
    }

// ---------------------------------------------------------------------------
// GetAccPolGenericID
// ---------------------------------------------------------------------------
//
TInt CAccessoryConnectionIf::GetAccPolGenericID( TAccPolGenericID& aAccPolGenericID )
    {
    FUNC_LOG;
    TInt retVal( KErrNone );
    if ( iAccPolGenericIDPtr )
        {
        aAccPolGenericID = *iAccPolGenericIDPtr;
        }
    else
        {
        retVal = KErrNotFound;
        }
    return retVal;
    }

// ---------------------------------------------------------
// ConstructL
// ---------------------------------------------------------
//
void CAccessoryConnectionIf::ConstructL(
        RAccessoryServer& aAccessoryServer  )
    {
    FUNC_LOG;
    CActiveScheduler::Add( this );
    // Create subsession.
    User::LeaveIfError( iAccessoryConnection.CreateSubSession ( aAccessoryServer ) );
    // Get asynchronously cable connection status
    iAccessoryConnection.GetAccessoryConnectionStatus ( 
                                          iStatus, iGenericIDarray );
    // Ready to run
    SetActive();

    }

// ======== GLOBAL FUNCTIONS ========

