 //lbsnetworkprivacyimpl.cpp

// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description:
//



#include    "lbsnetworkprivacyimpl.h"
#include    "lbsnetworkprivacyverifyhandler.h"
#include <lbs/lbsnetcommon.h>
#include <lbs/lbsadmin.h>
#include	"EPos_NetworkPrivacyPanicDeclaration.h"


/**************************************************************** 
 *  The Network Privacy Implementation hides the details of the 
 *  Network Privacy from the class CPosNetworkPrivacy 
 ***************************************************************/

/*
* Default constructor.
*/
CPosNetworkPrivacyImpl::CPosNetworkPrivacyImpl()
   :iConnectionTimeout(KDefaultConnectionTimeout), iConnectionTimeoutIsSet(EFalse)
    {
        
    }

/**
 * Standard ConstructL
 */
void CPosNetworkPrivacyImpl::ConstructL()
    {
    // The timeout timer
    iConnectionTimer = CLbsCallbackTimer::NewL(*this);
    }
/**
 * Standard Symbian OS two-phase constructor.
 * @return A new instance of this class.
 */
 CPosNetworkPrivacyImpl* CPosNetworkPrivacyImpl::NewL()
    {
    CPosNetworkPrivacyImpl* self = new (ELeave) CPosNetworkPrivacyImpl;
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
    }

/**
* Destructor
*/
CPosNetworkPrivacyImpl::~CPosNetworkPrivacyImpl()
    {
    iVerifyHandler.ResetAndDestroy();
    iServer.Close();
    iConnectionTimer->Cancel();
	delete iConnectionTimer;
	iConnectionTimer = NULL;
	}


/**
* Sets a timeout for the Privacy Framework connection.
* @param aTimeout  The connection timeout in microseconds.
* A panic occurs if this value is less than, or equal to zero.
*/
void CPosNetworkPrivacyImpl::SetConnectionTimeout(
    TTimeIntervalMicroSeconds32 aTimeout)
    {
	__ASSERT_ALWAYS(aTimeout.Int() > 0,
        Panic(EPosNetworkPrivacyInvalidConnectionTimeout));
    
	iConnectionTimeout = aTimeout;
	iConnectionTimeoutIsSet = ETrue;
	
	//Check any outstanding verification requests before start connection timer 
	CheckOutstandingAndStartTimer();
	}

/**
* Gets the timeout for the Privacy Framework connection.
* @return The connection timeout.
*/
TTimeIntervalMicroSeconds32 CPosNetworkPrivacyImpl::ConnectionTimeout() const
    {
    return iConnectionTimeout;
    }

/**  
 * Check any outstanding verification requests before start connection timer 
 */
void CPosNetworkPrivacyImpl::CheckOutstandingAndStartTimer()
 	{
 	if(!iConnectionTimeoutIsSet)
 		{
 		//ensure that the connection timeout is set once (and once only)
 		GetDefaultConnectionTimeout();
 		iConnectionTimeoutIsSet = ETrue;
 		}
 	
    if(iVerifyHandler.Count()==0)
		{
		//Reset the timer before starting
	    CancelConnectionTimer();
		//Start the timer 
		StartConnectionTimer();
		}
 	}

/**  
 * Connects to the server
 */
void CPosNetworkPrivacyImpl::ConnectToServerL()
 	{
    if(!iServerConnected)
		{
		User::LeaveIfError(iServer.Connect(ELbsProxyApiTypeNetwork));
		iServerConnected = ETrue;
		}
 	}

/**
* Verifies a location request.

* @param aRequestInfo Network specific privacy request information.
* @param aRequestId ID of the request,which is unique value 
* and same ID should be used when calling CancelVerifyLocationRequest
* & NotifyVerificationTimeoutL 
* @param aObserver  An object observing the completion of the
* verification request.
* @param aTimeoutStrategy Specifies whether the request will be
* accepted or rejected if the verification times out.
* @leave The function may leave for some reason, e.g. memory allocation
* failure. In this case, the passed observer will not be notified.
* The function leaves with error code, KErrArgument, if the length
* of a requestor exceeds KPosNWMaxRequestorLength.
*/
void CPosNetworkPrivacyImpl::VerifyLocationRequestL(
    const CPosNetworkPrivacyRequestInfo& aRequestInfo,
    TInt& aRequestId,
    MPosVerificationObserver& aObserver,
    CPosNetworkPrivacy::TRequestDecision aTimeoutStrategy)
    {
   
   // Reset the timer 
    CancelConnectionTimer();
   
    // Send Connect request to the server
    ConnectToServerL();
           
    CNetworkPrivacyVerifyHandler* verifyHandler = CNetworkPrivacyVerifyHandler::NewL(iServer,aObserver,*this);
    
    CleanupStack::PushL(verifyHandler);
    
    // Convert aRequestInfo type to  internal ExternalRequestInfo2 type. 
	ExtractRequestInfoL(aRequestInfo,iExternalRequestInfo2);
	
	// Setting privacy request parameters
	TLbsNetPosRequestPrivacy posRequestPrivacy;
	
	posRequestPrivacy.SetRequestAdvice(TLbsNetPosRequestPrivacy::ERequestAdviceVerify);
	posRequestPrivacy.SetRequestAction(ConvertInternalRequestType(aTimeoutStrategy));
		
	// Start the request
	verifyHandler->VerifyLocationRequest(aRequestId,iExternalRequestInfo2,posRequestPrivacy);
    
    // Add the pointer to the array
	User::LeaveIfError(iVerifyHandler.Append(verifyHandler));
    
    CleanupStack::Pop(verifyHandler);
    }
/**
* Cancels a verification request.
* If no outstanding request with the specified ID exists, this call
* will be ignored.
* @param aRequestId ID of the verification request to cancel.
* @param aCancelReason The cancel reason.
*/
void CPosNetworkPrivacyImpl::CancelVerifyLocationRequest(
    TInt aRequestId,
    CPosNetworkPrivacy::TCancelReason aCancelReason)
    {
    // Convert cancel reason into a standard Symbian error code
    TInt cancelReason(KErrCancel);
    if (aCancelReason == CPosNetworkPrivacy::ECancelReasonTimeout)
    	{
    	cancelReason = KErrTimedOut;
    	}
    else
    	{
    	cancelReason = KErrCancel;
    	}
    
    // Find the matching verifier object with this request Id
    for (TInt i = 0; i < iVerifyHandler.Count(); i++)
    	{
    	if (iVerifyHandler[i]->RequestId() == aRequestId)
    		{    		
    		iVerifyHandler[i]->CancelRequest(cancelReason);
    		}
    	}   
    }

/**
* Notify the location request to the phone user. 
* @param aRequestInfo Network specific privacy request information.
* @param aRequestId   ID of the request,which is unique value 
* and same ID should be used when calling CancelVerifyLocationRequest
* & NotifyVerificationTimeoutL. 
* @leave The function leaves with error code, KErrArgument, if the length
* of a requestor exceeds KPosNWMaxRequestorLength.
*/
void CPosNetworkPrivacyImpl::NotifyLocationRequestL(
    const CPosNetworkPrivacyRequestInfo& aRequestInfo,
    TInt& aRequestId)
    {
    // Reset the timer 
    CancelConnectionTimer();
    
    // Send Connect request to the server
    ConnectToServerL();
           
    // Convert aRequestInfo type to ExternalRequestInfo2 type. 
	ExtractRequestInfoL(aRequestInfo,iExternalRequestInfo2);
	
	// Setting privacy request parameters
	TLbsNetPosRequestPrivacy posRequestPrivacy;
	
	posRequestPrivacy.SetRequestAdvice(TLbsNetPosRequestPrivacy::ERequestAdviceNotify);
	posRequestPrivacy.SetRequestAction(TLbsNetPosRequestPrivacy::ERequestActionAllow);
	
	// Start the notification
	User::LeaveIfError(iServer.NotifyLocationRequest(iExternalRequestInfo2, aRequestId,posRequestPrivacy));
	
	// Check outstanding verification requests before start timer 
	CheckOutstandingAndStartTimer();
    }

/**
* Called to report that a verification timed out.
* @param aRequestInfo Network specific privacy request information.
* @param aRequestId ID obtained in the verification request.
* @param aRequestDecision The request decision made by the network.
* If the value is EDecisionNotAvailable, this function will panic.
* @leave The function leaves with error code KErrArgument if the length
* of a requestor exceeds KPosNWMaxRequestorLength.
* 
*/
void CPosNetworkPrivacyImpl::NotifyVerificationTimeoutL(
    const CPosNetworkPrivacyRequestInfo& aRequestInfo,
    TInt aRequestId,
    CPosNetworkPrivacy::TRequestDecision aRequestDecision)
    {
    // Reset the timer 
    CancelConnectionTimer();
    
    // Send Connect request to the server
    ConnectToServerL();

    // Convert aRequestInfo type to ExternalRequestInfo2 type. 
	ExtractRequestInfoL(aRequestInfo,iExternalRequestInfo2);

    __ASSERT_ALWAYS(aRequestDecision != CPosNetworkPrivacy::EDecisionNotAvailable,
        Panic(EPosNetworkPrivacyInvalidArgument));
    
    // Setting privacy request parameters 
    TLbsNetPosRequestPrivacy posRequestPrivacy;
	
    posRequestPrivacy.SetRequestAdvice(TLbsNetPosRequestPrivacy::ERequestAdviceNotify);
	posRequestPrivacy.SetRequestAction(ConvertInternalRequestType(aRequestDecision));
	
    // Start the notification
	User::LeaveIfError(iServer.NotifyVerificationTimeout(iExternalRequestInfo2,aRequestId,posRequestPrivacy));
    
    // Check outstanding verification requests before start timer 
	CheckOutstandingAndStartTimer();
    }

/** Called when a call to VerifyLocationRequestL completes, 
 */
void CPosNetworkPrivacyImpl::HandleVerifyComplete(TInt aRequestId, TInt /*aCompletionCode*/)
	{
	
	/* After the request finished need to delete that particular object 
	and remove the pointer in the array */ 
    
    for (TInt i = iVerifyHandler.Count()-1; i >= 0; i--)
	    {
	    if (iVerifyHandler[i]->RequestId() == aRequestId)
	        {
	        delete iVerifyHandler[i];
	        iVerifyHandler.Remove(i);
	        break;
	        }
	    }
	
	//Check outstanding verification requests before start timer 
	CheckOutstandingAndStartTimer();
	}
     	
/** Called when the timer expires */
void CPosNetworkPrivacyImpl::OnTimerEventL(TInt /*aTimerId*/)
     {
     iServer.Close();
     iServerConnected = EFalse;
     }


/** Called if OnTimerEventL leaves */ 
TInt CPosNetworkPrivacyImpl::OnTimerError(TInt /*aTimerId*/, TInt aError)
     {
     iServer.Close();
     iServerConnected = EFalse;
     return aError;	
     }

/*
* CheckIdLengthL Leaves with KErrArgument if the id length
* exceeds KLbsMaxRequesterIdSize2 (255)
*/
void CheckIdLengthL(const TDesC& aId)
    {
    if (aId.Length() > KLbsMaxRequesterIdSize2)
        {
        User::Leave(KErrArgument);
        }
    }

/*
* CheckClientNameLengthL Leaves with KErrArgument if the LCS Client Name string length
* exceeds KLbsMaxClientNameSize2 (255)
*/
void CheckClientNameLengthL(const TDesC& aId)
    {
    if (aId.Length() > KLbsMaxClientNameSize2)
        {
        User::Leave(KErrArgument);
        }
    }

/* 
* Starts connection timer after an interval 
*/
void CPosNetworkPrivacyImpl::StartConnectionTimer() 
	{
	iConnectionTimer->EventAfter(iConnectionTimeout,0); 
	}

/** 
* Cancel connection timer  
*/
 void CPosNetworkPrivacyImpl::CancelConnectionTimer() 
 	{
 	iConnectionTimer->Cancel();
 	}

/**
* Extracts requestor information from CPosNetworkPrivacyRequestInfo 
* type to TLbsExternalRequestInfo2 internal type.
* @param aRequestInfo The Network Privacy type.
* @param aExternalRequestInfo2 The corresponding internal data type.
*/

void CPosNetworkPrivacyImpl::ExtractRequestInfoL(
    const CPosNetworkPrivacyRequestInfo& aRequestInfo,
    TLbsExternalRequestInfo2& aExternalRequestInfo2)
    {
    // Reset the values of aExternalRequestInfo2.
    // Use the 'placement-new' operator to call the default 
    // constructor of TLbsExternalRequestInfo2 to reset its values.
    new (&aExternalRequestInfo2) TLbsExternalRequestInfo2;

    ExtractRequestTypeL(aRequestInfo,aExternalRequestInfo2);
    
    // Convert depending on the type of request info.
    switch (aRequestInfo.RequestInfoType())
        {
        case EPosGSMPrivacyRequestInfo :
            {
            aExternalRequestInfo2.SetNetworkType(TLbsExternalRequestInfo::ENetworkGSM);
            ExtractGSMRequestInfoL(aRequestInfo, aExternalRequestInfo2);
            }
            break;
        case EPosSUPLPrivacyRequestInfo: 
            {
            aExternalRequestInfo2.SetNetworkType(TLbsExternalRequestInfo::ENetworkSUPL);
            ExtractSUPLRequestInfoL(aRequestInfo, aExternalRequestInfo2);
            }
            break;
        default:
            aExternalRequestInfo2.SetNetworkType(TLbsExternalRequestInfo::ENetworkTypeUnknown);
            break;
        }
    }

/**
* Extracts request type from CPosNetworkPrivacyRequestInfo 
* type to TLbsExternalRequestInfo2 internal type.
* @param aRequestInfo The Network Privacy type.
* @param aExternalRequestInfo2 The corresponding internal data type.
*/
void CPosNetworkPrivacyImpl::ExtractRequestTypeL(
    const CPosNetworkPrivacyRequestInfo& aRequestInfo,
    TLbsExternalRequestInfo2& aExternalRequestInfo2)
    {
    switch (aRequestInfo.RequestType())
        {
        case CPosNetworkPrivacyRequestInfo::ERequestSingleShot :
            {
            aExternalRequestInfo2.SetRequestType(TLbsExternalRequestInfo::ERequestSingleShot);
            }
            break;
        case CPosNetworkPrivacyRequestInfo::ERequestPeriodic: 
            {
            aExternalRequestInfo2.SetRequestType(TLbsExternalRequestInfo::ERequestPeriodic);
            }
            break;
        case CPosNetworkPrivacyRequestInfo::ERequestAreaEvent :
            {
            aExternalRequestInfo2.SetRequestType(TLbsExternalRequestInfo::ERequestAreaEvent);
            }
            break;
        case CPosNetworkPrivacyRequestInfo::ERequestSingleShotSilent :
            {
            aExternalRequestInfo2.SetRequestType(TLbsExternalRequestInfo::ERequestSingleShotSilent);
            }
            break;
        case CPosNetworkPrivacyRequestInfo::ERequestPeriodicSilent :
            {
            aExternalRequestInfo2.SetRequestType(TLbsExternalRequestInfo::ERequestPeriodicSilent);
            }
            break;
        case CPosNetworkPrivacyRequestInfo::ERequestAreaEventSilent :  
            {
            aExternalRequestInfo2.SetRequestType(TLbsExternalRequestInfo::ERequestAreaEventSilent);
            }
            break;
        default:
            aExternalRequestInfo2.SetRequestType(TLbsExternalRequestInfo::ERequestTypeUnknown);
            break;
        }
    }

/**
* Extracts requestor information from a GSM privacy request.
* @param aRequestInfo The Network Privacy type.
* @param aExternalRequestInfo2 The corresponding internal data type.
*/
void CPosNetworkPrivacyImpl::ExtractGSMRequestInfoL(const CPosNetworkPrivacyRequestInfo& aRequestInfo,
                                                    TLbsExternalRequestInfo2& aExternalRequestInfo2)
    {
    const CPosGSMPrivacyRequestInfo* requestInfo =
        static_cast<const CPosGSMPrivacyRequestInfo*> (&aRequestInfo);

    TPtrC id(NULL, 0);
    CPosGSMPrivacyRequestInfo::TIdType idType =
        CPosGSMPrivacyRequestInfo::EIdTypeLogicalName;

    if (requestInfo->GetLCSClient(id, idType) == KErrNone)
        {
        CheckClientNameLengthL(id);
                        
        ConvertGSMRequestInfoL(CPosNetworkPrivacy::ERequestorClientName, idType, id, aExternalRequestInfo2);
        }

    if (requestInfo->GetRequestor(id, idType) == KErrNone)
        {
        CheckIdLengthL(id);

        ConvertGSMRequestInfoL(CPosNetworkPrivacy::ERequesterId ,idType, id, aExternalRequestInfo2);
        }
    }
/**
* Convert a GSM requestor info type to the internal type 
* @param aIdType The identification type.
* @param aId The identification string.
* @param aExternalRequestInfo2 The corresponding internal data type.
*/
void CPosNetworkPrivacyImpl::ConvertGSMRequestInfoL(CPosNetworkPrivacy::TRequestorType aRequestorType, 
		                                            CPosGSMPrivacyRequestInfo::TIdType aIdType,
                                                    const TDesC& aId,
                                                    TLbsExternalRequestInfo2& aExternalRequestInfo2)
    {
    TLbsExternalRequestInfo::TFormatIndicator format = TLbsExternalRequestInfo::EFormatUnknown;
              
    switch (aIdType)
        {
        case CPosGSMPrivacyRequestInfo::EIdTypeLogicalName :
            format = TLbsExternalRequestInfo::EFormatLogicalName;
            break;
        case CPosGSMPrivacyRequestInfo::EIdTypeMSISDN :
            format = TLbsExternalRequestInfo::EFormatMSISDN;
            break;
        case CPosGSMPrivacyRequestInfo::EIdTypeEmailAddress :
            format = TLbsExternalRequestInfo::EFormatEmailAddress;
            break;
        case CPosGSMPrivacyRequestInfo::EIdTypeURL :
            format = TLbsExternalRequestInfo::EFormatURL;
            break;
        case CPosGSMPrivacyRequestInfo::EIdTypeSIPURL :
            format = TLbsExternalRequestInfo::EFormatSIPURL;
            break;
        case CPosGSMPrivacyRequestInfo::EIdTypeIMSPublicIdentity :
            format = TLbsExternalRequestInfo::EFormatIMSPublicIdentity;
            break;
        default:
            break;
        }
        if(aRequestorType == CPosNetworkPrivacy::ERequestorClientName)
        	    {
        	    aExternalRequestInfo2.SetClientName(aId);	
        	    aExternalRequestInfo2.SetClientNameFormat(format);
        	    }
        if(aRequestorType ==CPosNetworkPrivacy::ERequesterId)
        	    {
        	    aExternalRequestInfo2.SetRequesterId(aId);	
        	    aExternalRequestInfo2.SetRequesterIdFormat(format);
        	    }      
    }

/**
* Extracts requestor information from a SUPL privacy request.
*
* @param aRequestInfo The Network Privacy type.
* @param aExternalRequestInfo2 The corresponding internal data type.
*/
void CPosNetworkPrivacyImpl::ExtractSUPLRequestInfoL(const CPosNetworkPrivacyRequestInfo& aRequestInfo,
                                                    TLbsExternalRequestInfo2& aExternalRequestInfo2)
    {
    const CPosSUPLPrivacyRequestInfo* requestInfo =
        static_cast<const CPosSUPLPrivacyRequestInfo*> (&aRequestInfo);

    TPtrC id(NULL, 0);
    CPosSUPLPrivacyRequestInfo::TIdType idType =
        CPosSUPLPrivacyRequestInfo::EIdTypeLogicalName;

    if (requestInfo->GetLCSClient(id, idType) == KErrNone)
        {
        CheckClientNameLengthL(id);
                        
        ConvertSUPLRequestInfoL(CPosNetworkPrivacy::ERequestorClientName, idType, id, aExternalRequestInfo2);
        }

    if (requestInfo->GetRequestor(id, idType) == KErrNone)
        {
        CheckIdLengthL(id);

        ConvertSUPLRequestInfoL(CPosNetworkPrivacy::ERequesterId ,idType, id, aExternalRequestInfo2);
        }
    }
/**
* Convert a SUPL requestor info type to the internal type.
* @param aIdType The identification type.
* @param aId The identification string.
* @param aExternalRequestInfo2 
*/
void CPosNetworkPrivacyImpl::ConvertSUPLRequestInfoL(CPosNetworkPrivacy::TRequestorType aRequestorType, 
		                                            CPosSUPLPrivacyRequestInfo::TIdType aIdType,
                                                    const TDesC& aId,
                                                    TLbsExternalRequestInfo2& aExternalRequestInfo2)
    {
    TLbsExternalRequestInfo::TFormatIndicator format = TLbsExternalRequestInfo::EFormatUnknown;
              
    switch (aIdType)
        {
        case CPosSUPLPrivacyRequestInfo::EIdTypeLogicalName :
            format = TLbsExternalRequestInfo::EFormatLogicalName;
            break;
        case CPosSUPLPrivacyRequestInfo::EIdTypeMSISDN :
            format = TLbsExternalRequestInfo::EFormatMSISDN;
            break;
        case CPosSUPLPrivacyRequestInfo::EIdTypeEmailAddress :
            format = TLbsExternalRequestInfo::EFormatEmailAddress;
            break;
        case CPosSUPLPrivacyRequestInfo::EIdTypeURL :
            format = TLbsExternalRequestInfo::EFormatURL;
            break;
        case CPosSUPLPrivacyRequestInfo::EIdTypeSIPURL :
            format = TLbsExternalRequestInfo::EFormatSIPURL;
            break;
        case CPosSUPLPrivacyRequestInfo::EIdTypeIMSPublicIdentity :
            format = TLbsExternalRequestInfo::EFormatIMSPublicIdentity;
            break;
        case CPosSUPLPrivacyRequestInfo::EIdTypeMIN :
            format = TLbsExternalRequestInfo::EFormatMIN;
            break;
        case CPosSUPLPrivacyRequestInfo::EIdTypeMDN:
            format = TLbsExternalRequestInfo::EFormatMDN;
            break;
        default:
            break;
        }
        if(aRequestorType == CPosNetworkPrivacy::ERequestorClientName)
        	    {
        	    aExternalRequestInfo2.SetClientName(aId);	
        	    aExternalRequestInfo2.SetClientNameFormat(format);
        	    }
        if(aRequestorType == CPosNetworkPrivacy::ERequesterId)
        	    {
        	    aExternalRequestInfo2.SetRequesterId(aId);	
        	    aExternalRequestInfo2.SetRequesterIdFormat(format);
        	    }      
    }

/**
 * Converts Network Privacy request decision type to Privacy FW internal
 * type
 * @param aRequestDecision The Network Privacy type.
 * @return The corresponding internal data type.
 */
TLbsNetPosRequestPrivacy::TLbsRequestAction CPosNetworkPrivacyImpl::ConvertInternalRequestType(CPosNetworkPrivacy::TRequestDecision aRequestDecision)
    {
    switch (aRequestDecision)
        {
        case CPosNetworkPrivacy::EDecisionNotAvailable:
            return TLbsNetPosRequestPrivacy::ERequestActionNotUsed;
        case CPosNetworkPrivacy::EDecisionAccepted:
            return TLbsNetPosRequestPrivacy::ERequestActionAllow;
        case CPosNetworkPrivacy::EDecisionRejected:
            return TLbsNetPosRequestPrivacy::ERequestActionReject;
        default:
            Panic(EPosNetworkPrivacyInvalidArgument);
        }
    return TLbsNetPosRequestPrivacy::ERequestActionReject;
    }

/**
* Reads admin setting for network privacy client connection timeout.
* Must be called after LBS has started to avoid requiring the WRITEDEVICEDATA capability.
*/
void CPosNetworkPrivacyImpl::GetDefaultConnectionTimeout()
    {
	// Read admin setting for network privacy client connection timeout
    iConnectionTimeout = 0;
	CLbsAdmin* admin = NULL;
	TRAPD(err, admin = CLbsAdmin::NewL());
	if(err == KErrNone)
	    {
        TUint connectionTimeout = KLbsDefaultPrivacyConnectionTimeout;
        TInt err = admin->Get(KLbsSettingPrivacyConnectionTimeout, connectionTimeout);
        if(err == KErrNone)
            {
            iConnectionTimeout = connectionTimeout;
            }
	    }
    }





