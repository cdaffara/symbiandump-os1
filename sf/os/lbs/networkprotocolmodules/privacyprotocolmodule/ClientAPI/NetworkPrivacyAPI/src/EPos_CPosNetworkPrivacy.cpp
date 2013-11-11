//EPos_CPosNetworkPrivacy.cpp  

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



#include "lbsnetworkprivacyimpl.h"
#include "lbsdevloggermacros.h"

// ---------------------------------------------------------------------------

/**
 * Standard C++ constructor.
*/
CPosNetworkPrivacy::CPosNetworkPrivacy()
    {
        
    }

/**
 * Standard ConstructL
 */
void CPosNetworkPrivacy::ConstructL()
    {
    //It creates the object that implements the functionality for CPosNetworkPrivacy  
    iImpl = CPosNetworkPrivacyImpl::NewL();
    }

/**
* Standard Symbian OS two-phase constructor.
* @return A new instance of this class.
*/
EXPORT_C CPosNetworkPrivacy* CPosNetworkPrivacy::NewL()
    {
    LBSLOG(ELogP1, "CPosNetworkPrivacy::NewL() Begin\n");
	LBSLOG(ELogP9, "<-S CPosNetworkPrivacy::NewL() SUPL-FW ");

    CPosNetworkPrivacy* self = new (ELeave) CPosNetworkPrivacy;
    CleanupStack::PushL(self);
    self->ConstructL();
    LBSLOG(ELogP1, "CPosNetworkPrivacy::NewL() End\n");
    CleanupStack::Pop(self);
    
    LBSLOG2(ELogP9, " return = 0x%08X", self);
        
    return self;
    }

/**
 * Standard C++ destructor
*/
CPosNetworkPrivacy::~CPosNetworkPrivacy()
    {
    LBSLOG(ELogP1, "CPosNetworkPrivacy::~CPosNetworkPrivacy() Begin\n");
	LBSLOG(ELogP9, "<-A CPosNetworkPrivacy::~CPosNetworkPrivacy SUPL-FW");
    delete iImpl;
    LBSLOG(ELogP1, "CPosNetworkPrivacy::~CPosNetworkPrivacy() End\n");
    }

/**
* Sets a timeout for the connection to the LBS subsystem.

* If no client activity is registered, the LBS subsystem connection will be
* released after the timeout. The connection will be automatically
* restored when it is needed.

* @param aTimeout  The connection timeout in microseconds.
* @panic KPosNetworkPrivacyPanicCategory EPosNetworkPrivacyInvalidConnectionTimeout If this value is less than, or equal to zero.
*/

EXPORT_C void CPosNetworkPrivacy::SetConnectionTimeout(
    TTimeIntervalMicroSeconds32 aTimeout)
    {
    LBSLOG(ELogP1, "CPosNetworkPrivacy::SetConnectionTimeout() Begin\n");
	LBSLOG(ELogP9, "<-S CPosNetworkPrivacy::SetConnectionTimeout() SUPL-FW ");
	LBSLOG2(ELogP9," > TTimeIntervalMicroSeconds aTimeout = %d\n", aTimeout.Int());
	
    iImpl->SetConnectionTimeout(aTimeout);
    
    LBSLOG(ELogP1, "CPosNetworkPrivacy::SetConnectionTimeout() End\n");
    }
	
/**
* Gets the timeout for the LBS subsystem connection.

* If no client activity is registered, the LBS subsystem will be
* released after the timeout. The connection will be automatically
* restored when it is needed.

* @return The connection timeout.
*/
EXPORT_C TTimeIntervalMicroSeconds32 
CPosNetworkPrivacy::ConnectionTimeout() const
    {
    LBSLOG(ELogP1, "CPosNetworkPrivacy::ConnectionTimeout() Begin and End\n");
	LBSLOG(ELogP9, "<-A CPosNetworkPrivacy::VerifyLocationRequestL() SUPL-FW ");
	LBSLOG2(ELogP9," return = %d\n",(iImpl->ConnectionTimeout()).Int());
    return iImpl->ConnectionTimeout();
    }

/**
* Verifies a location request.

* This function is asynchronous. On completion,
* MPosVerificationObserver::HandleVerifyComplete will be called in
* the passed observer object. The completion code and request ID are
* passed as parameters.

* This function supports multiple outstanding asynchronous requests.

* This call completes with code KErrNone if the location request is
* accepted, KErrAccessDenied if the location request is rejected,
* otherwise one of the system wide error codes.

* The request can be cancelled by calling
* CancelVerifyLocationRequest with the request ID which was
* returned by the VerifyLocationRequestL call. In this case the
* observer will not be notified.

* @param aRequestInfo Network specific privacy request information.
* @param aRequestId   ID of the request. This can be used to cancel the
* request or to perform more operations on the same request, e.g.
* @ref PositionUpdateL.
* @param aObserver An object observing the completion of the
* verification request.

* @leave KErrArgument The length of a requestor exceeds KPosNWMaxRequestorLength.
* @leave KErrNoMemory Memory cannot be allocated to process the privacy request.

* @capability Location
* @capability NetworkServices
* @capability ReadDeviceData
* @capability CommDD May be used instead of Location AND NetworkServices AND ReadDeviceData
*/
EXPORT_C void CPosNetworkPrivacy::VerifyLocationRequestL(
    const CPosNetworkPrivacyRequestInfo& aRequestInfo,
    TInt& aRequestId,
    MPosVerificationObserver& aObserver)
    {
    LBSLOG(ELogP1, "CPosNetworkPrivacy::VerifyLocationRequestL() Begin\n");
	LBSLOG(ELogP9, "<-A CPosNetworkPrivacy::VerifyLocationRequestL(3) SUPL-FW ");
	LBSLOG(ELogP9, " > CPosNetworkPrivacyRequestInfo aRequestInfo =\n");	
    LBSLOG2(ELogP9, "    - TUInt iRequestInfoType  = %u\n", aRequestInfo.RequestInfoType());
    LBSLOG2(ELogP9, "    - TRequestType iRequestType  = %u\n", aRequestInfo.RequestType());
    LBSLOG2(ELogP9, " > TInt aRequestId = %d\n", aRequestId); 
    LBSLOG(ELogP9, " > MPosVerificationObserver aObserver = unrecorded \n"); 
	
    VerifyLocationRequestL(aRequestInfo, aRequestId, aObserver,
        EDecisionNotAvailable);

    LBSLOG(ELogP1, "CPosNetworkPrivacy::VerifyLocationRequestL() End\n");

    }

/**
* Verifies a location request.
* In this overload of the verification function, the caller can specify
* what the decision will be if the network thinks the verification
* takes too long. The decision is used when the verification is
* cancelled by the network.

* @param aRequestInfo Network specific privacy request information.
* @param aRequestId ID of the request. This can be used to cancel the
* request or to perform more operations on the same request, e.g.
* PositionUpdateL.
* @param aObserver  An object observing the completion of the
* verification request.
* @param aTimeoutStrategy Specifies whether the request will be
* accepted or rejected if the verification times out.
  
* @leave KErrArgument The length of a requestor exceeds KPosNWMaxRequestorLength.
* @leave KErrNoMemory Memory cannot be allocated to process the privacy request.

* @capability Location
* @capability NetworkServices
* @capability ReadDeviceData
* @capability CommDD May be used instead of Location AND NetworkServices AND ReadDeviceData
*/
EXPORT_C void CPosNetworkPrivacy::VerifyLocationRequestL(
    const CPosNetworkPrivacyRequestInfo& aRequestInfo,
    TInt& aRequestId,
    MPosVerificationObserver& aObserver,
    TRequestDecision aTimeoutStrategy)
    {
    LBSLOG(ELogP1, "CPosNetworkPrivacy::VerifyLocationRequestL() Begin\n");
    LBSLOG(ELogP9, "<-A CPosNetworkPrivacy::VerifyLocationRequestL(4) SUPL-FW ");
    LBSLOG(ELogP9, " > CPosNetworkPrivacyRequestInfo aRequestInfo = \n"); 
    LBSLOG2(ELogP9, "    - TUInt iRequestInfoType  = %u\n", aRequestInfo.RequestInfoType());
    LBSLOG2(ELogP9, "    - TRequestType iRequestType  = %u\n", aRequestInfo.RequestType());
    LBSLOG2(ELogP9, " > TInt aRequestId = %d\n", aRequestId); 
    LBSLOG(ELogP9, " > MPosVerificationObserver aObserver = unrecorded \n"); 
    LBSLOG2(ELogP9, " > TRequestDecision aTimeoutStrategy = %u\n", aTimeoutStrategy); 
        
    iImpl->VerifyLocationRequestL(aRequestInfo, aRequestId, aObserver,aTimeoutStrategy);
        
    LBSLOG(ELogP1, "CPosNetworkPrivacy::VerifyLocationRequestL() End\n");
    }

/**
* Cancels a verification request.
* If no outstanding request with the specified ID exists, this call
* will be ignored.
* @param aRequestId ID of the verification request to cancel.
* @capability Location
* @capability NetworkServices
* @capability ReadDeviceData
* @capability CommDD May be used instead of Location AND NetworkServices AND ReadDeviceData
*/
EXPORT_C void CPosNetworkPrivacy::CancelVerifyLocationRequest(
    TInt aRequestId)
    {
    LBSLOG(ELogP1, "CPosNetworkPrivacy::CancelVerifyLocationRequest() Begin\n");
    LBSLOG(ELogP9, "<-S CPosNetworkPrivacy::CancelVerifyLocationRequest(1) SUPL-FW ");
    LBSLOG2(ELogP9, " > TInt aRequestId = %d\n", aRequestId); 
    
    CancelVerifyLocationRequest(aRequestId, ECancelReasonNotAvailable);
    
    LBSLOG(ELogP1, "CPosNetworkPrivacy::CancelVerifyLocationRequest() End\n");    
    }

/**
* Cancels a verification request.

* If no outstanding request with the specified ID exists, this call
* will be ignored.

* This overload of the cancel function specifies the reason for
* canceling the verification request.

* @param aRequestId ID of the verification request to cancel.
* @param aCancelReason The cancel reason.

* @capability Location
* @capability NetworkServices
* @capability ReadDeviceData
* @capability CommDD May be used instead of Location AND NetworkServices AND ReadDeviceData
*/

EXPORT_C void CPosNetworkPrivacy::CancelVerifyLocationRequest(
    TInt aRequestId,
    TCancelReason aCancelReason)
    {
    LBSLOG(ELogP1, "CPosNetworkPrivacy::CancelVerifyLocationRequest() Begin\n");    
    LBSLOG(ELogP9, "<-S CPosNetworkPrivacy::CancelVerifyLocationRequest(2) SUPL-FW ");
    LBSLOG2(ELogP9, " > TInt aRequestId = %d\n", aRequestId); 
    LBSLOG2(ELogP9, " > TCancelReason aCancelReason = %u\n", aCancelReason); 
    
    iImpl->CancelVerifyLocationRequest(aRequestId,aCancelReason);
    
    LBSLOG(ELogP1, "CPosNetworkPrivacy::CancelVerifyLocationRequest() End\n");
    }

/**
* Called to notify the phone user of the location request.

* If this function is called and @ref CancelVerifyLocationRequest
* has already been called, the LBS subsystem will treat it as a new
* location request.

* @param aRequestInfo  Network specific privacy request information.
* @param aRequestId    ID of the request. This can be used to perform more
* operations on the same request, e.g. PositionUpdateL.
 
* @leave KErrArgument If the length of a requestor exceeds KPosNWMaxRequestorLength.

* @capability Location
* @capability NetworkServices
* @capability ReadDeviceData
* @capability CommDD May be used instead of Location AND NetworkServices AND ReadDeviceData
*/
EXPORT_C void CPosNetworkPrivacy::NotifyLocationRequestL(
    const CPosNetworkPrivacyRequestInfo& aRequestInfo,
    TInt& aRequestId)
    {
	LBSLOG(ELogP1, "CPosNetworkPrivacy::NotifyLocationRequestL() Begin\n");
    LBSLOG(ELogP9, "<-S CPosNetworkPrivacy::NotifyLocationRequestL() SUPL-FW ");
    LBSLOG(ELogP9, " > CPosNetworkPrivacyRequestInfo aRequestInfo = \n"); 
    LBSLOG2(ELogP9, "    - TUInt iRequestInfoType  = %u\n", aRequestInfo.RequestInfoType());
    LBSLOG2(ELogP9, "    - TRequestType iRequestType  = %u\n", aRequestInfo.RequestType());    
    LBSLOG2(ELogP9, " > TInt aRequestId = %d\n", aRequestId); 
	
	iImpl->NotifyLocationRequestL(aRequestInfo,aRequestId);
	
	LBSLOG(ELogP1, "CPosNetworkPrivacy::NotifyLocationRequestL() End\n");
	}

/**
* Called to report that a verification timed out.

* This function is called by the client if the timeout occurs
* after the verification in the LBS subsystem was complete AND the
* timeout policy is different than the decision returned by LBS. 
* It should not be called if * CancelVerifyLocationRequest was used
* to report the timeout during the verification.

* This means that if the verification request is running, the timeout
* is reported by calling
* CancelVerifyLocationRequest(requestId, ECancelReasonTimeout)
* and if the verification is complete, the timeout is reported using
* this function.

* @param aRequestInfo  Network specific privacy request information.
* @param aRequestId    ID obtained in the verification request.
* @param aRequestDecision The request decision made by the network.
* If the value is EDecisionNotAvailable, this function will panic.
  
* @leave The function leaves with error code KErrArgument if the length
* of a requestor exceeds KPosNWMaxRequestorLength.
* 
* @panic KPosNetworkPrivacyPanicCategory EPosNetworkPrivacyInvalidArgument If aRequestDecision is an invalid value
  
* @capability Location
* @capability NetworkServices
* @capability ReadDeviceData
* @capability CommDD May be used instead of Location AND NetworkServices AND ReadDeviceData
*/

EXPORT_C void CPosNetworkPrivacy::NotifyVerificationTimeoutL(
    const CPosNetworkPrivacyRequestInfo& aRequestInfo,
    TInt aRequestId,
    TRequestDecision aRequestDecision)
    {
    LBSLOG(ELogP1, "CPosNetworkPrivacy::NotifyVerificationTimeoutL() Begin\n");
    LBSLOG(ELogP9, "<-S CPosNetworkPrivacy::NotifyVerificationTimeoutL() SUPL-FW ");
    LBSLOG(ELogP9, " > CPosNetworkPrivacyRequestInfo aRequestInfo =\n"); 
    LBSLOG2(ELogP9, "    - TUInt iRequestInfoType  = %u\n", aRequestInfo.RequestInfoType());
    LBSLOG2(ELogP9, "    - TRequestType iRequestType  = %u\n", aRequestInfo.RequestType());
    LBSLOG2(ELogP9, " > TInt aRequestId = %d\n", aRequestId); 
    LBSLOG2(ELogP9, " > TRequestDecision aRequestDecision = %u\n", aRequestDecision);
        
    iImpl->NotifyVerificationTimeoutL(aRequestInfo,aRequestId,aRequestDecision);  
    
    LBSLOG(ELogP1, "CPosNetworkPrivacy::NotifyVerificationTimeoutL() End\n");
    }



/**
* Reports the position acquired in a location request.

* This function requires that VerifyLocationRequestL or
* NotifyLocationRequestL has previously been called for the same
* location request. The client must therefore specify the request ID
* acquired in the verification/notification.

* If the request ID does not correspond to a known request, this call
* will be ignored.

* @param aRequestId IN. An unique ID representing the location request.
* @param aPosition IN. The position acquired in the location request.

* @capability Location
* @capability NetworkServices
* @capability ReadDeviceData
* @capability CommDD May be used instead of Location AND NetworkServices AND ReadDeviceData
* @leave KErrNotSupported This function is not supported and always leaves with KErrNotSupported
*/
EXPORT_C void CPosNetworkPrivacy::PositionUpdateL(TInt /*aRequestId */,
                                      const TPosition& /*aPosition*/)
    {
    LBSLOG(ELogP1, "CPosNetworkPrivacy::PositionUpdateL() Begin and End\n");
        
    // Not yet supported
    User::Leave(KErrNotSupported);
    }




