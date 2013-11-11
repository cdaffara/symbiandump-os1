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
* Description:  This is SUPL PSY Positioning Plug-in ( PSY )
*
*/



// INCLUDE FILES
#include <e32std.h>            
#include <centralrepository.h>
#include <ecom/implementationproxy.h>  // TImplementationProxy
#include <lbspositioninfo.h>      // TPositionInfoBase

#include "suplpsypositioner.h"
#include "suplpsycrkeys.h"
#include "suplpsyrequestmanager.h"
#include "suplpsygenericinfouser.h"
#include "suplpsylogging.h"
#include "suplpsypanic.h"
#include "suplpsy.hrh"


// LOCAL CONSTANTS AND MACROS
const TImplementationProxy KFactoryPtr =
    IMPLEMENTATION_PROXY_ENTRY( KSuplPsyImplUid, CSuplPsyPositioner::NewL );

// ========================= ==   == LOCAL FUNCTIONS ===============================

// Following function is required by Positioning Plug-in API at ordinal 1.

EXPORT_C const TImplementationProxy* ImplementationGroupProxy( 
    TInt& aTableCount )
    {
    aTableCount = 1;
    return &KFactoryPtr;
    }

// ======================== ==   == MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSuplPsyPositioner::CSuplPsyPositioner
// C++default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSuplPsyPositioner::CSuplPsyPositioner()
    {
    // Nothing to do here
    }


// -----------------------------------------------------------------------------
// CSuplPsyPositioner::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CSuplPsyPositioner::ConstructL( TAny* aConstructionParameters )
    {
    TRACESTRING( "CSuplPsyPositioner::ConstructL start" )
    // Calling BaseConstructL is required by Positioning Plug-in API.
    BaseConstructL( aConstructionParameters );

    // Get instance of request manager, and register to it
    iRequestManager = CSuplPsyRequestManager::RegisterL( *this );

    CRepository *repository = CRepository::NewL(KCRUidSuplPsy);

    TInt keyValue;

    TInt ret = repository->Get(KSuplPsyAllowedAccuracy, keyValue);
   
    if (ret == KErrNone)
        {            
        if (keyValue <= 0)
            {            
            TRACESTRING( "CSuplPsyPositioner::ConstructL, invalid value in the configuration " )
    	    iAllowedAccr = 0;
            }
        else		
    	    iAllowedAccr = keyValue;
        }
    else
    	iAllowedAccr = 0;

	
   	TRACESTRING2( "Allowes Accurracy...:%d", iAllowedAccr )
    delete repository;
    repository = NULL;
    TRACESTRING( "CSuplPsyPositioner::ConstructL end" )
    }


// -----------------------------------------------------------------------------
// CSuplPsyPositioner::NewL
// Two - phased constructor.
// -----------------------------------------------------------------------------
//
CSuplPsyPositioner* CSuplPsyPositioner::NewL( TAny* aConstructionParameters )
    {
    CSuplPsyPositioner* self = new( ELeave ) CSuplPsyPositioner;
    
    CleanupStack::PushL( self );
    self->ConstructL( aConstructionParameters );
    CleanupStack::Pop( self );

    return self;
    }

    
// -----------------------------------------------------------------------------
// CSuplPsyPositioner::~CSuplPsyPositioner
// Destructor
// -----------------------------------------------------------------------------
//
CSuplPsyPositioner::~CSuplPsyPositioner()
    {
    // Unregister to request manager
    if ( iRequestManager != NULL )
        {
        iRequestManager->Unregister( *this );
        }
    TRACESTRING( "CSuplPsyPositioner:: destructed" )
    }


// -----------------------------------------------------------------------------
// CSuplPsyPositioner::NotifyPositionUpdate
// 
// -----------------------------------------------------------------------------
//
void CSuplPsyPositioner::NotifyPositionUpdate ( 
    TPositionInfoBase& aPosInfo,  
    TRequestStatus&    aStatus )
    {
    TRACESTRING( "CSuplPsyPositioner::NotifyPositionUpdate start" )

    //Supl psy will not handle simultaneous location request
    if ( iRequestStatus != NULL )
        {
        Panic( EPanicPositionRequestOngoing );
        }
    
    aStatus = KRequestPending;    
        
    //Clear position info data
    if ( ClearPositionInfoData( aPosInfo ) != KErrNone )
        {
        TRequestStatus* status = &aStatus;
        User::RequestComplete( status, KErrArgument );
        }
    
    //Check maxAge
    TPosition lastPos;
    TTime maxAge;
    GetMaxAge( maxAge );
    iRequestManager->RecieveMaxAge( maxAge );
       
    //Issue location request to request manager
    iRequestManager->NotifyPositionUpdate( *this );
    iRequestStatus = &aStatus;
    iPositionInfo = &aPosInfo;
    TRACESTRING( "CSuplPsyPositioner::NotifyPositionUpdate end" )
    }


// -----------------------------------------------------------------------------
// CSuplPsyPositioner::CancelNotifyPositionUpdate
// 
// -----------------------------------------------------------------------------
//
void CSuplPsyPositioner::CancelNotifyPositionUpdate ()
    {
    TRACESTRING( "CSuplPsyPositioner::CancelNotifyPositionUpdate start" )
    if ( iRequestStatus != NULL )
        {
        iRequestManager->CancelNotifyPositionUpdate( *this );
        User::RequestComplete( iRequestStatus, KErrCancel );
        }
    TRACESTRING( "CSuplPsyPositioner::CancelNotifyPositionUpdate end" )
    }


// -----------------------------------------------------------------------------
// CSuplPsyPositioner::RequestComplete
// -----------------------------------------------------------------------------
//
TBool CSuplPsyPositioner::RequestComplete( 
            TInt aErr,
            const HPositionGenericInfo& aPosInfo )
    {
    TInt err = aErr;
    if ( err == KErrNone )
        {
        //Fill position info
        err = FillPositionInfoData( aPosInfo, *iPositionInfo );
        }

    //Complete the request with err code
    User::RequestComplete( iRequestStatus, err );
    return ETrue;
    }

// -----------------------------------------------------------------------------
// CSuplPsyPositioner::ClearPositionInfoData
// -----------------------------------------------------------------------------
//
TInt CSuplPsyPositioner::ClearPositionInfoData( 
            TPositionInfoBase& aPosInfo )
    {
    TInt err( KErrNone );
    TInt classType = aPosInfo.PositionClassType();
    if ( classType & EPositionSatelliteInfoClass )
        {
        err = KErrNotSupported;
        }
    else if ( classType & EPositionCourseInfoClass )
        {
        err = KErrNotSupported;
        }
    else if ( classType & EPositionGenericInfoClass )
        {
        // HPositionGenericInfo
        HPositionGenericInfo* genInfo =
            static_cast < HPositionGenericInfo*> ( &aPosInfo );
        
        genInfo->ClearPositionData();
        }
    else if ( classType & EPositionInfoClass )
        {
        // TPositionInfo
        ( void ) new ( &aPosInfo ) ( TPositionInfo );
        }
    else
        {
        // Unknown type, this should never happen
        // -->Panic if we get here
        Panic( EPanicUnknownPositioningClass );
        }

    aPosInfo.SetModuleId( ImplementationUid() );
    return err;
    }

// -----------------------------------------------------------------------------
// CSuplPsyPositioner::FillPositionInfoData
// -----------------------------------------------------------------------------
//
TInt CSuplPsyPositioner::FillPositionInfoData( 
            const HPositionGenericInfo& aSrcPosInfo,
            TPositionInfoBase& aDesPosInfo )
    {
    
    TInt err( KErrNone );
    TInt classType = aDesPosInfo.PositionClassType();
    if ( classType & EPositionSatelliteInfoClass )
        {
        err = KErrNotSupported;
        }
    else if ( classType & EPositionCourseInfoClass )
        {
        err = KErrNotSupported;
        }
    else if ( classType & EPositionGenericInfoClass )
        {
        // HPositionGenericInfo
        HPositionGenericInfo* genInfo =
            static_cast < HPositionGenericInfo*> ( &aDesPosInfo );
        
        if (iAllowedAccr > 0)
            {                
            
            TPosition position;
            aSrcPosInfo.GetPosition( position );
            TReal32 hAcc = position.HorizontalAccuracy();
            TInt acc = 	hAcc;
            TRACESTRING2( "EPositionGenericInfoClass : Received Accurracy...:%d",acc )
            
	            if (hAcc > 0 && hAcc > iAllowedAccr)
	            {
	            		TRACESTRING( "Returning Error...")
	                return KErrGeneral;
	            }    
            }
        err = SuplPsyGenericInfoUser::CopyHGenericInfo( aSrcPosInfo, *genInfo );
        }
    else if ( classType & EPositionInfoClass )
        {
        // TPositionInfo
        TPosition position;
        aSrcPosInfo.GetPosition( position );

        if (iAllowedAccr > 0)
            {                
            TReal32 hAcc = position.HorizontalAccuracy();
            TInt acc = 	hAcc;
            TRACESTRING2( "EPositionInfoClass : Received Accurracy...:%d",acc )
            
	            if (hAcc > 0 && hAcc > iAllowedAccr)
	            {
	            		TRACESTRING( "Returning Error...")
	                return KErrGeneral;
	            }    

            }

        ( reinterpret_cast < TPositionInfo&>( aDesPosInfo ) ).SetPosition( position );
        }
    else
        {
        // Unknown type, this should never happen
        // -->Panic if we get here
        Panic( EPanicUnknownPositioningClass );
        }

    return err;
    }


//  End of File  
