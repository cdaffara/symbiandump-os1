/*
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Provides the reference implementation for POS message plugin
*
*/


#include <ecom/ecom.h>
#include <lbs/epos_eomasuplposerrors.h>
#include <lbs/epos_comasuplpospayload.h>
#include <lbs/epos_momasuplobserver.h>
#include "epos_comasuplpossession.h"
#include "lbsdevloggermacros.h"

_LIT(KPosPanicCategory, "EPosSUplPosMsgPlugin");

// -----------------------------------------------------------------------------
// COMASuplPosSession::NewL
// Factory function to instantiate an object of COMASuplPosSession
// -----------------------------------------------------------------------------
//
COMASuplPosSessionBase* COMASuplPosSession::NewL(
                            MOMASuplObserver* aSuplObserver, RSuplProxyPosSession& aSuplProxyPosSession )
	{
	LBSLOG(ELogP1, "COMASuplPosSession::NewL() Begin\n");
	LBSLOG(ELogP9, "<-S COMASuplPosSession::NewL() SUPL-FW\n");    
	LBSLOG2(ELogP9, " > MOMASuplObserver* aSuplObserver = 0x%08X\n", aSuplObserver);
	LBSLOG(ELogP9, " > RSuplProxyPosSession aSuplProxyPosSession = unrecorded\n");
		
	COMASuplPosSession* self = new ( ELeave ) COMASuplPosSession( aSuplObserver , aSuplProxyPosSession );
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);	

	LBSLOG2(ELogP9, " return = 0x%08X\n", self);
	LBSLOG(ELogP1, "COMASuplPosSession::NewL() End\n");

	return self; 
    }

void COMASuplPosSession::ConstructL()
	{
	LBSLOG(ELogP1, "COMASuplPosSession::ConstructL() Begin and End\n");
	}


// -----------------------------------------------------------------------------
// COMASuplPosSession::~COMASuplPosSession
// Destructor. Removes the object from the active scheduler's list
// -----------------------------------------------------------------------------
//
COMASuplPosSession::~COMASuplPosSession()
    {
	LBSLOG(ELogP1, "COMASuplPosSession::~COMASuplPosSession() Begin\n");
    LBSLOG(ELogP9, "<-A COMASuplPosSession::~COMASuplPosSession() SUPL-FW\n");    
	
	if (iSuplProxyPosSubSession.SubSessionHandle())
		{
		iSuplProxyPosSubSession.Close();
		}
	
	if ( iRequestHandler )
	    {
            delete iRequestHandler;
            iRequestHandler = NULL;
	    }
    
	delete iPosPayload;
    
	LBSLOG(ELogP1, "COMASuplPosSession::~COMASuplPosSession() End\n");
    }


// -----------------------------------------------------------------------------
// COMASuplPosSession::COMASuplPosSession
// Constructor
// -----------------------------------------------------------------------------
//
COMASuplPosSession::COMASuplPosSession( MOMASuplObserver* aSuplObserver, RSuplProxyPosSession& aSuplProxyPosSession ): 
                       COMASuplPosSessionBase( aSuplObserver ),
                       iSuplProxyPosSession ( aSuplProxyPosSession ),
                       iRequestHandler(NULL)
                       
    {
	LBSLOG(ELogP1, "COMASuplPosSession::COMASuplPosSession() Begin\n");
    iIsInitialized = EFalse;
    iRequestID = 0;
	LBSLOG(ELogP1, "COMASuplPosSession::COMASuplPosSession() End\n");
    }


// -----------------------------------------------------------------------------
// COMASuplPosSession::InitializeL
// Initializes the instance of COMASuplPosSession
// -----------------------------------------------------------------------------
//
void COMASuplPosSession::InitializeL( const TInt aRequestID, 
                        TRequestStatus& aStatus )
    {
	LBSLOG(ELogP1, "COMASuplPosSession::InitializeL() Begin\n");
    LBSLOG(ELogP9, "<-A COMASuplPosSession::InitializeL() SUPL-FW\n");    
    LBSLOG2(ELogP9, " > TInt aRequestID = %d\n", aRequestID);
        
    __ASSERT_ALWAYS( iSuplProxyPosSession.Handle()>0, User::Leave( KErrGeneral ) );
        
    aStatus         = KRequestPending;
    iRequestID      = aRequestID;
    iClientStatus   = &aStatus;

    if ( iPosPayload )
        {
        delete iPosPayload;
        iPosPayload = NULL;
        }

    TInt initStatus = KErrGeneral;
    if (!(iSuplProxyPosSubSession.SubSessionHandle()))
        {
        // Open a subsession with the supl proxy protocol module.
        initStatus =  iSuplProxyPosSubSession.Open( iSuplProxyPosSession, iRequestID ) ;
        if (initStatus == KErrNone)
            {
            //Create an instance of the active object for sending async requests to LBS
            iRequestHandler = CSuplPosRequestHandler::NewL(*this, iSuplProxyPosSubSession);
            }
        }
    else
        {
        // Reuse an existing subsession by resetting the value of the
        // request id to the new value from the SUPL FW
        initStatus = iSuplProxyPosSubSession.ResetSubsessionRequestId(iRequestID);
        __ASSERT_DEBUG(iRequestHandler != NULL,User::Invariant());
        iRequestHandler->ResetState();
        }
    
    if (initStatus != KErrNone)
        {
        // Required to return KErrGeneral in this case by the middleware
        LBSLOG(ELogP9, "->A COMASuplPosSession::InitializeL() SUPL-FW\n");
        LBSLOG(ELogP9, " > TRequestStatus aStatus = -2\n");        
        User::RequestComplete(iClientStatus, KErrGeneral);
		LBSLOG(ELogP1, "COMASuplPosSession::InitializeL() End\n");
        return;
        }

	iIsInitialized = ETrue;
	iState = EIdle;

    LBSLOG(ELogP9, "->A COMASuplPosSession::InitializeL() SUPL-FW\n");
    LBSLOG(ELogP9, " > TRequestStatus aStatus = 0\n");        
	User::RequestComplete(iClientStatus, KErrNone);
	
	LBSLOG(ELogP1, "COMASuplPosSession::InitializeL() End\n");
    }
    

// -----------------------------------------------------------------------------
// COMASuplPosSession::CancelInitialize
// Cancels an outstanding request to initialize
// -----------------------------------------------------------------------------
//
void COMASuplPosSession::CancelInitialize()
    {
	LBSLOG(ELogP1, "COMASuplPosSession::CancelInitialize() Begin and End\n");
    LBSLOG(ELogP9, "<-S COMASuplPosSession::CancelInitialize() SUPL-FW\n");    
    }


// -----------------------------------------------------------------------------
// COMASuplPosSession::HandleSuplPosMessageL
// Handles SUPL POS payload 
// -----------------------------------------------------------------------------
//
void COMASuplPosSession::HandleSuplPosMessageL( 
                        const COMASuplPosPayload* aPosPayload )
    {
	LBSLOG(ELogP1, "COMASuplPosSession::HandleSuplPosMessageL() Begin\n");
    LBSLOG(ELogP9, "<-S COMASuplPosSession::HandleSuplPosMessageL() SUPL-FW\n");    
    LBSLOG2(ELogP9, " > COMASuplPosPayload* aPosPayload =  0x%08X\n", aPosPayload);   
    
    __ASSERT_ALWAYS( iIsInitialized, User::Leave( KErrOMASuplPosInActive ) );
	
	// Delete any existing memory
	if ( iPosPayload )
		{
		delete iPosPayload;
		iPosPayload = NULL;
		}
	// make a copy of the payload (so data remains valid for server)
	// TODO: consider just copying the buffer rather that the whole class
	iPosPayload = static_cast<COMASuplPosPayload*>( aPosPayload->CloneL() );
	
	HBufC8* payload;
	COMASuplPosPayload::TOMASuplPosPayloadType payloadType;
	iPosPayload->GetPosPayload(payload, payloadType);
	
    __ASSERT_ALWAYS(payloadType == COMASuplPosPayload::ERRLP, 
    		User::Panic(KSuplProxyPosPanicCategory, KErrNotSupported));
    	
    // synchronous return error code is ignored.
    // errors are indicated in the completion of the GetSuplInfo request
	iSuplProxyPosSubSession.SendPosPayloadL(*payload);
    
	LBSLOG(ELogP1, "COMASuplPosSession::HandleSuplPosMessageL() End\n");
    }
        

// -----------------------------------------------------------------------------
// COMASuplPosSession::GetSuplInfoL
// Provides different POS parameters to the SUPL FW
// -----------------------------------------------------------------------------
//
void COMASuplPosSession::GetSuplInfoL( 
                        COMASuplInfoRequestList* aInfoRequestList,
                        TRequestStatus& aStatus )
    {
	LBSLOG(ELogP1, "COMASuplPosSession::GetSuplInfoL() Begin\n");
    LBSLOG(ELogP9, "<-A COMASuplPosSession::GetSuplInfoL() SUPL-FW\n");    
    LBSLOG2(ELogP9, "  > COMASuplInfoRequestList* aInfoRequestList = 0x%08X\n", aInfoRequestList);
    
    aStatus = KRequestPending;
    
    // store client status word for later completion
    iClientStatus = &aStatus;
    
    if(!iIsInitialized)
    	{
		LBSLOG(ELogP9, "  < TRequestStatus aStatus = KErrOMASuplPosInActive\n");
    	User::RequestComplete(iClientStatus, KErrOMASuplPosInActive );
    	return;
    	}
    if(iState != EIdle)
    	{
		LBSLOG(ELogP9, "  < TRequestStatus aStatus = KErrInUse\n");
    	User::RequestComplete(iClientStatus, KErrInUse );
    	return;
    	}
    
    iState = EGetSuplInfo;
    
    // pass async request to handler for processing
    iRequestHandler->GetSuplInfoL(aInfoRequestList);
	LBSLOG(ELogP1, "COMASuplPosSession::GetSuplInfoL() End\n");
    }
    						  

// -----------------------------------------------------------------------------
// COMASuplPosSession::CancelSuplInfoRequest
// Cancels an outstanding request to GetSuplInfoL
// -----------------------------------------------------------------------------
//
void COMASuplPosSession::CancelSuplInfoRequest()
    {
	LBSLOG(ELogP1, "COMASuplPosSession::CancelSuplInfoRequest() Begin\n");
    LBSLOG(ELogP9, "<-S COMASuplPosSession::CancelSuplInfoRequest SUPL-FW\n");

    iRequestHandler->Cancel();

	LBSLOG(ELogP1, "COMASuplPosSession::CancelSuplInfoRequest() End\n");
    }
        

// -----------------------------------------------------------------------------
// COMASuplPosSession::GetPositionL
// Provides position estimates to the SUPL FW
// -----------------------------------------------------------------------------
//
void COMASuplPosSession::GetPositionL( TRequestStatus& aStatus, 
                        COMASuplPosition* aPosition )
    {
	LBSLOG(ELogP1, "COMASuplPosSession::GetPositionL() Begin\n");
    LBSLOG(ELogP9, "<-A COMASuplPosSession::GetPositionL SUPL-FW\n");    
    LBSLOG2(ELogP9, "  > COMASuplPosition* aPosition = 0x%08X\n", aPosition);
    
    aStatus = KRequestPending;
    // store client status word for later completion
    iClientStatus = &aStatus;
    
    if(!iIsInitialized)
    	{

		LBSLOG(ELogP9, " < TRequestStatus aStatus = KErrOMASuplPosInActive\n");        

    	User::RequestComplete(iClientStatus, KErrOMASuplPosInActive );
    	return;
    	}
    if(iState != EIdle)
    	{
		LBSLOG(ELogP9, " < TRequestStatus aStatus = KErrInUse\n");        

    	User::RequestComplete(iClientStatus, KErrInUse );
    	return;
    	}
    
    // pass async request to handler for processing
    iRequestHandler->GetPositionL(aPosition);

    iState = EGetPosition;
	LBSLOG(ELogP1, "COMASuplPosSession::GetPositionL() End\n");
    }
        

// -----------------------------------------------------------------------------
// COMASuplPosSession::CancelGetPosition
// Cancels an outstanding request to GetPositionL
// -----------------------------------------------------------------------------
//
void COMASuplPosSession::CancelGetPosition()
    {
	LBSLOG(ELogP1, "COMASuplPosSession::CancelGetPosition() Begin\n");
    LBSLOG(ELogP9, "<-S COMASuplPosSession::CancelGetPosition SUPL-FW\n");    
    iRequestHandler->Cancel(); 
	LBSLOG(ELogP1, "COMASuplPosSession::CancelGetPosition() End\n");
    }
        

// -----------------------------------------------------------------------------
// COMASuplPosSession::SessionEnd
// Terminates a SUPL POS Session
// -----------------------------------------------------------------------------
//
void COMASuplPosSession::SessionEnd()
    {
	LBSLOG(ELogP1, "COMASuplPosSession::SessionEnd() Begin\n");
    LBSLOG(ELogP9, "<-S COMASuplPosSession::SessionEnd SUPL-FW\n");    
   
    //Cancel any oustanding request 
    iRequestHandler->Cancel();
    
    //Notify the SuplProxyProtocolModule server that the SUPL session had ended
    if ( iSuplProxyPosSubSession.SubSessionHandle() )
        {
            iSuplProxyPosSubSession.SessionComplete();
        }
    
    iIsInitialized = EFalse;
	LBSLOG(ELogP1, "COMASuplPosSession::SessionEnd() End\n");
    }


// -----------------------------------------------------------------------------
//
// Synchronous method that is used to set the allowed capabilites and positioning method. 
// Note that we ignore the allowed capabilities
// -----------------------------------------------------------------------------
void COMASuplPosSession::SetPosMethodAndAllowedCapabilities( TOMASuplAllowedCapabilities /* aAllowedCapabilities */, TOMASuplPositioningMethod aPosMethod)
    {
	LBSLOG(ELogP1, "COMASuplPosSession::SetPosMethodAndAllowedCapabilities() Begin\n");
    LBSLOG(ELogP9, "<-S COMASuplPosSession::SetPosMethodAndAllowedCapabilities SUPL-FW\n");    
	LBSLOG(ELogP9, "  > TOMASuplAllowedCapabilities aAllowedCapabilities = unrecorded\n");
    LBSLOG2(ELogP9, " > TOMASuplPositioningMethod aPosMethod = %u\n", aPosMethod);
        
	__ASSERT_ALWAYS( iIsInitialized, User::Panic(KSuplProxyPosPanicCategory, KErrOMASuplPosInActive ) );

	TLbsNetPosMethod posMethods[1];	
	
	switch(aPosMethod)
		{
		case EGpsSETAssistedPreferred:
		case EGpsSETAssisted:	// TA
			{
			posMethods[0].SetPosMethod(KLbsPositioningMeansGps, TPositionModuleInfo::ETechnologyNetwork | TPositionModuleInfo::ETechnologyAssisted);
			break;
			}			
		case EGpsSETBasedPreferred:
		case EGpsSETBased:	// TB
			{
			posMethods[0].SetPosMethod(KLbsPositioningMeansGps, TPositionModuleInfo::ETechnologyTerminal | TPositionModuleInfo::ETechnologyAssisted);
			break;
			}			
//		case ENoPosition:
//		    {
//		    // TODO - This corresponds to 'Privacy only' SUPL use case. 
//			// SUPL FW will not use this. Instead they will open a positioning session and call SessionEnd on it
//		    }	
//		case EAutonomousGps:	// autonomous request from network not supported
//		case EAFLT:				// not supported
//		case EeOTD:             // not supported
//		case EoTDOA:            // not supported
		default:
		    {
			__ASSERT_DEBUG(EFalse, User::Invariant());
			posMethods[0].SetPosMethod(KLbsPositioningMeansGps, TPositionModuleInfo::ETechnologyTerminal | TPositionModuleInfo::ETechnologyAssisted);
			break;
			}
		}
	
	TLbsNetPosRequestMethod method;	
	method.SetPosMethods(posMethods, 1);
	
	iSuplProxyPosSubSession.SetPositioningMethod(method);
	LBSLOG(ELogP1, "COMASuplPosSession::SetPosMethodAndAllowedCapabilities() End\n");
    }

// -----------------------------------------------------------------------------
//
// Synchronous method that is used to set the SLP Address of the server used.
// -----------------------------------------------------------------------------

void COMASuplPosSession::SetSLPAddressUsed(const TDesC& aServerAddress)
    {
	LBSLOG(ELogP1, "COMASuplPosSession::SetSLPAddressUsed() Begin and End\n");
    LBSLOG(ELogP9, "<-S COMASuplPosSession::SetSLPAddressUsed SUPL-FW\n");
    //LBSLOG2(ELogP9, " > const TDesC aServerAddress = %S\n", aServerAddress);

    iSuplProxyPosSubSession.SetSlpAddress(aServerAddress);
    }

// -----------------------------------------------------------------------------
// COMASuplPosSession::OnGetPositionComplete
// from MRequestHandlerObserver
// Handles response to GetPositionL() from subsession
void COMASuplPosSession::OnGetPositionComplete(TInt aResult)
	{
	LBSLOG(ELogP1, "COMASuplPosSession::OnGetPositionComplete() Begin\n");
	__ASSERT_ALWAYS(iState == EGetPosition, User::Panic(KPosPanicCategory, KErrGeneral));
	
	TInt error = aResult;
	if(error != KErrNone && error != KErrCancel)	// pass on KErrCancel
		{
		error = KErrOMASuplPosInfo;	// documentation dictates error
		}
	
    LBSLOG(ELogP9, "->A COMASuplPosSession::OnGetPositionComplete SUPL-FW\n");
    LBSLOG2(ELogP9, " > TRequestStatus error = %d\n", error);
	User::RequestComplete(iClientStatus, error);
	
	iState = EIdle;
	LBSLOG(ELogP1, "COMASuplPosSession::OnGetPositionComplete() End\n");
	}
	
// -----------------------------------------------------------------------------
// COMASuplPosSession::OnGetSuplInfoComplete
// from MRequestHandlerObserver
// Handles response to GetSuplInfoL() from subsession
void COMASuplPosSession::OnGetSuplInfoComplete(TInt aResult)
	{
	LBSLOG(ELogP1, "COMASuplPosSession::OnGetSuplInfoComplete() Begin\n");
	__ASSERT_ALWAYS(iState == EGetSuplInfo, User::Panic(KPosPanicCategory, KErrGeneral));
	
    LBSLOG(ELogP9, "->A COMASuplPosSession::OnGetSuplInfoComplete SUPL-FW\n");
    LBSLOG2(ELogP9, " > TRequestStatus aResult = %d\n", aResult);	
	User::RequestComplete(iClientStatus, aResult);
	
	iState = EIdle;
	LBSLOG(ELogP1, "COMASuplPosSession::OnGetSuplInfoComplete() End\n");
	}
