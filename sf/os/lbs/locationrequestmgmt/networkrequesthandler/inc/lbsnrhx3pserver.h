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
 @internalTechnology
 @released
*/
#ifndef __LBSNRHX3PSERVER_H__
#define __LBSNRHX3PSERVER_H__

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
#include "lbsx3pdata.h"
#include "lbsnrhserver.h"
#include "LbsLogger.h"
#include "lbsnetinternalapi.h"


class CNrhX3pServerSubsession : public CBase, 
								public MSubSessionImpl,
								public MX3pHandlerObserver
	{
public:
	static CNrhX3pServerSubsession* NewL(RLbsLogger& aLogger);
	~CNrhX3pServerSubsession();
	
	// from MSubSessionImpl
	void DispatchL(const RMessage2& aMessage);
	void DispatchError(const RMessage2& aMessage, TInt aError);
	void CreateSubSessionL(const RMessage2& aMessage,const CSecureServerBase* aServer);
	void CloseSubSession();
	
	// via MSubSessionImpl::MRelease	
	void VirtualRelease();
	
	//
	// action methods.
	//
	void TransmitPosition(const RMessage2& aMessage);
	void CancelTransmitPosition(const RMessage2& aMessage);
	void SetTransmitOptions(const RMessage2& aMessage);
	void GetTransmitOptions(const RMessage2& aMessage);
	void ReportReferenceLocation(const RMessage2& aMessage);

protected: // From MX3pHandlerObserver
	void OnTransmitLocationComplete(const TLbsNetSessionIdInt& aSessionId,
									const TPositionInfoBase& aPositionInfo,
									TInt aReason);
	void OnReferenceLocationAvailable(const TLbsNetSessionIdInt& aSessionId,
									const TPositionInfoBase& aPositionInfo);

protected:
	CNrhX3pServerSubsession(RLbsLogger& aLogger);
	void ConstructL();
	
	TBool CheckTransmitPositionAdminSettings();

private:
	RMessage2					iTransmitPositionMessage;
	TLbsTransmitPositionOptions	iTransmitOptions;
	TLbsNetSessionIdInt			iSessionId;
	TUint						iClientPosInfoClassSize;
	const CNrhServer*			iServer;
	
	RMessage2					iReferencePositionMessage;
	TUint						iClientRefPosInfoClassSize;
	TInt 						iRefLocErrorCode;

	// Logging
	RLbsLogger&					iLogger;
	TTransmitPositionParams		iTransmitParams;
	TUid						iClientUid;
	};
	
	
#endif //__LBSNRHX3PSERVER_H__
