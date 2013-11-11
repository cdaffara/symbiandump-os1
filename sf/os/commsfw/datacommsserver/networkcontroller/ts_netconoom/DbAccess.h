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

#if !defined(__DBACCESS_H__)
#define __DBACCESS_H__

#include <etelmm.h>
#include <comms-infras/connectionsettings.h>

class CCommDbOverrideSettings;
class MServiceChangeObserver;

class CCommsDbAccess : public CBase
	{
public:
	~CCommsDbAccess();
	static CCommsDbAccess* NewL();
	static CCommsDbAccess* NewL(TBool);


	TInt GetConnectionAttempts();
	void GetBearerAvailabilityTsyNameL(TDes& aTsyName);
	void GetCurrentSettingsL(TConnectionSettings& aSettings, TCommDbConnectionDirection aDirection, TUint32 aRank);
	void SetCurrentSettingsL(const TConnectionSettings& aSettings);
	TBool IsTelNumLengthZeroForRasConnectionL(TConnectionSettings& aSettings);
	void GetServiceSettingsL(TConnectionSettings& aSettings);
	RMobilePhone::TMobilePhoneNetworkMode NetworkMode() const;
	void SetNetworkMode(RMobilePhone::TMobilePhoneNetworkMode aMode);
	void GetDesL(const TDesC& aTable, const TDesC& aField, TDes16& aValue);
	void GetIntL(const TDesC& aTable, const TDesC& aField, TUint32& aValue);
	void SetOverridesL(CCommDbOverrideSettings* aOverrides);
	void GetPreferedIapL(TUint32& aIapId, TCommDbConnectionDirection aDirection, TUint32 aRank);
	TBool DoesIapExistL(TUint32 aIapId);
	void GetTsyNameL(TDes& aTsyName);

	void RequestNotificationOfServiceChangeL(MServiceChangeObserver* aObserver);
	void CancelRequestNotificationOfServiceChange(MServiceChangeObserver* aObserver);



protected:
	CCommsDbAccess();
	};

#endif // __DBACCESS_H__

