// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef __LBSPRIVACYSERVERMSGENUMS_H__
#define __LBSPRIVACYSERVERMSGENUMS_H__

#include <e32std.h>

const TInt KLbsLocalPrivacyAPIVersionMajor = 0;
const TInt KLbsLocalPrivacyAPIVersionMinor = 0;
const TInt KLbsLocalPrivacyAPIVersionBuild = 0;

const TInt KLbsNetworkPrivacyAPIVersionMajor = 0;
const TInt KLbsNetworkPrivacyAPIVersionMinor = 1;
const TInt KLbsNetworkPrivacyAPIVersionBuild = 0;

// Type of Proxy API
enum TLbsProxyApiType
{
	ELbsProxyApiTypeUnknown = 0,
	ELbsProxyApiTypeLocal,
	ELbsProxyApiTypeNetwork,
	ELbsProxyApiTypePos
};

// message enums
enum ELbsPrivacyServerMsgEnums
	{
	ELbsPrivacyServerConnect = 1,
	ELbsPrivacyServerDisconnect,
	ELbsPrivacyServerNotifyLocation,
	ELbsPrivacyServerVerifyLocation,
	ELbsPrivacyServerNotifyVerificationTimeout,
	ELbsPrivacyServerCancelVerifyLocationRequest,
	ELbsPrivacyServerNewPrivacyRequestLocal, // = 7
	ELbsPrivacyServerRepeatPrivacyRequestLocal,
	ELbsPrivacyServerCompleteRequest,
	ELbsPrivacyServerNewPrivacyRequestNetwork, // = 10
	ELbsPrivacyServerRepeatPrivacyRequestNetwork,

	ELbsPrivacyServerNextAvailableMsgEnum	// for continuation by extended servers.
	};

#endif // __LBSPRIVACYSERVERMSGENUMS_H__
