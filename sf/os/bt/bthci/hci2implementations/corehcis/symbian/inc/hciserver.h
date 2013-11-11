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
class CBluetoothDutModeManager;
class CA2dpOptimiserManager;
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
	CBluetoothDutModeManager& DutModeManager();
	CA2dpOptimiserManager& A2dpOptimiserManager();
	
	CHCIServerRequestManager* ConnectToServiceL(TInt aInterfaceUid);
	void ReleaseService(CHCIServerRequestManager* aServiceToRelease);

	void OpenClientReference();
	void CloseClientReference();
	
private:
	CHCIServer(CCoreHCIPluginImpl& aParent);
	void ConstructL();

	// from CPolicyServer
	virtual CSession2* NewSessionL(const TVersion &aVersion, const RMessage2& aMessage) const;

private:
	enum TRequestManagerLocks
		{
		// Bitmasks
		EBTPowerControlService = 0x01,
		EHCIDirectAccessService = 0x02,
		EBluetoothDutModeService = 0x04,
		EA2dpOptimiserService = 0x08,
		};

private:
	CCoreHCIPluginImpl& iParent;
	CBTPowerControlManager* iBTPowerControlManager;
	CHCIDirectAccessManager* iHCIDirectAccessManager;
	CBluetoothDutModeManager* iBluetoothDutModeManager;
	CA2dpOptimiserManager* iA2dpOptimiserManager;

	TUint8 iRequestManagerLocks;
	};

#endif // HCISERVER_H
