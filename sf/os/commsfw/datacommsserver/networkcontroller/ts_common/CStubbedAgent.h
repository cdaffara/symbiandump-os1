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

#ifndef __CSTUBBEDAGENT_H__
#define __CSTUBBEDAGENT_H__

#include <comms-infras/nifagt.h>
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <nifman_internal.h>
#endif

class TConnectionSettings;
class CStubbedAgent : public CNifAgentBase
	{
public:
	virtual ~CStubbedAgent();

	virtual void Info(TNifAgentInfo& aInfo) const;
	virtual void Connect(TAgentConnectType aType);
	virtual void Connect(TAgentConnectType aType, CStoreableOverrideSettings* aOverrideSettings);
	virtual void CancelConnect();
	virtual void Reconnect();
	virtual void CancelReconnect();
	virtual void Authenticate(TDes& aUsername, TDes& aPassword);
	virtual void CancelAuthenticate();
	virtual void Disconnect(TInt aReason);
	virtual TInt GetExcessData(TDes8& aBuffer);
	virtual TInt Notification(TNifToAgentEventType aEvent, TAny* aInfo);
	virtual TInt IncomingConnectionReceived();
	virtual void GetLastError(TInt& aError);
	virtual TBool IsActive() const;
	virtual TBool IsReconnect() const;
	virtual void SetConnectionSettingsL(const TConnectionSettings& aSettings);
	virtual TConnectionSettings& ConnectionSettingsL();
	virtual void SetOverridesL(CStoreableOverrideSettings* aOverrideSettings);
	virtual CStoreableOverrideSettings* OverridesL();

	virtual void RequestNotificationOfServiceChangeL(MAgentSessionNotify* aSession);
	virtual void CancelRequestNotificationOfServiceChange(MAgentSessionNotify* aSession);

protected:
	virtual TInt DoReadInt(const TDesC& aField, TUint32& aValue,const RMessagePtr2* aMessage);
	virtual TInt DoWriteInt(const TDesC& aField, TUint32 aValue,const RMessagePtr2* aMessage);
	virtual TInt DoReadDes(const TDesC& aField, TDes8& aValue,const RMessagePtr2* aMessage);
	virtual TInt DoWriteDes(const TDesC& aField, const TDesC8& aValue,const RMessagePtr2* aMessage);
	virtual TInt DoReadDes(const TDesC& aField, TDes16& aValue,const RMessagePtr2* aMessage);
	virtual TInt DoWriteDes(const TDesC& aField, const TDesC16& aValue,const RMessagePtr2* aMessage);
	virtual TInt DoReadBool(const TDesC& aField, TBool& aValue,const RMessagePtr2* aMessage);
	virtual TInt DoWriteBool(const TDesC& aField, TBool aValue,const RMessagePtr2* aMessage);
	virtual HBufC* DoReadLongDesLC(const TDesC& aField,const RMessagePtr2* aMessage);

	void Unimplemented() const;

private:
	TConnectionSettings iSettings;
	CStoreableOverrideSettings* iOverrides;
	};

#endif // __CSTUBBEDAGENT_H__

