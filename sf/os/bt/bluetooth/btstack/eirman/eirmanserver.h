// Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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
*/

#ifndef _EIRMANSERVER_H
#define _EIRMANSERVER_H

#include <e32base.h>

class CEirManSession;
class CEirManInternalSession;
class CEirManager;
class MHCICommandQueue;
class CLinkMgrProtocol;
class MEirInternalSessionNotifier;

enum TEirFeatureState
	{
	EEirFeaturePending = 0,
	EEirFeatureReady = 1,
	EEirFeatureNotSupported = 2,
	};

/**
The EIR Manager server class.
*/
NONSHARABLE_CLASS(CEirManServer) : public CPolicyServer
	{
public:
	static CEirManServer* NewL(MHCICommandQueue& aCommandQueue, CLinkMgrProtocol& aLinkMgrProtocol);
	~CEirManServer();
	
	CEirManInternalSession* NewInternalSessionL(MEirInternalSessionNotifier& aParent);
	
	inline CEirManager& EirManager() const;
	
	void AddSession(CEirManSession& aSession, TBool aInternalSession);
	void DropSession(TBool aInternalSession);
	void NotifyFeaturesReady();
	TEirFeatureState EirFeatureState();

private:
	CEirManServer(MHCICommandQueue& aCommandQueue, CLinkMgrProtocol& aLinkMgrProtocol);
	void ConstructL();

	// from CPolicyServer
	virtual CSession2* NewSessionL(const TVersion &aVersion, const RMessage2& aMessage) const;
	TCustomResult CustomSecurityCheckL(const RMessage2& aMsg, TInt& aAction, TSecurityInfo& aMissing);
	
private:
	// unowned
	MHCICommandQueue&	iCommandQueue;
	CLinkMgrProtocol& 	iLinkMgrProtocol;
	//owned
	CEirManager* 		iEirManager;
	TInt				iInternalSessionCount;
	TInt				iExternalSessionCount;
	TBool				iIsFeaturesReady;
	TBool				iIsEirSupported;
	TDblQue<CEirManSession> iSessions;
	};
#include "eirmanserver.inl"

#endif // _EIRMANSERVER_H

