// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <remcon/remcontargetselectorpluginobserver.h>
#include "tspv4stub.h"
#include "utils.h"

#include <bluetooth/logger.h>

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_REMCON_SERVER);
#endif

TTspV4Stub::TTspV4Stub(MRemConTargetSelectorPluginObserver& aObserver)
	: iObserver(aObserver)
	{
	LOG_FUNC
	}

void TTspV4Stub::PermitIncomingCommand(
	TUid /*aInterfaceUid*/,
	TUint /*aOperationId*/, 
	const TClientInfo& /*aClient*/)
	{
	LOG_FUNC
	iObserver.IncomingCommandPermitted(ETrue);
	}

void TTspV4Stub::PermitIncomingNotify(
	TUid /*aInterfaceUid*/,
	TUint /*aOperationId*/, 
	const TClientInfo& /*aClient*/)
	{
	LOG_FUNC
	iObserver.IncomingNotifyPermitted(ETrue);
	}

void TTspV4Stub::SetRemoteAddressedClient(const TUid& /*aBearerUid*/, 
		const TClientInfo& /*aClient*/)
	{
	LOG_FUNC
	// Ignore this, we are stateless
	}
	
	
