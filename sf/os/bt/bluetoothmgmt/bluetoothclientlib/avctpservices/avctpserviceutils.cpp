// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**
 @file
 @internalComponent
*/

#include <bluetooth/logger.h>
#include <bluetoothav.h>
#include <e32base.h>
#include "avctpserviceutils.h"

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_AVCTP_SERVICES);
#endif

void Panic(TAvctpServicesPanic aPanic)
/**
Panic AVCTP due to stack bug.

  @internalComponent
*/
	{
	LOG_STATIC_FUNC
	_LIT(KAvctpPanicName, "AvctpServPanic");
	User::Panic(KAvctpPanicName, aPanic);
	}

TBool IsNullAddress(const TBTDevAddr& aAddr)
	{
	return (aAddr == TBTDevAddr(0));
	}
