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

#ifndef __LBSSUPLPROXYSERVERMSGENUMS_H__
#define __LBSSUPLPROXYSERVERMSGENUMS_H__

#include <e32std.h>
#include "lbsprivacyservermsgenums.h"

// extension API identifiers.
const TInt KLbsSuplProxyPosAPIVersionMajor = 0;
const TInt KLbsSuplProxyPosAPIVersionMinor = 2;
const TInt KLbsSuplProxyPosAPIVersionBuild = 0;

// privacy message enums
enum ELbsSuplProxyServerMsgEnums
	{
	// positioning op codes
    ELbsSuplProxyServerPosOpen = ELbsPrivacyServerNextAvailableMsgEnum,
    ELbsSuplProxyServerPosClose,
    ELbsSuplProxyServerPosResetRequestId,
    ELbsSuplProxyServerGetPosition,
    ELbsSuplProxyServerCancelGetPosition,
    ELbsSuplProxyServerPosPayload,
	ELbsSuplProxyServerPosNotifySessionComplete,
    ELbsSuplProxyServerGetSuplInfo,
    ELbsSuplProxyServerCancelGetSuplInfo,	
    ELbsSuplProxyServerSetPosMethod,
    ELbsSuplProxyServerSetSlpAddress
	};

#endif // __LBSSUPLPROXYSERVERMSGENUMS_H__
