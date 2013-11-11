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
/*
*/

#ifndef PRIVACYADVANCEDNOTIFIERHANDLER_H__ 
#define PRIVACYADVANCEDNOTIFIERHANDLER_H__


#include <lbs/lbsloccommon.h>         // For TLbsExternalRequestInfo
#include <lbs/lbsprivacycontroller.h> // For MLbsSessionObserver
#include <lbs/epos_privacy.h>
#include <lbs/epos_cposrequestor.h>
#include "EPos_PrivacyInternal.h"
#include "privacyhandlerobserver.h"
#include "privacyhandler.h"


class CPosDialogCtrl;
class CPosPrivManager;
class RPosRequestorStack;

/** Extended data class with extra members for the Q&N notififer.
*/
class CPrivacyAdvancedRequest : public CPrivacyRequest
	{
public:
	static CPrivacyAdvancedRequest* NewL();
	~CPrivacyAdvancedRequest() {}

	const TPosRequestData RequestData() const { return iRequestData; }
	void SetRequestData(const TPosRequestData& aRequestData) { iRequestData = aRequestData; }
	
	static TBool IsSessionIdEqual(
		const TLbsNetSessionIdInt* aSessionId,
		const CPrivacyAdvancedRequest& aRequest)
		{
		return (*aSessionId == aRequest.SessionId());
		}

protected:
	CPrivacyAdvancedRequest() {};
	CPrivacyAdvancedRequest(const CPrivacyAdvancedRequest&);

private:
	TPosRequestData iRequestData;
	};

/** Privacy handler for the Q&N API.
 */
class CPrivacyAdvancedNotifierHandler : public CPrivacyHandler
	{
public:
	static CPrivacyAdvancedNotifierHandler* NewL(CLbsAdmin& aLbsAdmin,
												 RLbsNetworkRegistrationStatus& aNetRegStatus);
	~CPrivacyAdvancedNotifierHandler();

public: 
	// From MLbsSessionObserver
	void ProcessNetworkLocationRequest(TLbsNetSessionIdInt aRequestId, 
							const TLbsNetworkEnumInt::TLbsNetProtocolServiceInt aSessionType,
							const TLbsExternalRequestInfo& aRequestInfo, 
							const TLbsNetPosRequestPrivacyInt& aNetPosRequestPrivacy,
							TBool aIsEmergency);

	void ProcessNetworkPositionUpdate(TLbsNetSessionIdInt aRequestId, 
	                          const TPositionInfo& aPosInfo);
	void ProcessRequestComplete(TLbsNetSessionIdInt aRequestId, 
	                            TInt aReason);

public: 
	// From MPrivacyHandlerObserver
	void SetServerObserver(MLbsSessionObserver* aNrhPrivacyServer);
	void OnRespondNetworkLocationRequest(const TLbsNetSessionIdInt& aRequestId, 
	                        TLbsNetworkEnumInt::TLbsPrivacyResponseInt aRequestResult,
	                        TInt aResponseReason);
	void OnCancelNetworkLocationRequest(const TLbsNetSessionIdInt& aRequestId);

protected: 
	// From CActive
	void RunL();
	void DoCancel();

protected:    
	CPrivacyAdvancedNotifierHandler(CLbsAdmin& aLbsAdmin,
									RLbsNetworkRegistrationStatus& aNetRegStatus);
	void RegisterObserver(MPrivacyHandlerObserver* aObserver);
	void ConstructL();
	
	TBool IsPrivacyRequestActive();
	void SendNextPrivacyRequest();
	void SendPrivacyResponse();
	
	void SendNotificationRequest(CPrivacyAdvancedRequest& aActiveRequest);
	void SendVerificationRequest(CPrivacyAdvancedRequest& aActiveRequest);
	
	TInt BufferPrivacyRequest(
			const TLbsNetSessionIdInt& aSessionId,
			const TLbsExternalRequestInfo& aRequestInfo,
			const TLbsNetPosRequestPrivacyInt& aRequestPrivacy,
			TBool aIsEmergency);
	void RemovePrivacyRequestFromBuffer(const TLbsNetSessionIdInt& aSessionId);
					   
	void ResetRequestData(TPosRequestData& aRequestData);

	void ConvertActionToTPosRequestDecision(TLbsNetPosRequestPrivacyInt::TLbsRequestActionInt aAction,
											TPosRequestDecision& aDecision);
	TInt InsertVerification(CPrivacyAdvancedRequest* aRequest);
	TInt DeferNotification(TInt aIndex);
private:
	CPosDialogCtrl* iPrivacyDialogController;
	CPosPrivManager* iPrivacyManager;
	
	TLbsNetSessionIdInt iHighestSessionId;
	
	RPointerArray<CPrivacyAdvancedRequest> iRequestBuffer;
	};


#endif // PRIVACYADVANCEDNOTIFIERHANDLER_H__ 
