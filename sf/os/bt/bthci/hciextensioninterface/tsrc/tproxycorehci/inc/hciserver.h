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
*/

#ifndef HCISERVER_H
#define HCISERVER_H

#include <e32base.h>

class CHCISession;
class CHCIServerRequestManager;
class CBTPowerControlManager;
class CHCIDirectAccessManager;
class CCoreHCIPluginImpl;

/**
The HCI server class.
*/
NONSHARABLE_CLASS(CHCIServer) : public CPolicyServer
	{
public:
	static CHCIServer* NewL(CCoreHCIPluginImpl& aParent);
	~CHCIServer();
	CHCIDirectAccessManager& DirectAccessManager();
	CBTPowerControlManager& PowerControlManager();
	
	CHCIServerRequestManager* ConnectToServiceL(TInt aInterfaceUid);
	void ReleaseService(CHCIServerRequestManager* aServiceToRelease);

private:
	CHCIServer(CCoreHCIPluginImpl& aParent);
	void ConstructL();

	// from CPolicyServer
	virtual CSession2* NewSessionL(const TVersion &aVersion, const RMessage2& aMessage) const;

private:
	enum TRequestManagerLocks
		{
		// Bitmasks
		EBTPowerControlService = 1,
		EHCIDirectAccessService = 2,
		};

private:
	CCoreHCIPluginImpl& iParent;
	CBTPowerControlManager* iBTPowerControlManager;
	CHCIDirectAccessManager* iHCIDirectAccessManager;
	TUint8 iRequestManagerLocks;
	};

#endif // HCISERVER_H
