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
* Description:  This is SUPL PSY location requestor
*
*/



// INCLUDE FILES
#include <e32cmn.h>
#include <centralrepository.h>
#include <lbssuplpsyadaptation.h>
#include "suplpsyrequestor.h"
#include "suplpsygenericinfouser.h"
#include "suplpsylogging.h"
#include "suplpsypanic.h"
#include "suplpsy.hrh"

// ===================== ==   ==   ==   == AL FUNCTIONS ===============================


// ==================== ==   ==   ==   == BER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSuplPsyRequestor::CSuplPsyRequestor
// C++default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSuplPsyRequestor::CSuplPsyRequestor( const TCallBack& aObserver ) :
          iObserver( aObserver ),
          iFirstReq(ETrue),
          iPrevFixSuccess(EFalse),
          iRequestIdCount(0)
    {
    }


// -----------------------------------------------------------------------------
// CSuplPsyRequestor::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CSuplPsyRequestor::ConstructL()
    {
    TRACESTRING( "CSuplPsyRequestor::ConstructL start" )
    
    //Create PSY Adapataion object
    iPsyAdaptation = CLbsSuplPsyAdaptation::NewL(*this); 

    //Construct iPositionInfo;
    iPositionInfo = ConstructGenericInfoL();

    CRepository *repository = CRepository::NewL(KCRUidSuplPsy);

    //TInt keyValue;

    TInt ret = repository->Get(KPSYTimeToNextFix, iTtnf);
    
    if (KErrNone != repository->Get(KPSYPmUid, iPmUid))
        {
        // Use default UID (SUPL Proxy PM)
        iPmUid = 0x102871EC;
        }

    delete repository;
    repository = NULL;
    
    TRACESTRING( "CSuplPsyRequestor::ConstructL end" )
    }

// -----------------------------------------------------------------------------
// CSuplPsyRequestor::ConstructGenericInfoL
// -----------------------------------------------------------------------------
//
HPositionGenericInfo* CSuplPsyRequestor::ConstructGenericInfoL( 
    TInt aBufferSize ) const
    {
    TRACESTRING( "CSuplPsyRequestor::ConstructGenericInfoL start" )
    HPositionGenericInfo* positionInfo = HPositionGenericInfo::NewL( aBufferSize );
    
    //Set all requested fields
    TInt count = SuplPsyGenericInfoUser::SupportedFieldsCount();
    for ( TInt i = 0; i < count; i++ )
        {
        positionInfo->SetRequestedField( 
            SuplPsyGenericInfoUser::SupportedFieldAt( i ) );
        }
    TRACESTRING( "CSuplPsyRequestor::ConstructGenericInfoL end" )
    return positionInfo;
    }


// -----------------------------------------------------------------------------
// CSuplPsyRequestor::NewL
// Two - phased constructor.
// -----------------------------------------------------------------------------
//
CSuplPsyRequestor* CSuplPsyRequestor::NewL( 
        const TCallBack& aObserver )
    {
    CSuplPsyRequestor* self = new( ELeave ) CSuplPsyRequestor( aObserver );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }

    
// -----------------------------------------------------------------------------
// CSuplPsyRequestor::~CSuplPsyRequestor
// Destructor
// -----------------------------------------------------------------------------
//
CSuplPsyRequestor::~CSuplPsyRequestor()
    {
    //Cancel();
    delete iPositionInfo;
    iPositionInfo = NULL;
    delete iPsyAdaptation;
    iPsyAdaptation = NULL;
    }

// -----------------------------------------------------------------------------
// CSuplPsyRequestor::OnLocationUpdate
// -----------------------------------------------------------------------------
//
void CSuplPsyRequestor::OnLocationUpdate(TUint aRequestId, TPositionInfoBase& aPosition, TInt aReason)
    {
    TRACESTRING2( "CSuplPsyRequestor::OnLocationUpdate reason =%d", aReason )
    
    if (iRequestId != aRequestId)
        aReason = KErrGeneral;            

    iPosResult = aReason;            

    //Call callback function
    if ( aReason >= KErrNone )
        {
		//Get position info
		TRACESTRING( "Extracting position... " )
		HPositionGenericInfo* posInfo = static_cast < HPositionGenericInfo*> ( &aPosition );
										
	        //Set module Id
	        iPositionInfo->SetModuleId( TUid::Uid( KSuplPsyImplUid ) );
									        
	        //Set time
		TPosition pos;
		posInfo->GetPosition( pos );
		TTime now;
		now.UniversalTime();
		pos.SetTime( now );
		iPositionInfo->SetPosition( pos );
		iPrevFixSuccess = ETrue;

        }
	else
		{
		iPrevFixSuccess = EFalse;
		}
    iObserver.CallBack();
    TRACESTRING( "CSuplPsyRequestor::OnLocationUpdate end" )
    }

// -----------------------------------------------------------------------------
// CSuplPsyRequestor::RequestLocation
// -----------------------------------------------------------------------------
//
void CSuplPsyRequestor::RequestLocation()
    {
    TRACESTRING( "CSuplPsyRequestor::RequestLocation start" )
    
    // Check if the request is made withing TTNF,
    // if so return the previous computed position 
    // if it was success
                    
    if (!iFirstReq && iPrevFixSuccess)
         if (IsRequestInTtnf())
            {
            iObserver.CallBack();
            TRACESTRING( "Interval is within TTNF, returning. Returning Previous Position." )
            return;
            }            
        
        
	TRACESTRING( "Calling SUPL FW RunSession... " )
    //Make location request
    iRequestId = GetRequestId();

	const TUint KProtocolModuleUidValue = iPmUid;
    const TUid KProtocolModuleUid = { KProtocolModuleUidValue };
    iPsyAdaptation->RequestLocationUpdate(iRequestId, ETrue, KProtocolModuleUid); 
    if (iFirstReq)
        {            
        iFirstReq = EFalse;                
        }
    TRACESTRING( "CSuplPsyRequestor::RequestLocation end" )
    }

// -----------------------------------------------------------------------------
// CSuplPsyRequestor::GetRequestResult
// -----------------------------------------------------------------------------
//
TInt CSuplPsyRequestor::GetRequestResult()
    {
    TRACESTRING( "CSuplPsyRequestor::GetRequestResult " )
    return iPosResult;
    }

// -----------------------------------------------------------------------------
// CSuplPsyRequestor::GetLocation
// -----------------------------------------------------------------------------
//
const HPositionGenericInfo& CSuplPsyRequestor::GetLocation()
    {
    TRACESTRING( "CSuplPsyRequestor::GetLocation " )
    return *iPositionInfo;
    }

// -----------------------------------------------------------------------------
// CSuplPsyRequestor::ResetLastPosition
// -----------------------------------------------------------------------------
//
void CSuplPsyRequestor::ResetLastPosition()
    {
//    iPositionInfo->ClearPositionData();
    }

TBool CSuplPsyRequestor::IsRequestInTtnf()
    {
    TRACESTRING( "CSuplPsyRequestor::IsRequestInTtnf start" )
    TTime now;
    now.UniversalTime();

    TInt diff = 1000000;

    TPosition pos;

    iPositionInfo->GetPosition( pos );

    TTimeIntervalSeconds secs;

    now.SecondsFrom(pos.Time(), secs); 

    diff = diff * secs.Int();

	TRACESTRING2( "Time diff. for request...:%d", diff )
    if (diff <= iTtnf)
        return ETrue;            

    TRACESTRING( "CSuplPsyRequestor::IsRequestInTtnf end" )
    return EFalse;
    }        

void CSuplPsyRequestor::OnSessionComplete(TUint aRequestId, TInt aReason)
    {                
    if (iRequestId != aRequestId)
        aReason = KErrGeneral;            
    iPosResult = aReason;            
    iObserver.CallBack();
    }            

void CSuplPsyRequestor::Cancel()
    {
    TRACESTRING( "CSuplPsyRequestor::Cancel " )
    iPsyAdaptation->CancelLocationRequest(iRequestId, KErrCancel);            
    }            

TUint CSuplPsyRequestor::GetRequestId()
    {
    TRACESTRING( "CSuplPsyRequestor::GetRequestId " )
    return iRequestIdCount++; 
    }            
    
TVersion CSuplPsyRequestor::Version()
    {
    TRACESTRING( "CSuplPsyRequestor::Version " )
    TVersion version;
    return version;
    }
    
//  End of File  
