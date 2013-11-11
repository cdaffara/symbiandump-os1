/*
* Copyright (c) 2009 NTT DOCOMO, INC. All Rights Reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* NTT DOCOMO, INC. - initial contribution.
* Nokia Corporation - additional changes.
* 
* Contributors:
* 
* Description:
*
*/

/**
 @file
 @publishedPartner
 @released
*/

#ifndef LBS_PRIVACY_CONTROLLER_H
#define LBS_PRIVACY_CONTROLLER_H

#include <e32std.h>

class TLbsExternalRequestInfo;
class TPositionInfo;
class CLbsPrivacyControllerImpl;


/**
The mixin class that defines the interface for the LBS privacy 
controller observer.

@publishedPartner
@released 

The callbacks defined in this class are called by the LBS system 
when needed, e.g. when a network location request is received, 
when the reference position is received, when the final position 
is calculated and when the request has completed.
*/
class MLbsPrivacyObserver
	{
public:
	/** Contains the user notification action defined by the 
	network location request (MT-LR). This setting may be
	overridden by the setting(s) in the LBS Admin Settings API.
	
	@see CLbsAdmin::TExternalLocateService
	@see CLbsAdmin::TPrivacyTimeoutAction
	*/
	enum TNotificationType
		{
		/** Unused/Unknown notification. */
		ENotifyUnknown = 0,
		/** Accept and notify user.*/
		ENotifyLocationAccepted,
		/** Verify (query user). Accept if no response.*/
		ENotifyAndVerifyLocationAcceptedIfNoReponse,
		/** Verify (query user). Reject if no response.*/
		ENotifyAndVerifyLocationRejectedIfNoReponse,
		/** Reject and notify user.*/
		ENotifyLocationRejected,
		};

public:
	IMPORT_C virtual TVersion Version();
	
	/**
	Called when a network location request is received by the LBS system.
	The implementer of this function must decide whether the location request 
	is allowed and give the result to the LBS system. The result is passed 
	by calling CPrivacyController::SendRequestVerificationResult().
	
	@param aRequestId Identifies the request within the LBS system.
	@param aRequestInfo Information about the external client making the request.
	  Note that the actual type may be of one the derived classes
	  e.g. TLbsExternalRequestInfo2.
	  The type may be determined by calling ClassType().
	  If the class is of the TLbsExternalRequestInfo2 type, this parameter may be 
	  casted to access the extended information.
	@param aNotificationType The value set by the network for the user notification.
	
	@see TLbsExternalRequestInfo2
	@see TLbsLocClassTypeBase::ClassType()
	*/
	virtual void ProcessNetworkLocationRequest(TUint aRequestId, const TLbsExternalRequestInfo& aRequestInfo, const TNotificationType& aNotificationType) = 0;
	
	/**
	Called whenever a position update is ready while processing a network 
	location request. Typically, it will be called twice:
	1) As soon as the network reference position is available.
	2) As soon as the final calculated position is available.
	
	@param aRequestId Identifies the request within the LBS system.
	@param aPosInfo The latest position data.
	*/
	virtual void ProcessNetworkPositionUpdate(TUint aRequestId, const TPositionInfo& aPosInfo) = 0;
	
	/**
	Called when a network location request (e.g. MT-LR) has been completed.
	
	@param aRequestId Identifies the request within the LBS system.
	@param aReason Contains the reason for the completion of the request.
	               Usually KErrNone, or a standard Symbian error code.
	               KErrNotFound if aRequestId is not a currently 
	               active external location request.
	*/
	virtual void ProcessRequestComplete(TUint aRequestId, TInt aReason) = 0;
	};


/**
The mixin class that defines an extended interface for the LBS privacy 
controller observer.

It should be used only if the start up of a Privacy Controller process 
(platform specific, not a part of LBS) is controlled by the LBS sub-system
by adding the name of the Privacy Controller process to the LBS Root Process
configuration file.

@see MLbsPrivacyObserver
@see CLbsPrivacyController

@publishedPartner
@released
*/
class MLbsPrivacyObserver2 : public MLbsPrivacyObserver
	{
public:
	IMPORT_C virtual TVersion Version();
	
	/**
	Called whenever the LBS sub-system is closing down.
	After receiving the call the current process MUST close itself down.
	
	@param aReservedParam Reserved for future extensions.
	*/
	virtual void ProcessCloseDownNotification(TInt32 aReservedParam) = 0;
	};


/**
An application can use this class to register itself as the user
privacy controller for the LBS system. 

The application must supply an implementation of the MLbsPrivacyObserver 
interface so that it can be notified of important events. 

When it receives the MLbsPrivacyObserver::ProcessNetworkLocationRequest()
notification it must reply by calling 
CLbsPrivacyController::RespondNetworkLocationRequest() to tell the 
LBS system the user response to the network location request.

If the request was accepted then the application will recieve more 
notifications:
1) MLbsPrivacyObserver::ProcessNetworkPositionUpdate() with the reference
position when the assistance data is received from the network.
2) MLbsPrivacyObserver::ProcessNetworkPositionUpdate() with the calculated
position when it is sent back to the network.
3) MLbsPrivacyObserver::ProcessRequestCompleted() when the application
no longer needs to listen to notifications for that request.

Each notification contains aRequestId, which identifies each network
location request within the LBS system. It is meant to be used so that
each of the different notifications can be matched to the request they
came from.
*/	
NONSHARABLE_CLASS(CLbsPrivacyController) : public CBase
	{
  public:
  	/** The user response to the network location request dialog. */
  	enum TRequestVerificationResult
  		{
  		/** Unused request. */
  		ERequestUnknown = 0,
  		/** Request accepted (permission granted). */
  	   	ERequestAccepted,
  		/** Request rejected (permission denied). */
  	   	ERequestRejected,
  		/** Tell the Lbs system to ignore the request. Currently not implemented. */
  		ERequestIgnore
  		};
		
public:
	IMPORT_C static CLbsPrivacyController* NewL(MLbsPrivacyObserver& aObserver);
	~CLbsPrivacyController();
	
	IMPORT_C void RespondNetworkLocationRequest(TUint aRequestId, TRequestVerificationResult aResult);
	
	IMPORT_C void CancelNetworkLocationRequest(TUint aRequestId);

private:
	CLbsPrivacyController();
	void ConstructL(MLbsPrivacyObserver& aObserver);
	
private:
	/** CLbsPrivacyControllerImp* Impl is the internal handle to the Implementation */
	CLbsPrivacyControllerImpl* iImpl;
	};

	
#endif // LBS_PRIVACY_CONTROLLER_H
