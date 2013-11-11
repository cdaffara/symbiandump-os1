// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <e32base.h>
#include "lbsdevloggermacros.h"
#include "lbsprivacyrequest.h"
#include "lbsprivacyrequestimpl.h"


//=============================================================================
// MLbsPrivacyRequestObserver
//=============================================================================

/**
Class version control.
@return the version of the interface
*/
EXPORT_C TVersion MLbsPrivacyRequestObserver::Version() const
	{
	return TVersion(1, 0, 0);
	}

/**
Should not be used. Provides for future expansion of the observer interface. 
*/  
/*virtual*/ EXPORT_C TAny* MLbsPrivacyRequestObserver::ExtendedInterface
	(
	TInt  /*aFunctionNumber*/,
	TAny* /*aPtr1*/,
	TAny* /*aPtr2*/
	)
	{
	return NULL;
	}


//=============================================================================
// CLbsPrivacyRequest
//=============================================================================
/**
Static constructor for CLbsPrivacyRequest
@param aObserver [In] The observer class that receives privacy responses.
*/
EXPORT_C CLbsPrivacyRequest* CLbsPrivacyRequest::NewL(MLbsPrivacyRequestObserver& aObserver /*in*/)
	{
	LBSLOG(ELogP9, "<-S CLbsPrivacyRequest::NewL()\n");
    LBSLOG(ELogP9, " > MLbsPrivacyRequestObserver aObserver = unrecorded \n"); 
	CLbsPrivacyRequest* self = new (ELeave) CLbsPrivacyRequest;
    CleanupStack::PushL(self);
    self->ConstructL(aObserver);
    CleanupStack::Pop(self);
    return self;
	}
	
/**
Default constructor.

@internalComponent
@prototype
*/
CLbsPrivacyRequest::CLbsPrivacyRequest()
	{
	}
	
/**
2nd phase static constructor.
@internalComponent
@prototype
*/
void CLbsPrivacyRequest::ConstructL(MLbsPrivacyRequestObserver& aObserver)
	{
	iImpl = CLbsPrivacyRequestImpl::NewL(aObserver);
	}

/**
Closes the interface and frees all the resources used.
*/	
/*virtual*/ CLbsPrivacyRequest::~CLbsPrivacyRequest()
	{
    LBSLOG(ELogP9, "<-S CLbsPrivacyRequest::~CLbsPrivacyRequest()\n");
    delete iImpl;
	}

/**
Asks the LBS subsystem to process a privacy request from either a network or a local application.

NewPrivacyRequest() is the first stage of a privacy verification or notification process and
supplies information about the requester but does not actually ask the device for its position.

The NewPrivacyRequest() method returns quickly but does not indicate if the 
request should be accepted or rejected. When the LBS subsystem has determined whether the user 
wants to accept or reject the request (or it times out), it responds via a call to MLbsPrivacyRequestObserver::OnPrivacyResponse(). 
The response indicates if a user is willing to accept a location request.

Default timeout may be controlled through the LBS Admin API.

Note on capabilities: Calling this method only requires the capability NetworkServices 
is the request is from the network as indicated by the TLbsExternalRequestInfo

@param aRequestId    [Out] Unique identifier of a request.
@param aPrivacy      [In]  Specifies the type of a request and a default action.
@param aRequestInfo  [In]  Detailed information about the request.

The following table presents valid privacy requests.
If a request is found invalid, the method will return with
the KErrArgument error code(if the combination of different values is incorrect)
or the KErrNotSupported error code (if the functionality is not supported).

@code
TLbsPrivacyRequest::
||================================================||
|| TLbsRequestAdvice      | TLbsRequestAction     ||
||================================================||
|| any, except            | any, except           ||
|| ERequestAdviceNotUsed, | ERequestActionNotUsed ||
|| ERequestAdviceStealth  |                       ||
||================================================||


TLbsExternalRequestInfo::
||=======================================================================================================||
|| TRequestSource         | TRequestType        | TNetworkType        | TFormatIndicator                 ||
||=======================================================================================================||
|| ERequestSourceLocal    | ERequestSingleShot  | ENetworkTypeUnknown | any except                       ||
||                        | ERequestPeriodic    |                     | EFormatUnknown                   ||
||-------------------------------------------------------------------------------------------------------||
|| ERequestSourceNetwork  | any except          | GSM/WCDMA           | any, except EFormatUnknown,      ||
||                        | ERequestTypeUnknown |                     | EFormatMIN, EFormatMDN and       ||
||                        |                     |                     | EFormatAppUID                    ||
||                        |                     |---------------------|----------------------------------||
||                        |                     | SUPL                | any, except EFormatUnknown       ||
||                        |                     |                     | and EFormatAppUID                ||
||=======================================================================================================||
@endcode

@see MLbsPrivacyRequestObserver::OnPrivacyResponse

@param aRequestId    [Out] A unique identifier of the request. 
@param aPrivacy      [In]  Type of a request (e.g. notification)
@param aRequestInfo  [In]  Detailed information about the request (e.g. source, client name)

@return KErrArgument If the arguments are not valid, or any other system-wide error code

@see CLbsAdmin
@see KLbsSettingHomeExternalLocate
@see KLbsSettingRoamingExternalLocate
@see CLbsAdmin::ExternalLocateService
@see KLbsSettingPrivacyTimeoutAction
@see CLbsAdmin::TPrivacyTimeoutAction
@see TLbsExternalRequestInfo::_TRequestSource
				
*/
EXPORT_C TInt CLbsPrivacyRequest::NewPrivacyRequest(TLbsPrivacyRequestId& aRequestId, /*out*/
												 const TLbsPrivacyRequest& aPrivacy,/*in*/
												 const TLbsExternalRequestInfo& aRequestInfo/*in*/)
	{
    LBSLOG(ELogP9, "->A CLbsPrivacyRequest::NewPrivacyRequest(aRequestId, aPrivacy, aRequestInfo) LbsPrivacyProtocolModule\n");
    TInt err = iImpl->NewPrivacyRequest(aRequestId, aPrivacy, aRequestInfo);

#ifdef ENABLE_LBS_DEV_LOGGER
    LBSLOG2(ELogP8, "\t < TLbsPrivacyRequestId aRequestId = %d\n", aRequestId);
    LBSLOG(ELogP8, "\t > TLbsPrivacyRequest aPrivacy = \n");
    LBSLOG2(ELogP8, "\t\t > TLbsRequestAdvice iAdvice = %d\n", aPrivacy.RequestAdvice());
    LBSLOG2(ELogP8, "\t\t > TLbsRequestAction iAction = %d\n", aPrivacy.RequestAction());
    TBuf8<256> logInfo8;
    LBSLOG(ELogP8, "\t > TLbsExternalRequestInfo aRequestInfo = \n");
    aRequestInfo.GetRequesterId(logInfo8);
    LBSLOG2(ELogP8, "\t\t > TLbsRequesterId iRequesterId = %S\n", &logInfo8);
    aRequestInfo.GetClientName(logInfo8);
    LBSLOG2(ELogP8, "\t\t > TLbsClientName iClientName = %S\n", &logInfo8);
    aRequestInfo.GetClientExternalId(logInfo8);
    LBSLOG2(ELogP8, "\t\t > TLbsClientExternalId iClientExternalId = %S\n", &logInfo8);
    LBSLOG2(ELogP8, "\t\t > TRequestType iRequestType = %d\n", aRequestInfo.RequestType());
    LBSLOG2(ELogP8, "\t\t > TRequestSource iRequestSource = %d\n", aRequestInfo.RequestSource());
    LBSLOG2(ELogP8, "\t\t > TNetworkType iNetworkType = %d\n", aRequestInfo.NetworkType());
    LBSLOG2(ELogP8, "\t\t > TFormatIndicator iRequesterFormat = %d\n", aRequestInfo.RequesterIdFormat());
    LBSLOG2(ELogP8, "\t\t > TFormatIndicator iClientFormat = %d\n", aRequestInfo.ClientNameFormat());
    LBSLOG2(ELogP8, "\t\t > TCodingScheme iRequesterCoding = %d\n", aRequestInfo.RequesterIdCodingScheme());
    LBSLOG2(ELogP8, "\t\t > TCodingScheme iClientCoding = %d\n", aRequestInfo.ClientNameCodingScheme());
    LBSLOG2(ELogP8, "\t return = %d\n", err);
#endif
    return err;
	}

/**
RepeatPrivacyRequest is an optional variation of the NewPrivacyRequest() method and may be used to 
notify a user about a difference between the action that was specified in the privacy response and the action 
taken by the network.

For example, this situation may happen if the API client process decides to ignore a user's privacy response
or not to wait for a user's privacy response before processing a location request from the network.
This is not recommended because of the private nature of a user's location information - in most cases it should only
be revealed with the permission of the mobile device user. In general it is recommended that the API client process
always waits for the privacy response to a NewPrivacyRequest() call before disclosing a user's position.

Note that before calling RepeatPrivacyRequest() a client must always have previously called both NewPrivacyRequest() and CompleteRequest().
The client must provide a known request id parameter which is a return parameter from NewPrivacyRequest().
If the client provides an unknown request id parameter LBS will still process the request and return KErrNone.
Request ids that are less than 1, or greater than KTIntMax are not valid, LBS will not process them and KErrArgument will be returned.

Default timeout may be controlled through the LBS Admin API.

The following table presents valid privacy requests.
If a request is found invalid, the method will return with
the KErrArgument error code (if the combination of different values is incorrect)
or the KErrNotSupported error code (if the functionality is not supported).

@code
TLbsPrivacyRequest::
||================================================||
|| TLbsRequestAdvice      | TLbsRequestAction     ||
||================================================||
|| ERequestAdviceNotify   | any, except           ||
||                        | ERequestActionNotUsed ||
||================================================||

TLbsExternalRequestInfo::
||=======================================================================================================||
|| TRequestSource         | TRequestType        | TNetworkType        | TFormatIndicator                 ||
||=======================================================================================================||
|| ERequestSourceNetwork  | any except          | GSM/WCDMA           | any, except EFormatUnknown,      ||
||                        | ERequestTypeUnknown |                     | EFormatMIN, EFormatMDN and       ||
||                        |                     |                     | EFormatAppUID                    ||
||                        |                     |---------------------|----------------------------------||
||                        |                     | SUPL                | any, except EFormatUnknown       ||
||                        |                     |                     | and EFormatAppUID                ||
||=======================================================================================================||
@endcode

@param aRequestId    [In]  The unique identifier of the request (as returned from previous call to NewPrivacyRequest())
@param aPrivacy      [In]  Type of a request (e.g. notification)
@param aRequestInfo  [In]  Detailed information about the request (e.g. source, client name)

@return KErrArgument If the arguments are not valid, or any other system-wide error code
				
@see CLbsPrivacyRequest::NewPrivacyRequest()
@see CLbsAdmin
@see KLbsSettingHomeExternalLocate
@see KLbsSettingRoamingExternalLocate
@see CLbsAdmin::ExternalLocateService
@see KLbsSettingPrivacyTimeoutAction
@see CLbsAdmin::TPrivacyTimeoutAction
@see TLbsExternalRequestInfo::_TRequestSource

*/
EXPORT_C TInt CLbsPrivacyRequest::RepeatPrivacyRequest(const TLbsPrivacyRequestId aRequestId, /*in*/
								 const TLbsPrivacyRequest& aPrivacy,/*in*/
								 const TLbsExternalRequestInfo& aRequestInfo/*in*/)
	{
    LBSLOG(ELogP9, "->A CLbsPrivacyRequest::RepeatPrivacyRequest(aRequestId, aPrivacy, aRequestInfo) LbsPrivacyProtocolModule\n");
    TInt err = iImpl->RepeatPrivacyRequest(aRequestId, aPrivacy, aRequestInfo);
#ifdef ENABLE_LBS_DEV_LOGGER
    LBSLOG2(ELogP8, "\t > TLbsPrivacyRequestId aRequestId = %d\n", aRequestId);
    LBSLOG(ELogP8, "\t > TLbsPrivacyRequest aPrivacy = \n");
    LBSLOG2(ELogP8, "\t\t > TLbsRequestAdvice iAdvice = %d\n", aPrivacy.RequestAdvice());
    LBSLOG2(ELogP8, "\t\t > TLbsRequestAction iAction = %d\n", aPrivacy.RequestAction());
    TBuf8<256> logInfo8;
    LBSLOG(ELogP8, "\t > TLbsExternalRequestInfo aRequestInfo = \n");
    aRequestInfo.GetRequesterId(logInfo8);
    LBSLOG2(ELogP8, "\t\t > TLbsRequesterId iRequesterId = %S\n", &logInfo8);
    aRequestInfo.GetClientName(logInfo8);
    LBSLOG2(ELogP8, "\t\t > TLbsClientName iClientName = %S\n", &logInfo8);
    aRequestInfo.GetClientExternalId(logInfo8);
    LBSLOG2(ELogP8, "\t\t > TLbsClientExternalId iClientExternalId = %S\n", &logInfo8);
    LBSLOG2(ELogP8, "\t\t > TRequestType iRequestType = %d\n", aRequestInfo.RequestType());
    LBSLOG2(ELogP8, "\t\t > TRequestSource iRequestSource = %d\n", aRequestInfo.RequestSource());
    LBSLOG2(ELogP8, "\t\t > TNetworkType iNetworkType = %d\n", aRequestInfo.NetworkType());
    LBSLOG2(ELogP8, "\t\t > TFormatIndicator iRequesterFormat = %d\n", aRequestInfo.RequesterIdFormat());
    LBSLOG2(ELogP8, "\t\t > TFormatIndicator iClientFormat = %d\n", aRequestInfo.ClientNameFormat());
    LBSLOG2(ELogP8, "\t\t > TCodingScheme iRequesterCoding = %d\n", aRequestInfo.RequesterIdCodingScheme());
    LBSLOG2(ELogP8, "\t\t > TCodingScheme iClientCoding = %d\n", aRequestInfo.ClientNameCodingScheme());
    LBSLOG2(ELogP8, "\t return = %d\n", err);
#endif
    return err;
	}
	
/**
An application issues a single call to RequestComplete() to complete a request and to
return the final result code to the LBS subsystem. 

In normal situations, RequestComplete() will be invoked when the overall request has
finished. However, it may occur at any stage - for example to cancel an outstanding request.

An application MUST invoke RequestComplete() at the end of every privacy verification/notification sequence.

@param aRequestId [In] A unique identifier of the request to be completed. This ID is allocated when a 
                       new request is created.
@param aReason    [In] Reason for the completion of the request. 
                       Indicates the overall success or failure of the request. 
                       KErrNone if the request has been completed successfully, 
                       KErrCancel if the application wants to completed the request prematurely 
                       (before a response has been received) or any other system wide error code.

@return A system-wide error code
                  
@capability Location
@capability ReadDeviceData

@see TLbsExternalRequestInfo::TRequestType

*/
EXPORT_C TInt CLbsPrivacyRequest::CompleteRequest(TLbsPrivacyRequestId aRequestId/*in*/,
								  				  TInt aReason/*in*/)
	{
    LBSLOG(ELogP9, "->S CLbsPrivacyRequest::CompleteRequest(aRequestId, aReason) LbsPrivacyProtocolModule\n");
    LBSLOG2(ELogP8, "\t > TLbsPrivacyRequestId aRequestId = %d\n", aRequestId);
    LBSLOG2(ELogP8, "\t > TLbsPrivacyRequestId aReason = %d\n", aReason);
    return iImpl->CompleteRequest(aRequestId, aReason);
	}

