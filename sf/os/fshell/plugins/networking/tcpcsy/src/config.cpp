// config.cpp
// 
// Copyright (c) 2008 - 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//

#include "panic.h"
#include "tcpcsy.h"
#include "config.h"


const TUid KRepositoryUid = { 0x102836da };
const TInt KModeField =			0x00000000;
const TInt KFamilyField =		0x00010000;
const TInt KProtocolField =		0x00020000;
const TInt KIpAddressField =	0x00030000;
const TInt KPortField =			0x00040000;


CConfig* CConfig::NewL(CTcpPortFactory& aFactory)
	{
	CConfig* self = new(ELeave) CConfig(aFactory);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CConfig::~CConfig()
	{
	delete iRepository;
	}

CConfig::TMode CConfig::ModeL(const TUint aUnit) const
	{
	TInt mode = ReadIntL(KModeField, aUnit);
	if ((mode < EActive) || (mode > EPassive))
		{
		User::Leave(KErrCorrupt);
		}
	return static_cast<TMode>(mode);
	}

TUint CConfig::ProtocolFamilyL(const TUint aUnit) const
	{
	return static_cast<TUint>(ReadIntL(KFamilyField, aUnit));
	}

TUint CConfig::ProtocolL(const TUint aUnit) const
	{
	return static_cast<TUint>(ReadIntL(KProtocolField, aUnit));
	}

void CConfig::GetAddressL(const TUint aUnit, TInetAddr& aAddress) const
	{
	TBuf<64> ip;
	ReadStringL(KIpAddressField, aUnit, ip);
	TInt err = aAddress.Input(ip);
	if (err)
		{
		User::Leave(KErrCorrupt);
		}
	TUint port = static_cast<TUint>(ReadIntL(KPortField, aUnit));
	aAddress.SetPort(port);
	TPtr8 narrowIp = ip.Collapse();
	LOG("Unit: %u, IP: \"%S\", Port: %u", aUnit, &narrowIp, port);
	}

void CConfig::UnitRange(TUint& aLowUnit, TUint& aHighUnit) const
	{
	TInt i = 0;
	TInt val;
	while (iRepository->Get(KModeField + i, val) != KErrNone)
		{
		++i;
		}
	aLowUnit = i;
	while (iRepository->Get(KModeField + i, val) == KErrNone)
		{
		++i;
		}
	aHighUnit = Max(0, i - 1);
	}

CConfig::CConfig(CTcpPortFactory& aFactory)
	: iFactory(aFactory)
	{
	}

void CConfig::ConstructL()
	{
	iRepository = CRepository::NewL(KRepositoryUid);
	}

TInt CConfig::ReadIntL(TInt aFieldId, const TUint aUnit) const
	{
	if (aUnit > KMaxTUint16)
		{
		User::Leave(KErrArgument);
		}
	TInt val;
	TInt err = iRepository->Get(aFieldId + aUnit, val);
	if (err == KErrNotFound)
		{
		err = KErrNotSupported;
		}
	User::LeaveIfError(err);
	return val;
	}

void CConfig::ReadStringL(TInt aFieldId, const TUint aUnit, TDes& aString) const
	{
	if (aUnit > KMaxTUint16)
		{
		User::Leave(KErrArgument);
		}
	TInt err = iRepository->Get(aFieldId + aUnit, aString);
	if (err == KErrNotFound)
		{
		err = KErrNotSupported;
		}
	User::LeaveIfError(err);
	}
