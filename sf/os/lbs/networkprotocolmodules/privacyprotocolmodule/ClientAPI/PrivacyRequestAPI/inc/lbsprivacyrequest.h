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

/**
 @file
 @publishedPartner
 @prototype
*/

#ifndef LBSPRIVACYREQUEST_H_
#define LBSPRIVACYREQUEST_H_

#include <e32base.h>
#include <lbs/lbsloccommon.h>
#include <lbs/lbsprivacyrequestcommon.h>

class CLbsPrivacyRequestImpl;

/** A type for the unique privacy request identifier. 
@publishedPartner
@prototype
*/
typedef TUint32 TLbsPrivacyRequestId;


/**
The MLbsPrivacyRequestObserver is an abstract interface to an external application
and is used to pass privacy responses. 

The MLbsPrivacyRequestObserver class is used in partnership with the CLbsPrivacyRequest 
interface class. 

A concrete class must derive from MLbsPrivacyRequestObserver to provide the specific 
implementation of the interface methods. 

@see CLbsPrivacyRequest

@publishedPartner
@prototype
*/
class MLbsPrivacyRequestObserver
	{
public:
	/** Defined type for TLbsPrivacyResponse */
	typedef TUint32 TLbsPrivacyResponse;

	/** The response to a privacy request.
 	Note that the TLbsPrivacyResponse enum may be extended in the future by adding
 	more enumerated values. To maintain compatibility any unrecognized values 
 	must be handled as EPrivacyResponseUnknown. */
	enum _TLbsPrivacyResponse
		{
			/** Not Used */
		EPrivacyResponseUnknown = 0,
			/** Inform the requestor the terminal will accept its location request */
		EPrivacyResponseAccepted,
			/** Inform the requestor the terminal will reject its location request */
		EPrivacyResponseRejected,
			/** Inform the application it should not respond to the location request.
			This option is currently not supported */
		EPrivacyResponseIgnored
		};
	

public:
	IMPORT_C virtual TVersion Version() const;

	/**
	OnPrivacyResponse() is called by the LBS subsystem in response to an application
	issuing a privacy request on behalf of a local client or a network.

	The method indicates to the application whether the terminal is willing to accept or has
	rejected the privacy request. This method is called for privacy verification and privacy 
	notification requests and the request ID parameter indicates which request is being 
	responded to by the LBS subsystem.

	If a request is of the notification type, the response is usually identical to the 
	default action passed as a parameter of a privacy request.
	
	The privacy response is set to the value defined by CLbsAdmin::TPrivacyTimeoutAction
	if the privacy request times out and no default action was specified as part of the 
	privacy request (@see CLbsAdmin::TPrivacyTimeoutAction.

	@param aRequestId [In] The Id of the privacy request associated with this privacy response
	@param aResponse  [In] The result of the privacy check.
	@param aError     [In] KErrNone a request has been processed successfully,
	                       KErrArgument if the request parameters are incorrect,
	                       KErrServerBusy if calling CLbsPrivacyRequest::NewPrivacyRequest() causes
	                       the number of privacy requests to exceed the maximum number of privacy requests
	                       that can be handled simultaneously (as defined by KLbsSettingMaximumExternalLocateRequests),
	                       or any other system wide error code.
	                      
	@see TLbsPrivacyResponse
	@see CLbsPrivacyRequest
	@see CLbsAdmin::TPrivacyTimeoutAction
	@see KLbsSettingMaximumExternalLocateRequests


	*/
	virtual void OnPrivacyResponse(TLbsPrivacyRequestId aRequestId,
								   const TLbsPrivacyResponse& aResponse,
								   TInt aError) = 0;
								   

protected:		
	/**
	@prototype
	Should not be used. Provides for future expansion of the observer interface. 
	Currently not implemented.
	*/
	IMPORT_C virtual TAny* ExtendedInterface(TInt aFunctionNumber, TAny* aPtr1, TAny* aPtr2);
	};


/**
The CLbsPrivacyRequest interface class provides a simple alternative to the Protocol Module API
(@see CLbsNetworkProtocolBase) and allows an application to perform privacy verification or 
notification procedures without the need of actually implementing a full Protocol Module. 

The interface is to be used in Standalone Privacy Mode only. In this mode, the LBS sub-system is used
to process privacy requests only, not location requests. It is the responsibility of the application using
this interface to act on the user's privacy response to decide whether to send the user's position.

The interface consists of two main methods, NewPrivacyRequest() (used send a new privacy request to LBS)
and CompleteRequest() (used complete a privacy request). RepeatPrivacyRequest() may be used to 
notify a user about a difference between the action that was specified in the privacy response and the action 
taken by the network.

A unique request ID couples the privacy requests and responses.

This interface supports requests originating from local applications as well as from the network.

@publishedPartner
@prototype
@see MLbsPrivacyRequestObserver
*/
NONSHARABLE_CLASS(CLbsPrivacyRequest) : public CBase
	{
public:
	IMPORT_C static CLbsPrivacyRequest* NewL(MLbsPrivacyRequestObserver& aObserver /*in*/);
	virtual ~CLbsPrivacyRequest();

public:
	IMPORT_C TInt NewPrivacyRequest(TLbsPrivacyRequestId& aRequestId, /*out*/
								 const TLbsPrivacyRequest& aPrivacy,/*in*/
								 const TLbsExternalRequestInfo& aRequestInfo/*in*/);
	
	IMPORT_C TInt RepeatPrivacyRequest(const TLbsPrivacyRequestId aRequestId, /*in*/
								 const TLbsPrivacyRequest& aPrivacy,/*in*/
								 const TLbsExternalRequestInfo& aRequestInfo/*in*/);

	IMPORT_C TInt CompleteRequest(TLbsPrivacyRequestId aRequestId/*in*/,
								  TInt aReason/*in*/);

private:
	CLbsPrivacyRequest();
	void ConstructL(MLbsPrivacyRequestObserver& aObserver);
	
	//** Prohibit copy constructor */
	CLbsPrivacyRequest(const CLbsPrivacyRequest&);
	//** Prohibit assigment operator */
	CLbsPrivacyRequest& operator= (const CLbsPrivacyRequest&);

private:
	CLbsPrivacyRequestImpl* iImpl;
	};


#endif //LBSPRIVACYREQUEST_H_
