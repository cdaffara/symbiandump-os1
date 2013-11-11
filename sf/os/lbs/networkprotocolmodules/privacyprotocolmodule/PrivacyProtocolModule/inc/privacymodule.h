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

#ifndef __PRIVACYMODULE_H__
#define __PRIVACYMODULE_H__

#include <e32base.h>

#include <lbs/lbsnetcommon.h>
#include <lbs/lbsnetprotocolbase.h>

#include "privacyprotocol.h"
#include "privacyserver.h"
#include "privacyrequestinfo.h"


/** 
LBS Network Protocol Interface 
This class implements the LBS Network Gateway plug-in interface which
allows the module to be accessed by, and to interact with, LBS.
@see CLbsNetworkProtocolBase
@see MLbsNetworkProtocolObserver

The class also implements the internal MPrivacyProtocol interface
which is used to receive calls from network privacy API clients via the server session.
@see MPrivacyProtocol
*/
NONSHARABLE_CLASS(CPrivacyProtocol) : public CLbsNetworkProtocolBase, public MPrivacyProtocol
	{
	
public:

	static CPrivacyProtocol* NewL(TAny* aParams);
	~CPrivacyProtocol();

	// MPrivacyProtocol methods:

	void CancelVerifyLocationRequest(CCancelPrivacyRequestInfo* aCancelInfo);
	void GetNextSessionId(TLbsNetSessionId& aSessionId);
	void NotifyLocationRequest(CPrivacyRequestInfo* aRequestInfo);
	void NotifyVerificationTimeout(CPrivacyRequestInfo* aRequestInfo);
	void VerifyLocationRequest(CPrivacyRequestInfo* aRequestInfo);
	void PrivacyLocationRequest(CPrivacyRequestInfo* aRequestInfo);
	void CompletePrivacyRequest(TUint32 aRequestId, TInt aReason);
	void SessionDisconnected(const CSession2* aSession);
	

	// CLbsNetworkProtocolBase methods:

	void RespondPrivacyRequest(const TLbsNetSessionId& aSessionId,
								const TLbsPrivacyResponse& aResponse);
	
	void RespondLocationRequest(const TLbsNetSessionId& aSessionId,
								TInt  aReason,
								const TPositionInfoBase& aPosInfo);

	void RequestTransmitLocation(const TLbsNetSessionId& aSessionId,
								const TDesC& aDestination,
								TInt aPriority);
									
	void CancelTransmitLocation(const TLbsNetSessionId& aSessionId,
								TInt  aReason);
	
	void RequestAssistanceData(TLbsAsistanceDataGroup aDataRequestMask);
	
	void RequestNetworkLocation(const TLbsNetSessionId& aSessionId,
							const TLbsNetPosRequestOptionsBase& aOptions);	
	
	void CancelNetworkLocation(const TLbsNetSessionId& aSessionId,
							TInt  aReason);

	void RequestSelfLocation(const TLbsNetSessionId& aSessionId,
							const TLbsNetPosRequestOptionsBase& aOptions);	
	
	void CancelSelfLocation(const TLbsNetSessionId& aSessionId,
							TInt  aReason);

	void AdviceSystemStatus(TLbsSystemStatus aStatus);

private:

	CPrivacyProtocol(MLbsNetworkProtocolObserver& aObserver);
	
	void ConstructL();
	
	void ProcessRequest(CPrivacyRequestInfo* aRequest);

private:

	/** Stores the requests */
	CPrivacyRequestBuffer* iRequestBuffer;

	/** Protocol manager object pointer.
	This object is created and owned by this class and represents the
	internal manager for the protocol module. The object is used to
	make requests into the module in support of LBS interactions.
	*/
	CPrivacyServer* iServer;

	/** Reference to this protocol module's observer.
	This is used to make callbacks into LBS from the protocol module.
	*/
	MLbsNetworkProtocolObserver& iObserver;
	
	/** ID of the next session */
	TLbsNetSessionId iNextSessionId;
	
	/** Maximum number of requests that can be active at once */
	TInt iMaxSimultaneousActiveRequests;
	};


#endif // __PRIVACYMODULE_H__
