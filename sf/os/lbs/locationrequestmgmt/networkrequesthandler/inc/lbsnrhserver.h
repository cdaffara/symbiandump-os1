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
//

/**
 @file
 @internalComponent
 @released
*/
#ifndef __LBSNRHSERVER_H__
#define __LBSNRHSERVER_H__

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
#include "x3phandler.h"
#include "LbsLogger.h"


class CNrhServerImpl : public CBase, public MCreateServerImpl
	{
public:
	virtual void CreateServerLC(TServerStartParams& aParams);	
	};
	
class MNrhServerObserver
	{
public:
	virtual void SetX3pServerObserver(MX3pHandlerNotifier* aX3pObserver) = 0;
	virtual void SetPrivacyServerObserver(MPrivacyHandlerObserver* aX3pObserver) = 0;
	};
	
class CNrhServer : public CSecureServerBase, 
				   public MNrhServerObserver,
				   public MLbsProcessCloseDown
	{
public:
	static RLbsLogger iLogger;
	
public:
	CNrhServer(TInt aPriority, const CPolicyServer::TPolicy& aSecurityPolicy);
	~CNrhServer();
	void ConstructL(const TDesC& aServerName);
	CSession2* DoNewSessionL(const TVersion &aVersion, const RMessage2& aMessage) const;
	TVersion GetServerVersion() const;
	TCustomResult CustomSecurityCheckL(const RMessage2 &aMsg, 
									   TInt& aAction, 
									   TSecurityInfo& aMissing);
	
	MX3pHandlerNotifier* X3pObserver() const;
	MPrivacyHandlerObserver* PrivacyObserver() const;
	CLbsAdmin* Admin() const;
	RLbsNetworkRegistrationStatus& NetworkRegistrationStatus();

public: // From MNrhServerObserver
	void SetX3pServerObserver(MX3pHandlerNotifier* aX3pObserver);
	void SetPrivacyServerObserver(MPrivacyHandlerObserver* aPrivacyObserver);	

public: // From MLbsProcessCloseDown
    void OnProcessCloseDown();
    
protected:
	// property
	TVersion iVersion;
private:
	CLbsNetworkRequestHandler*	iNetworkRequestHandler;
	CLbsCloseDownRequestDetector*	iCloseDownRequestDetector;
	MX3pHandlerNotifier*		iX3pObserver;
	MPrivacyHandlerObserver*	iPrivacyObserver;
	
	};
	
// this is a session WITH subsession support	
class CNrhServerSession : public CSecureSessionSubSessionBase
	{
public:
	~CNrhServerSession();
	void CreateL();
	void ServiceMessageL(const RMessage2& aMessage);
	void ServiceMessageError(const RMessage2& aMessage, const TInt aError);
	
private:
	RLbsSystemController iSystemController;
	};

#endif //__LBSNRHSERVER_H__
