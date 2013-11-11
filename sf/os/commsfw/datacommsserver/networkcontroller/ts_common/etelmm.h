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

#ifndef __ETELMM_H__
#define __ETELMM_H__

#include "etel.h"
#include "etelmmcs.h"

class RMobilePhone : public RPhone
	{
public:
	enum TMobilePhoneModeCaps
		{
		KCapsGsmSupported=0x00000001,
		KCapsGprsSupported=0x00000002,
		KCapsAmpsSupported=0x00000004
		};

	TInt GetMultimodeCaps(TUint32& aCaps) const;

	enum TMobilePhoneNetworkMode
		{
		ENetworkModeUnknown,
		ENetworkModeUnregistered,
		ENetworkModeGsm,
		ENetworkModeAmps
		};

	TInt GetCurrentMode(TMobilePhoneNetworkMode& aNetworkMode) const;

	void GetSignalStrength(TRequestStatus& aReqStatus, TInt32& aSignalStrength, TInt8& aBar) const;
	void NotifySignalStrengthChange(TRequestStatus& aReqStatus, TInt32& aSignalStrength, TInt8& aBar) const;
	};

#endif // __ETELMM_H__

