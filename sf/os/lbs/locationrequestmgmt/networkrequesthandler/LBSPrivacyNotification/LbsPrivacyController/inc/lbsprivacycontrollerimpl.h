// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// client side exmaple of how to use the abstract server framework
// 
//

/**
 @file
 @internalComponent
 @released
*/

#ifndef __LBSPRIVACYCONTROLLERIMPL_H__
#define __LBSPRIVACYCONTROLLERIMPL_H__

#include "lbscommoninternaldatatypes.h" // for TLbsNetSessionIdInt
#include "rsecuresessionbase.h"
#include "tserverstartparams.h"
#include "opensessionparams.h"
// jcm not nedded now uses INT #include "lbsloccommon.h" 			// For TLbsExternalRequestInfo
#include "lbsnrhserverdata.h"			// For server name and filename
#include <lbs/lbsprivacycontroller.h> 	// For MLbsPrivacyObserver
#include "lbsprivacycontrollerdata.h"
#include "lbsrootapi.h"
 

// this s really a session with server
NONSHARABLE_CLASS(RNrhServer) : public RSecureSessionBase
	{
public:
	RNrhServer();
	~RNrhServer();
	TVersion Version();
	TInt StartServer(TServerStartParams& aParams);
	
	void StartLbsL();
	};
	
// this is really a subsession of the above
NONSHARABLE_CLASS(RNrhSession) : public RSecureSubSessionBase
	{
public:
	RNrhSession();
	~RNrhSession();
	TVersion Version();
	void RegisterForServerMessages(TRequestStatus& aTrs);
	TInt SendPrivacyResponse(TLbsNetSessionIdInt aSessionId,
					CLbsPrivacyController::TRequestVerificationResult aResult);
	TInt SendPrivacyCancel(TLbsNetSessionIdInt aSessionId);

	TInt CancelRequest();

public:	
	TPckgBuf<TPrivacyRequestParams> iPrivacyRequestParamsBuf;
	TPckgBuf<TLocationUpdateParams> iLocationUpdateParamsBuf;
	TPckgBuf<TLbsSessionCompleteParams> iLbsSessionCompleteParamsBuf;
	};

NONSHARABLE_CLASS(CLbsPrivacyControllerImpl) : public CActive,
											   public MLbsProcessCloseDown
	{
public:
	static CLbsPrivacyControllerImpl* NewL(MLbsPrivacyObserver* aObserver);
	void RespondNetworkLocationRequest(TUint aRequestId, 
					CLbsPrivacyController::TRequestVerificationResult aResult);
	void CancelNetworkLocationRequest(TUint aRequestId);	
	~CLbsPrivacyControllerImpl();
	
protected:
	CLbsPrivacyControllerImpl(MLbsPrivacyObserver* aObserver);
	void ConstructL();
	
protected: // From CActive
	void RunL();
	void DoCancel();	
	
private:
	void HandlePrivacyRequest();
	void HandleLocationUpdate();
	void HandleLbsSessionComplete();

private:
	// from MLbsProcessCloseDown
	void OnProcessCloseDown();
	
private:
	MLbsPrivacyObserver* iObserver;	

	RNrhServer 		iServer;
	RNrhSession 	iSession;
	TServerStartParams 	iParams; // MUST live for as long as the client
	
	// Details of request received back from server
	TLbsNetSessionIdInt			iSessionId;

#if defined(SYMBIAN_LOCATION_PRIVACY_V2)
	CLbsCloseDownRequestDetector* iCloseDownRequestor;
#endif // SYMBIAN_LOCATION_PRIVACY_V2
	};
	
#endif // __LBSPRIVACYCONTROLLERIMPL_H__
