// remote_console_config.h
// 
// Copyright (c) 2005 - 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//


#include <e32std.h>
#include <in_sock.h>
#ifdef USE_RCONNECTION
#include <es_enum.h>
#endif


class TRemoteConsoleConfig
	{
public:
	enum TConnectionType
		{
		EImplicit,
		EExplicit
		};
public:
	TRemoteConsoleConfig();
	TInt Init();
	void PersistL() const;
	TConnectionType ConnectionType() const;
	const TDesC& ConnectionTypeString() const;
	TUint RemotePort() const;
	const TDesC& RemoteHostName() const;
	TUint Protocol() const;
	TUint IapId() const;
	TUint NetworkId() const;
	TBool Debug() const;
	void SetConnectionType(const TDesC& aConnectionType);
	void SetConnectionType(TConnectionType aType);
	void SetRemotePort(TUint aPort);
	TInt SetRemoteHostName(const TDesC& aName);
	void SetProtocol(TUint aProtocol);
	void SetIapId(TUint aIapId);
	void SetNetworkId(TUint aNetworkId);
	void SetDebug(TBool aDebug);
private:
	TConnectionType iConnectionType;
	TUint iRemotePort;
	TName iRemoteHostName;
	TUint iProtocol;
	TUint iIapId;
	TUint iNetworkId;
	TBool iDebug;
	};

