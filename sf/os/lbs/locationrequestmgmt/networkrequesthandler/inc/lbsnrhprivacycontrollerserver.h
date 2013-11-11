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
// lbsnrhserver.h
// 
//

/**
 @file
 @internalComponent
 @released
*/
#ifndef __LBSPRIVACYCONTROLLERSERVER_H__
#define __LBSPRIVACYCONTROLLERSERVER_H__

#include <e32property.h>
#include "tserverstartparams.h"
#include "csecureasbase.h"
#include "csecureserverbase.h"
#include "csecuresessionbase.h"
#include "csecuresessionsubsessionbase.h"
#include "csubsessionbase.h"
#include "messageutils.h"
#include "lbsnetworkrequesthandler.h"
#include "privacyhandler.h"
#include "lbsprivacycontrollerdata.h"
#include "lbsnrhserver.h"


/**
The class used to correct the class type and class size when converting from TPositionCourseInfo, 
TPositionSatelliteInfo,TPositionExtendedSatelliteInfo and HPositionGenericInfo to TPositionInfo.
  
@internalComponent
*/
class TPositionInfoClassTypeFixer : public TPositionInfo
	{
public:
	TPositionInfoClassTypeFixer(const TPositionInfo& aPositionInfo);
 	};


class CNrhPrivacyServerSubsession : public CBase, 
									public MSubSessionImpl,
									public MLbsSessionObserver
	{
public:
	static CNrhPrivacyServerSubsession* NewL();
	~CNrhPrivacyServerSubsession();
	
	// from MSubSessionImpl
	void DispatchL(const RMessage2& aMessage);
	void DispatchError(const RMessage2& aMessage, TInt aError);
	void CreateSubSessionL(const RMessage2& aMessage,const CSecureServerBase* aServer);
	void CloseSubSession();
	// via MSubSessionImpl::MRelease	
	void VirtualRelease();
	//
	// action methods. These functions pass message to the client.
	//
	void SetClientReady(const RMessage2& aMessage);
	void CancelClientReady(const RMessage2& aMessage);
	void HandlePrivacyResponse(const RMessage2& aMessage);
	void HandlePrivacyCancel(const RMessage2& aMessage);

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
	
protected:
	CNrhPrivacyServerSubsession();
	void ConstructL();

private:
	TInt CheckPrivacyObserverVersion(const RMessage2& aMessage);
		
private:	
	const CNrhServer* iNrhServer;
	
	RMessage2 iPrivacyControllerMessage;

	// Need to be able to store instances of the message data
	// in case a message arrives before the client has indicated
	// it's ready for the next one.
	
	RArray<TNrhServerMessageType>	iPendingMessage;
	
	//RArray<TPrivacyRequestParams>	iPendingPrivacyRequests;
	CArrayFixSeg<TPrivacyRequestParams>	iPendingPrivacyRequests;
	RArray<TLocationUpdateParams>	iPendingLocationUpdates;
	RArray<TLbsSessionCompleteParams>	iPendingSessionCompletes;	
	};
	
#endif //__LBSPRIVACYCONTROLLERSERVER_H__
