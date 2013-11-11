// remote_console_config.cpp
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

#include <f32file.h>
#include <in_sock.h>
#include <fshell/settings.h>
#include "remote_console_config.h"

#define CLEANUP_IF_ERROR(x) ret = x; if (ret < 0) goto cleanup


// Default values (used if the config file can't be read).
_LIT(KDefaultRemoteIpAddress, "wsockhost.mrouter");
const TUint KDefaultRemotePort = 8086;
const TUint KDefaultIapId = 0;
const TUint KDefaultNetworkId = 0;

// Config file details.
_LIT(KConfigFileName, "\\system\\console\\rcons.ini");
_LIT(KConfigDesciptionFile, "\\resource\\rcons.idf");

// Configuration attributes.
_LIT(KAttConnectionType, "connection_type");
_LIT(KAttRemoteIpAddress, "remote_ip");
_LIT(KAttRemotePort, "remote_port");
_LIT(KAttProtocol, "protocol");
_LIT(KAttIapId, "iap_id");
_LIT(KAttNetId, "network_id");
_LIT(KAttDebug, "debug");
_LIT(KConTypeImplicit, "implicit");
_LIT(KConTypeExplicit, "explicit");


TRemoteConsoleConfig::TRemoteConsoleConfig()
	: iConnectionType(EImplicit), iRemotePort(KDefaultRemotePort), iRemoteHostName(KDefaultRemoteIpAddress), iProtocol(KProtocolInetTcp), iIapId(KDefaultIapId), iNetworkId(KDefaultNetworkId), iDebug(EFalse)
	{
	}

TInt TRemoteConsoleConfig::Init()
	{
	LtkUtils::CIniFile* settings = NULL;
	TRAPD(err, settings = LtkUtils::CIniFile::NewL(KConfigFileName, KConfigDesciptionFile));
	
	if (err)
		{
		return err;
		}
		
	SetConnectionType((TConnectionType)settings->GetInt(KAttConnectionType));
	SetRemoteHostName(settings->GetString(KAttRemoteIpAddress));
	SetRemotePort(settings->GetInt(KAttRemotePort));
	iProtocol = settings->GetInt(KAttProtocol);
	iIapId = settings->GetInt(KAttIapId);
	iNetworkId = settings->GetInt(KAttNetId);
	iDebug = settings->GetBool(KAttDebug);
	
	delete settings;
	return KErrNone;
	}

void TRemoteConsoleConfig::PersistL() const
	{
	LtkUtils::CIniFile* settings = LtkUtils::CIniFile::NewL(KConfigFileName, KConfigDesciptionFile);
	CleanupStack::PushL(settings);
	
	settings->SetL(KAttConnectionType, iConnectionType);
	settings->SetL(KAttRemoteIpAddress, iRemoteHostName);
	settings->SetL(KAttRemotePort, iRemotePort);
	settings->SetL(KAttProtocol, iProtocol);
	settings->SetL(KAttIapId, iIapId);
	settings->SetL(KAttNetId, iNetworkId);
	settings->SetL(KAttDebug, iDebug);
	
	settings->WriteL();
	CleanupStack::PopAndDestroy(settings);
	
	}

TRemoteConsoleConfig::TConnectionType TRemoteConsoleConfig::ConnectionType() const
	{
	return iConnectionType;
	}

const TDesC& TRemoteConsoleConfig::ConnectionTypeString() const
	{
	switch (iConnectionType)
		{
		case EImplicit:
			{
			return KConTypeImplicit;
			}
		case EExplicit:
			{
			return KConTypeExplicit;
			}
		default:
			{
			_LIT(KUnknownConnectionType, "unknown");
			return KUnknownConnectionType;
			}
		}
	}

TUint TRemoteConsoleConfig::RemotePort() const
	{
	return iRemotePort;
	}

const TDesC& TRemoteConsoleConfig::RemoteHostName() const
	{
	return iRemoteHostName;
	}

TUint TRemoteConsoleConfig::Protocol() const
	{
	return iProtocol;
	}

TUint TRemoteConsoleConfig::IapId() const
	{
	return iIapId;
	}
	
TUint TRemoteConsoleConfig::NetworkId() const
	{
	return iNetworkId;
	}

TBool TRemoteConsoleConfig::Debug() const
	{
	return iDebug;
	}

void TRemoteConsoleConfig::SetConnectionType(const TDesC& aConnectionType)
	{
	if (aConnectionType == KConTypeImplicit)
		{
		iConnectionType = EImplicit;
		}
	else if (aConnectionType == KConTypeExplicit)
		{
		iConnectionType = EExplicit;
		}
	else
		{
		iConnectionType = EImplicit;
		}
	}

void TRemoteConsoleConfig::SetConnectionType(TConnectionType aType)
	{
	iConnectionType = aType;
	}

TInt TRemoteConsoleConfig::SetRemoteHostName(const TDesC& aName)
	{
	TInt err = KErrNone;
	if (aName.Length() > iRemoteHostName.MaxLength())
		{
		err = KErrTooBig;
		iRemoteHostName = aName.Left(iRemoteHostName.MaxLength());
		}
	else
		{
		iRemoteHostName = aName;
		}
	return err;
	}

void TRemoteConsoleConfig::SetRemotePort(TUint aPort)
	{
	iRemotePort = aPort;
	}

void TRemoteConsoleConfig::SetProtocol(TUint aProtocol)
	{
	iProtocol = aProtocol;
	}

void TRemoteConsoleConfig::SetIapId(TUint aIapId)
	{
	iIapId = aIapId;
	}

void TRemoteConsoleConfig::SetNetworkId(TUint aNetworkId)
	{
	iNetworkId = aNetworkId;
	}

void TRemoteConsoleConfig::SetDebug(TBool aDebug)
	{
	iDebug = aDebug;
	}
