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
* Description:  This is SUPL PSY location request manager
*
*/



// INCLUDE FILES
#include <e32std.h>
#include <bacntf.h>
#include <lbs.h>      
#include "suplpsyrequestmanager.h"
#include "suplpsyrequestor.h"
#include "suplpsyrequestcompletelistener.h"
#include "suplpsylogging.h"
#include "suplpsypanic.h"
#include "suplpsyinfostoremanager.h"


// LOCAL CONSTANTS AND MACROS

// =========================== == LOCAL FUNCTIONS ===============================

// ========================== == MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSuplPsyRequestManager::CSuplPsyRequestManager
// C++default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSuplPsyRequestManager::CSuplPsyRequestManager(): iSuplPsyCellIdHandler( NULL ),
                                                  iSuplPsyInfoStoreManager( NULL ),
                                                  iMaxAge( 0 )
    {
    // Nothing to do here
    }


// -----------------------------------------------------------------------------
// CSuplPsyRequestManager::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CSuplPsyRequestManager::ConstructL()
    {
    TRACESTRING( "CSuplPsyRequestManager::ConstructL start" )

    //Construct requestor
    iRequestor = CSuplPsyRequestor::NewL( 
        TCallBack( LocationRequestCompleteStatic, this ) );

    //Construct environment change notifier
    iEnvChangeNotifier = 
        CEnvironmentChangeNotifier::NewL( 
            CActive::EPriorityStandard, 
            TCallBack( NotifierCallBack, this ) );
    iSuplPsyCellIdHandler = CSuplPsyCellIdHandler::NewL( *this );
    
    iSuplPsyInfoStoreManager = CSuplPsyInfoStoreManager::NewL();
    
    iEnvChangeNotifier->Start();
    TRACESTRING( "CSuplPsyRequestManager::ConstructL end" )
    }


// -----------------------------------------------------------------------------
// CSuplPsyRequestManager::RegisterL
// -----------------------------------------------------------------------------
//
CSuplPsyRequestManager* CSuplPsyRequestManager::RegisterL( 
        MSuplPsyRequestCompleteListener& aPositioner )
    {
    CSuplPsyRequestManager* self = reinterpret_cast < CSuplPsyRequestManager* >
        ( Dll::Tls() );

    if ( self == NULL )
        {
        self = new( ELeave ) CSuplPsyRequestManager;
    
        CleanupStack::PushL( self );
        self->ConstructL();
        CleanupStack::Pop( self );

        Dll::SetTls( self );
        }

    self->RegisterPositionerL( aPositioner );
    return self;
    }

    
// -----------------------------------------------------------------------------
// CSuplPsyRequestManager::~CSuplPsyRequestManager
// Destructor
// -----------------------------------------------------------------------------
//
CSuplPsyRequestManager::~CSuplPsyRequestManager()
    {
    delete iRequestor;
    
    iPositionerArray.Reset();
    iPositionerArray.Close();

    // Delete environment change notifier
    if ( iEnvChangeNotifier != NULL )
        {
        iEnvChangeNotifier->Cancel();
        delete iEnvChangeNotifier;
        }

    delete iSuplPsyCellIdHandler;
    iSuplPsyCellIdHandler = NULL;
    
    delete iSuplPsyInfoStoreManager;
    iSuplPsyInfoStoreManager = NULL;
    TRACESTRING( "CSuplPsyRequestManager:: destructed" )
    }


// -----------------------------------------------------------------------------
// CSuplPsyRequestManager::NotifyPositionUpdate
// 
// -----------------------------------------------------------------------------
//
void CSuplPsyRequestManager::NotifyPositionUpdate ( 
        MSuplPsyRequestCompleteListener& aPositioner )
    {
    TInt index = FindPositioner( aPositioner );
    if ( index != KErrNotFound )
        {
        iPositionerArray[index].iRequested = ETrue;
        // Issue request to get current cell id info
        iSuplPsyCellIdHandler->GetCellID( iCellIdInfo );
        }
    }


// -----------------------------------------------------------------------------
// CSuplPsyRequestManager::CancelNotifyPositionUpdate
// 
// -----------------------------------------------------------------------------
//
void CSuplPsyRequestManager::CancelNotifyPositionUpdate ( 
        MSuplPsyRequestCompleteListener& aPositioner )
    {
    TInt index = FindPositioner( aPositioner );
    if ( index != KErrNotFound )
        {
        iPositionerArray[index].iRequested = EFalse;

        if ( !IsActiveLocationRequest() )
            {
            iRequestor->Cancel();
            }
        }
    }



// -----------------------------------------------------------------------------
// CSuplPsyRequestManager::RegisterPositionerL
// -----------------------------------------------------------------------------
//
void CSuplPsyRequestManager::RegisterPositionerL( 
        MSuplPsyRequestCompleteListener& aPositioner )
    {
    //Panic if the positiner has already registered
    if ( FindPositioner( aPositioner ) != KErrNotFound )
        {
        Panic( EPanicPositionerAlreadyRegistered );
        }

    TPositionerStatus status;
    status.iPositioner = &aPositioner;
    status.iRequested = EFalse;
    User::LeaveIfError( iPositionerArray.Append( status ) );
    }


// -----------------------------------------------------------------------------
// CSuplPsyRequestManager::FindPositioner
// -----------------------------------------------------------------------------
//
TInt CSuplPsyRequestManager::FindPositioner( 
        MSuplPsyRequestCompleteListener& aPositioner ) const
    {
    TInt count = iPositionerArray.Count();
    for ( TInt i = 0; i < count; i++ )
        {
        if ( iPositionerArray[i].iPositioner == &aPositioner )
            {
            return i;
            }
        }
    return KErrNotFound;
    }

// -----------------------------------------------------------------------------
// CSuplPsyRequestManager::Unregister
// -----------------------------------------------------------------------------
//
void CSuplPsyRequestManager::Unregister( 
        MSuplPsyRequestCompleteListener& aPositioner )
    {
    TInt index = FindPositioner( aPositioner );
    if ( index != KErrNotFound )
        {
        iPositionerArray.Remove( index );
        }

    //Delete this instance if no positioner is registered
    if ( iPositionerArray.Count() == 0 )
        {
        delete this;
        Dll::SetTls( NULL );
        }
    }

// -----------------------------------------------------------------------------
// CSuplPsyRequestManager::IsActiveLocationRequest
// -----------------------------------------------------------------------------
//
TBool CSuplPsyRequestManager::IsActiveLocationRequest() const
    {
    TInt count = iPositionerArray.Count();
    for ( TInt i = 0; i < count; i++ )
        {
        if ( iPositionerArray[i].iRequested )
            {
            return ETrue;
            }
        }
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CSuplPsyRequestManager::LocationRequestComplete
// -----------------------------------------------------------------------------
//
void CSuplPsyRequestManager::LocationRequestComplete()
    {
    // Update the Cell id information from SUPL FW
    TCellIdInfo cellinfofw;
    
    TInt errCIdfw = iSuplPsyInfoStoreManager->ReadSuplFwCellIdInfo( cellinfofw );
            
    if( KErrNone == errCIdfw )
	    {
	    // Log supl fw cid info
    	TRACESTRING2("supl fw Cid=%d",cellinfofw.iCid )
    	TRACESTRING2("supl fw Lac=%d",cellinfofw.iLac )
    	TRACESTRING2("supl fw MCC=%d",cellinfofw.iMCC )
    	TRACESTRING2("supl fw MNC=%d",cellinfofw.iMNC )	
	    TInt er = iSuplPsyInfoStoreManager->WriteSuplPsyCellIdInfo( cellinfofw );//Ignore the error
	    TRACESTRING2("cenrep write error=%d",er )
	    }
    
    
    // Update the location information
    iSuplPsyInfoStoreManager->WriteSuplPsyLocationInfo( GetPosition() );//Ignore the error 
    TInt err = iRequestor->GetRequestResult();
        
    //Complete all requests
    TInt count = iPositionerArray.Count();
    for ( TInt i = 0; i < count; i++ )
        {
        TPositionerStatus& positionerStatus = iPositionerArray[i];
        if ( positionerStatus.iRequested )
            {
            TBool completed = positionerStatus.iPositioner->RequestComplete( 
                err,
                iRequestor->GetLocation() );
                
            if ( completed )
                {
                positionerStatus.iRequested = EFalse;
                }
            }
        }
    }
        
// -----------------------------------------------------------------------------
// CSuplPsyRequestManager::LocationRequestCompleteStatic
// -----------------------------------------------------------------------------
//
TInt CSuplPsyRequestManager::LocationRequestCompleteStatic( TAny* aAny )
    {
    reinterpret_cast < CSuplPsyRequestManager* >( aAny )->LocationRequestComplete();
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CSuplPsyRequestManager::IsLastPostionValid
// -----------------------------------------------------------------------------
//
TBool CSuplPsyRequestManager::IsLastPostionValid( TTime aMaxAge ) const
    {
    TRACESTRING( "CSuplPsyRequestManager:: IsLastPostionValid" )
    TPosition position;
    iRequestor->GetLocation().GetPosition( position );

    return ( aMaxAge > 0 && position.Time() >= aMaxAge );
    }
    
// -----------------------------------------------------------------------------
// CSuplPsyRequestManager::GetPosition
// -----------------------------------------------------------------------------
const HPositionGenericInfo& CSuplPsyRequestManager::GetPosition()
    {
    return iRequestor->GetLocation();
    }
    

// -----------------------------------------------------------------------------
// CSuplPsyRequestManager::NotifierCallBack
// -----------------------------------------------------------------------------
TInt CSuplPsyRequestManager::NotifierCallBack( TAny* aPtr )
    {
    reinterpret_cast < CSuplPsyRequestManager* >( aPtr )->HandleEnvironmentChange();
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CSuplPsyRequestManager:HandleEnvironmentChange
// 
// Currently we are only interested of system time change. This function is also
// called once immediately after enviroment change notifier object is
// constructed.
//
// NOTE: This function can be used to detect changes in phone power supply
// in case this information is needed to change the functionality depending of
// whether external power supply is connected or not.
//
// ( other items were commented in a header ).
// -----------------------------------------------------------------------------
//
void CSuplPsyRequestManager::HandleEnvironmentChange()
    {
    // Get reason why this function was called
    TInt envChange = iEnvChangeNotifier->Change();

    // If reason was cancel, or system time change flag is not on, then we
    // can return.
     if ( envChange == KErrCancel || !( envChange & EChangesSystemTime ) )
        {
        return;
        }

    // If system time really changed, then we set last fix time as 0
    iRequestor->ResetLastPosition();
    }

// -----------------------------------------------------------------------------
// CSuplPsyRequestManager::BasicCellIDRequestCompletedL
// 
// -----------------------------------------------------------------------------
//
void CSuplPsyRequestManager::BasicCellIDRequestCompletedL( TInt aErrorCode )
    {
    TRACESTRING( "CSuplPsyRequestManager::BasicCellIDRequestCompletedL start" )
    
    TRACESTRING2("Request Completed with Error Code: %d",aErrorCode )
        
    TBool requestSuplFw = ETrue;    
        
    if( KErrNone == aErrorCode )
	    {
        // Log current cell id info
        TRACESTRING2("Current Cid=%d",iCellIdInfo.iCid )
        TRACESTRING2("Current Lac=%d",iCellIdInfo.iLac )
        TRACESTRING2("Current MCC=%d",iCellIdInfo.iMCC )
        TRACESTRING2("Current MNC=%d",iCellIdInfo.iMNC )
             
	    // Get stored cell id from cenrep
	    	   
	    CSuplPsyCellIdHandler::TNetworkTypeInfo type = iSuplPsyCellIdHandler->NetworkType();
	    TCellIdInfo cellinfo;
	    TInt errCId = iSuplPsyInfoStoreManager->ReadSuplPsyCellIdInfo( cellinfo );
	       
	        
	    HPositionGenericInfo* posinfo =  HPositionGenericInfo::NewLC();
	    TInt errPosInfo = iSuplPsyInfoStoreManager->ReadSuplPsyLocationInfo( *posinfo );
	    	    
	    // Validity of cell id info
        TRACESTRING2("Validity of cell id info=%d",errCId )
        
        // Presence of saved location info
        TRACESTRING2("Saved Location info is present=%d",errPosInfo )
	    
	    if( KErrNone == errCId && KErrNone == errPosInfo)
		    {
		    TRACESTRING( "Valid Cell Id & Location stored info exists" )
		    
		    // Log saved cell id info
	    	TRACESTRING2("Saved Cid=%d",cellinfo.iCid )
        	TRACESTRING2("Saved Lac=%d",cellinfo.iLac )
        	TRACESTRING2("Saved MCC=%d",cellinfo.iMCC )
        	TRACESTRING2("Saved MNC=%d",cellinfo.iMNC )
		    
		    TPosition pos;
	        posinfo->GetPosition( pos );
	    
	        TRACESTRING2( "Max Age=%d",iMaxAge.Int64() )
	        TRACESTRING2( "Fix Time=%d",pos.Time().Int64() )
	        
	        // Check if max age expires
	        if( iMaxAge > 0 && pos.Time() >= iMaxAge )
		    	{
		        TRACESTRING( "Max age not expired" )
		        // Check if current cell id is same as stored one.
		        if( iCellIdInfo.iCid == cellinfo.iCid && 
		            ( iCellIdInfo.iLac == cellinfo.iLac || 
		            CSuplPsyCellIdHandler:: EWCDMA == iSuplPsyCellIdHandler->NetworkType() ) &&
		            iCellIdInfo.iMCC == cellinfo.iMCC &&
		            iCellIdInfo.iMNC == cellinfo.iMNC )
					{
			        TRACESTRING( "Current CID info is same as stored one" )
			        // completes client's request with stored position info
			        TInt count = iPositionerArray.Count();
                    for ( TInt i = 0; i < count; i++ )
                		{
                	    TPositionerStatus& positionerStatus = iPositionerArray[i];
                 	    if ( positionerStatus.iRequested )
                 			{
                  		    TBool completed = positionerStatus.iPositioner->RequestComplete( 
                							  aErrorCode,*posinfo );
                   			if ( completed )
		                		{
		                		positionerStatus.iRequested = EFalse;
		                		}
            				}	
			    		}
		    	    TRACESTRING( "Clients request completed with stored location info" )
		    	    requestSuplFw = EFalse;
		    		}
				}
		    }
	    CleanupStack::PopAndDestroy( posinfo );
	    }
    
    if( requestSuplFw )
	    {
	    TRACESTRING( "Issuing Location Request..." )
	    //Issue location request to SUPL FW
	    iRequestor->RequestLocation();	
	    }
    TRACESTRING( "CSuplPsyRequestManager::BasicCellIDRequestCompletedL end" )
    }
    
// -----------------------------------------------------------------------------
// CSuplPsyRequestManager::RecieveMaxAge
// 
// -----------------------------------------------------------------------------
//
void CSuplPsyRequestManager::RecieveMaxAge( TTime aMaxAge )
	{
	iMaxAge = aMaxAge;	
	}
//  End of File  
