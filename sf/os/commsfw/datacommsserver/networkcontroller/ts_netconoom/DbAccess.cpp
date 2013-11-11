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

#include "DbAccess.h"
#include <cdbover.h>
#include <cdbcols.h>

CCommsDbAccess* CCommsDbAccess::NewL()
	{
	return CCommsDbAccess::NewL(EFalse);
	}

CCommsDbAccess* CCommsDbAccess::NewL(TBool)
	{
	CCommsDbAccess* self = new(ELeave)CCommsDbAccess();
	return self;
	}

CCommsDbAccess::CCommsDbAccess()
	{
	}

CCommsDbAccess::~CCommsDbAccess()
	{
	}

TInt CCommsDbAccess::GetConnectionAttempts()
	{

	return 2;
	}


void CCommsDbAccess::GetBearerAvailabilityTsyNameL(TDes& aTsyName)
	{
	GetTsyNameL(aTsyName);
	}

void CCommsDbAccess::GetTsyNameL(TDes& aTsyName)
	{
	_LIT(KTsyName, "mm");
	aTsyName = KTsyName();
	}

void CCommsDbAccess::GetCurrentSettingsL(TConnectionSettings& aSettings, TCommDbConnectionDirection aDirection, TUint32)
	{

	aSettings.iRank = 1;
	aSettings.iDirection = aDirection;
	aSettings.iDialogPref = ECommDbDialogPrefPrompt; //ECommDbDialogPrefDoNotPrompt;
	aSettings.iBearerSet = KCommDbBearerCSD;
	aSettings.iIAPId = 1;
	aSettings.iServiceId = 1;
	aSettings.iServiceType.Copy(TPtrC(DIAL_OUT_ISP));
	aSettings.iBearerId = 1;
	aSettings.iLocationId = 1;
	aSettings.iChargeCardId = 0;
	}

void CCommsDbAccess::SetCurrentSettingsL(const TConnectionSettings&)
	{
	}

TBool CCommsDbAccess::IsTelNumLengthZeroForRasConnectionL(TConnectionSettings&)
	{

	return EFalse; //ETrue;
	}

void CCommsDbAccess::GetServiceSettingsL(TConnectionSettings&)
	{
	}

RMobilePhone::TMobilePhoneNetworkMode CCommsDbAccess::NetworkMode() const
	{
	return RMobilePhone::ENetworkModeUnknown;
	}

void CCommsDbAccess::SetNetworkMode(RMobilePhone::TMobilePhoneNetworkMode)
	{
	}

void CCommsDbAccess::GetDesL(const TDesC&, const TDesC& aField, TDes16& aValue)
	{

	if(aField == TPtrC(AGENT_NAME))
		{
		_LIT(KAgentName, "StubbedAgent");
		aValue = KAgentName();
		}
	else
		{
		User::Invariant();
		}
	}

void CCommsDbAccess::GetIntL(const TDesC& aTable, const TDesC& aField, TUint32& aValue)
	{

	if (aTable == TPtrC(IAP) && aField == TPtrC(COMMDB_ID))
		{
		aValue = 1;
		}
	else if(aTable == TPtrC(IAP) && aField == TPtrC(IAP_NETWORK))
		{
		aValue = 1;
		}
	else if (aTable == TPtrC(MODEM_BEARER) && aField == TPtrC(MODEM_MIN_SIGNAL_LEVEL))
		{
		aValue = 10500;
		}
	else
		{
		User::Invariant();
		}
	}

void CCommsDbAccess::SetOverridesL(CCommDbOverrideSettings*)
	{
	}

void CCommsDbAccess::GetPreferedIapL(TUint32& aIapId, TCommDbConnectionDirection, TUint32)
	{
	aIapId = 1;
	}

TBool CCommsDbAccess::DoesIapExistL(TUint32)
	{
	return ETrue;
	}

void CCommsDbAccess::RequestNotificationOfServiceChangeL(MServiceChangeObserver*)
	{
	}

void CCommsDbAccess::CancelRequestNotificationOfServiceChange(MServiceChangeObserver*)
	{
	}

