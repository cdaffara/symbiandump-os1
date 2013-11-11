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
* Description:
*
*/
/**
@file
@internalComponent
@released
*/
#ifndef __PRIVACYHANDLER_H__ 
#define __PRIVACYHANDLER_H__

#include <lbs/lbsloccommon.h>         // For TLbsExternalRequestInfo
#include <lbs/lbsprivacycontroller.h> // For MLbsSessionObserver
#include "privacyhandlerobserver.h"
#include "lbsprivacynotifier.h"
#include "lbstimer.h"
#include "lbsprivacycontrollerdata.h"
#include "lbsnetregstatusint.h"
#include <lbs/lbsadmin.h>

class CLbsPrivacyNotifier;
class CPrivacyRequest;
class CLbsAdmin;

class MLbsSessionObserver
    {
public:

    virtual void ProcessNetworkLocationRequest(TLbsNetSessionIdInt aRequestId,
       										   const TLbsNetworkEnumInt::TLbsNetProtocolServiceInt aSessionType,
											   const TLbsExternalRequestInfo& aRequestInfo,
											   const TLbsNetPosRequestPrivacyInt& aNetPosRequestPrivacy,
											   TBool aIsEmergency) = 0;
                     
    virtual void ProcessNetworkPositionUpdate(TLbsNetSessionIdInt aRequestId, 
                              				  const TPositionInfo& aPosInfo) = 0;
    virtual void ProcessRequestComplete(TLbsNetSessionIdInt aRequestId, 
                                        TInt aReason) = 0;
    };

/** Abstract base class for all privacy request handlers.

Each privacy request handler for a specific API/interface
must derive from this class and implement the pure virtual 
functions.
 */
class CPrivacyHandler : public CActive, 
						public MLbsSessionObserver, 
						public MPrivacyHandlerObserver
    {
protected:
	enum TRequiredPrivacyAction
		{
		ERequiredPrivacyActionNone,
		ERequiredPrivacyActionNotify,
		ERequiredPrivacyActionVerify
		};
		
public:
	static CPrivacyHandler* CreateL(MPrivacyHandlerObserver* aObserver, 
									CLbsAdmin& aLbsAdmin,
									RLbsNetworkRegistrationStatus& aNetRegStatus);

protected:
    CPrivacyHandler(CLbsAdmin& aLbsAdmin,
					RLbsNetworkRegistrationStatus& aNetRegStatus);

protected: 
	// From CActive
    virtual void RunL() = 0;
    virtual void DoCancel() = 0;
    
    virtual void GetPrivacyTimeoutAction(
    		TLbsNetworkEnumInt::TLbsPrivacyResponseInt& aResponse, 
    		const TLbsNetPosRequestPrivacyInt& aRequestPrivacy);
    
	void GetExternalLocateAdminSetting(const TLbsNetworkEnumInt::TLbsNetProtocolServiceInt aSessionType,
    		CLbsAdmin::TExternalLocateService& aExternalLocateService);


protected:
	CLbsAdmin& iLbsAdmin;
	RLbsNetworkRegistrationStatus& iNetRegStatus;
    MPrivacyHandlerObserver* iObserver;

private:
    virtual void RegisterObserver(MPrivacyHandlerObserver* aObserver) = 0;

    };

///////////////////////////////////////////////////////////////////////////////
// CPrivacyRequest
///////////////////////////////////////////////////////////////////////////////

/* Class that holds the data for a single privacy request.
*/
class CPrivacyRequest : public CBase
	{
public:
	/* Privacy request states.
	
	The state is needed to keep track of what has happened to this 
	request so far. For example, it can be used to guard against
	multiple responses from the privacy controller application
	(or privacy notifier or Q&N Notifier).
	 */
	enum TPrivReqState
		{
		/* 'Undefined' state. */
		EPrivReqStateUnknown = 0,
		/* Privacy request sent to user; waiting for response. */
		EPrivReqStateWaitPrivacyResponse,
		/* Privacy response received and sent into NRH. */
		EPrivReqStateResponseSent,
		/* Privacy request complete message received from NRH. */
		EPrivReqStateCompleted
		};

public:
	static CPrivacyRequest* NewL();
	~CPrivacyRequest();

public:
	TPrivReqState State() const;
	void SetState(TPrivReqState aState);

	const TLbsNetSessionIdInt& SessionId() const;
	void SetSessionId(const TLbsNetSessionIdInt& aSessionId);

	const TLbsNetworkEnumInt::TLbsNetProtocolServiceInt& SessionType() const;
	void SetSessionType(const TLbsNetworkEnumInt::TLbsNetProtocolServiceInt& aSessionType);
	
	const TLbsExternalRequestInfo& RequestInfo() const;
	void SetRequestInfo(const TLbsExternalRequestInfo& aRequestInfo);
	
	const TLbsNetPosRequestPrivacyInt& RequestPrivacy() const;
	void SetRequestPrivacy(const TLbsNetPosRequestPrivacyInt& aRequestPrivacy);
	
	TBool IsEmergency() const;
	void SetIsEmergency(TBool aIsEmergency);
	
	TTime StartTime() const;
	void SetStartTime();

public:
	static TBool IsSessionIdEqual(
			const TLbsNetSessionIdInt* aSessionId,
			const CPrivacyRequest& aItem);

protected:
	CPrivacyRequest();
	CPrivacyRequest(const CPrivacyRequest&);
	
	void ConstructL();

private:
	TPrivReqState iState;
	TLbsNetSessionIdInt iSessionId;
	TLbsNetworkEnumInt::TLbsNetProtocolServiceInt iSessionType;
	TBool iIsEmergency;
	TTime iStartTime;
	TLbsNetPosRequestPrivacyInt iRequestPrivacy;
	TLbsExternalRequestInfo2 iRequestInfo;
	};

///////////////////////////////////////////////////////////////////////////////
// CPrivacyNotifierHandler
///////////////////////////////////////////////////////////////////////////////
class CPrivacyNotifierHandler : public CPrivacyHandler, 
								public MLbsPrivacyNotifierObserver, 
								public MLbsCallbackTimerObserver
	{
    
public:
	static CPrivacyNotifierHandler* NewL(CLbsAdmin& aLbsAdmin,
										 RLbsNetworkRegistrationStatus& aNetRegStatus);
	~CPrivacyNotifierHandler();

public: // From MLbsSessionObserver

	void ProcessNetworkLocationRequest(TLbsNetSessionIdInt aRequestId, 
							const TLbsNetworkEnumInt::TLbsNetProtocolServiceInt aSessionType,
							const TLbsExternalRequestInfo& aRequestInfo, 
							const TLbsNetPosRequestPrivacyInt& aNetPosRequestPrivacy,
							TBool aIsEmergency);
 
	void ProcessNetworkPositionUpdate(TLbsNetSessionIdInt aRequestId, 
                              const TPositionInfo& aPosInfo);
	void ProcessRequestComplete(TLbsNetSessionIdInt aRequestId, 
                                TInt aReason);

public: // From MLbsPrivacyNotifierObserver
	void OnNotificationDialogResponse(TInt aErr, 
	const TLbsPrivacyNotifierResponse& aResponse);

public: // From MLbsCallbackTimerObserver
	void OnTimerEventL(TInt aTimerId);
	TInt OnTimerError(TInt aTimerId, TInt aError);

public: // From MPrivacyHandlerObserver
	void SetServerObserver(MLbsSessionObserver* aNrhPrivacyServer);
    void OnRespondNetworkLocationRequest(const TLbsNetSessionIdInt& aRequestId, 
                            TLbsNetworkEnumInt::TLbsPrivacyResponseInt aRequestResult,
                            TInt aReason);
    void OnCancelNetworkLocationRequest(const TLbsNetSessionIdInt& aRequestId);

protected: // From CActive
	void RunL();
	void DoCancel();

protected:    
	CPrivacyNotifierHandler(CLbsAdmin& aLbsAdmin,
							RLbsNetworkRegistrationStatus& aNetRegStatus);
	void RegisterObserver(MPrivacyHandlerObserver* aObserver);
	void ConstructL();

private:
	TBool IsPrivacyRequestActive();
	void SendNextPrivacyRequest();
	void SendPrivacyResponse();
    void WaitForPrivacyResponse();
	
	TInt BufferPrivacyRequest(
		const TLbsNetSessionIdInt& aSessionId,
		const TLbsNetworkEnumInt::TLbsNetProtocolServiceInt aSessionType,
		const TLbsExternalRequestInfo& aRequestInfo,
		const TLbsNetPosRequestPrivacyInt& aRequestPrivacy,
		TBool aIsEmergency);
	
	void RemovePrivacyRequestFromBuffer(
		const TLbsNetSessionIdInt& aSessionId);

	void GetRequiredNotificationAction(
		TBool aIsEmergency,
		CLbsAdmin::TExternalLocateService aExternalLocate,
		TRequiredPrivacyAction& aPrivacyAction, 
		TBool& aTimeoutRequired,
		TLbsNetworkEnumInt::TLbsPrivacyResponseInt& aResponse,
		TLbsNetPosRequestPrivacyInt& aPrivacy);
    
private:
	CLbsCallbackTimer* iPrivacyResponseTimer;
	TRequiredPrivacyAction		iNotifierAction;
	CLbsPrivacyNotifier*		iNotifier;
	RPointerArray<CPrivacyRequest> iRequestBuffer;
	TBool						iPrivacyResponseOutstanding;
	};

///////////////////////////////////////////////////////////////////////////////
// CPrivacyControllerHandler
///////////////////////////////////////////////////////////////////////////////
class CPrivacyControllerHandler :	public CPrivacyHandler, 
									public MLbsCallbackTimerObserver
	{
public:
	static CPrivacyControllerHandler* NewL(CLbsAdmin& aLbsAdmin,
										   RLbsNetworkRegistrationStatus& aNetRegStatus);
	~CPrivacyControllerHandler();

public: // From MLbsSessionObserver

	void ProcessNetworkLocationRequest(TLbsNetSessionIdInt aRequestId, 
							const TLbsNetworkEnumInt::TLbsNetProtocolServiceInt aSessionType,
							const TLbsExternalRequestInfo& aRequestInfo, 
							const TLbsNetPosRequestPrivacyInt& aNetPosRequestPrivacy,
							TBool aIsEmergency);
 
	void ProcessNetworkPositionUpdate(TLbsNetSessionIdInt aRequestId, 
                              const TPositionInfo& aPosInfo);
	void ProcessRequestComplete(TLbsNetSessionIdInt aRequestId, 
                                TInt aReason);
                                
public: // From MPrivacyHandlerObserver
	void SetServerObserver(MLbsSessionObserver* aNrhPrivacyServer);
    void OnRespondNetworkLocationRequest(const TLbsNetSessionIdInt& aRequestId, 
                            TLbsNetworkEnumInt::TLbsPrivacyResponseInt aRequestResult,
                            TInt aResponseReason);
    void OnCancelNetworkLocationRequest(const TLbsNetSessionIdInt& aRequestId);
public: // From MLbsCallbackTimerObserver
	void OnTimerEventL(TInt aTimerId);
	TInt OnTimerError(TInt aTimerId, TInt aError);

protected:
	CPrivacyControllerHandler(CLbsAdmin& aLbsAdmin,
							  RLbsNetworkRegistrationStatus& aNetRegStatus);
	void ConstructL();

	// From CActive
	void RunL();
	void DoCancel();

	void GetPrivacyTimeoutAction(
			TLbsNetworkEnumInt::TLbsPrivacyResponseInt& aResponse, 
			const TLbsNetPosRequestPrivacyInt& aRequestPrivacy);	
	
protected:
	void RegisterObserver(MPrivacyHandlerObserver* aObserver);

private:
	void StartResponseTimer();
	void CancelResponseTimer();

private:
	static TBool IsSessionIdMatch(const CPrivacyRequest& aReq1, const CPrivacyRequest& aReq2);
	CPrivacyRequest* FindPrivacyRequest(
			const TLbsNetSessionIdInt& aSessionId,
			TBool aRemoveRequest);

private:
	MLbsSessionObserver* iNrhServer;
	CLbsCallbackTimer* iPrivacyResponseTimer;
	RPointerArray<CPrivacyRequest> iRequestBuffer;
	CPrivacyRequest* iEmergencyPrivReq;
	TBool iSessionActive;
	TBool iRefPosReported;
	TBool iIsEmergency;
	CLbsAdmin::TLbsBehaviourMode iLbsBehaviourMode;	
	TBool iAlwaysVerify;	
	
	};


#endif // __PRIVACYHANDLER_H__ 
