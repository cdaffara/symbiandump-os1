// Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "CStubbedAgent.h"

void CStubbedAgent::Info(TNifAgentInfo& aInfo) const
	{

	_LIT(KAgentName, "Stubbed");
	aInfo.iName = KAgentName();
	aInfo.iName.AppendFormat(_L("-Agent[0x%08x]"), this);
	}

CStubbedAgent::~CStubbedAgent()
	{

	if(iOverrides)
		{
		delete iOverrides;
		}
	}

void CStubbedAgent::Unimplemented() const
	{
	User::Invariant();
	}


void CStubbedAgent::Connect(TAgentConnectType)
	{

	Unimplemented();
	}

void CStubbedAgent::Connect(TAgentConnectType, CStoreableOverrideSettings*)
	{

	Unimplemented();
	}

void CStubbedAgent::CancelConnect()
	{

	Unimplemented();
	}

void CStubbedAgent::Reconnect()
	{

	Unimplemented();
	}

void CStubbedAgent::CancelReconnect()
	{

	Unimplemented();
	}

void CStubbedAgent::Authenticate(TDes&, TDes&)
	{
	
	Unimplemented();
	}

void CStubbedAgent::CancelAuthenticate()
	{

	Unimplemented();
	}

void CStubbedAgent::Disconnect(TInt)
	{

	Unimplemented();
	}

TInt CStubbedAgent::GetExcessData(TDes8&)
	{

	Unimplemented();
	return KErrNone;
	}

TInt CStubbedAgent::DoReadInt(const TDesC& aField, TUint32& aValue,const RMessagePtr2* /*aMessage*/)
	{
	TInt ret(KErrNotFound);

	_LIT(KIapIdField, "Iap\\Id");
	if(aField.CompareF(KIapIdField()) == 0)
		{
		aValue = 1;
		ret = KErrNone;
		}
	else
		{
		Unimplemented();
		}

	return ret;
	}

TInt CStubbedAgent::DoWriteInt(const TDesC&, TUint32,const RMessagePtr2* /*aMessage*/)

	{

	Unimplemented();
	return KErrNone;
	}

TInt CStubbedAgent::DoReadDes(const TDesC&, TDes8&,const RMessagePtr2* /*aMessage*/)
	{

	Unimplemented();
	return KErrNone;
	}

TInt CStubbedAgent::DoWriteDes(const TDesC&, const TDesC8&,const RMessagePtr2* /*aMessage*/)
	{

	Unimplemented();
	return KErrNone;
	}

TInt CStubbedAgent::DoReadDes(const TDesC&, TDes16&,const RMessagePtr2* /*aMessage*/)
	{

	Unimplemented();
	return KErrNone;
	}

TInt CStubbedAgent::DoWriteDes(const TDesC&, const TDesC16&,const RMessagePtr2* /*aMessage*/)
	{

	Unimplemented();
	return KErrNone;
	}

TInt CStubbedAgent::DoReadBool(const TDesC&, TBool&,const RMessagePtr2* /*aMessage*/)
	{

	Unimplemented();
	return KErrNone;
	}

TInt CStubbedAgent::DoWriteBool(const TDesC&, TBool,const RMessagePtr2* /*aMessage*/)
	{

	Unimplemented();
	return KErrNone;
	}

HBufC* CStubbedAgent::DoReadLongDesLC(const TDesC&,const RMessagePtr2* /*aMessage*/)
	{

	Unimplemented();
	return NULL;
	}

TInt CStubbedAgent::Notification(TNifToAgentEventType, TAny*)
	{

	Unimplemented();
	return KErrNone;
	}

TInt CStubbedAgent::IncomingConnectionReceived()
	{

	Unimplemented();
	return KErrNone;
	}

void CStubbedAgent::GetLastError(TInt&)
	{

	Unimplemented();
	}

TBool CStubbedAgent::IsActive() const
	{

	Unimplemented();
	return EFalse;
	}

TBool CStubbedAgent::IsReconnect() const
	{

	Unimplemented();
	return EFalse;
	}

void CStubbedAgent::SetConnectionSettingsL(const TConnectionSettings& aSettings)
	{

	iSettings = aSettings;
	}

TConnectionSettings& CStubbedAgent::ConnectionSettingsL()
	{

	return iSettings;
	}

void CStubbedAgent::SetOverridesL(CStoreableOverrideSettings* aOverrides)
	{

	iOverrides = aOverrides;
	}

CStoreableOverrideSettings* CStubbedAgent::OverridesL()
	{

	return iOverrides;
	}

void CStubbedAgent::RequestNotificationOfServiceChangeL(MAgentSessionNotify*)
	{

	Unimplemented();
	}

void CStubbedAgent::CancelRequestNotificationOfServiceChange(MAgentSessionNotify*)
	{

	Unimplemented();
	}

