// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// RawIPMCPR.CPP
// RawIP MCPR
// 
//

/**
 @file
 @internalComponent
*/

#include <comms-infras/nifprvar.h>			// for TAgentToFlowEventType
#include <comms-infras/linkprovision.h>
#include "RawIpAgentHandler.h"

using namespace ESock;

void CRawIpAgentHandler::ConnectCompleteL()
/**
RawIp handler called on Agent ConnectComplete().

Allocate and populate the provisioning structure.
*/
	{
	// If not already poresent, allocate and append the provisioning structure.  Assumed to be
	// cleaned up by CAccessPointConfig.

	if (iProvision == NULL)
		{
		CRawIpAgentConfig* provision = new (ELeave) CRawIpAgentConfig();
		CleanupStack::PushL(provision);
		AppendExtensionL(provision);
		CleanupStack::Pop(provision);			// ownership given to CAccessPointConfig
	
		iProvision = provision;					// cache pointer to provisioning structure (no ownership)
		}
	
	// The following calls towards the Agent will trigger Notification() calls from Agent back
	// towards us (in the same stack frame).  These will be processed in NotificationFromAgent().
	NotificationToAgent(EFlowToAgentEventTsyConfig, NULL);
	NotificationToAgent(EFlowToAgentEventTsyConnectionSpeed, NULL);
	}
	
TInt CRawIpAgentHandler::NotificationFromAgent(TAgentToFlowEventType aEvent, TAny* aInfo)
/**
RawIp handler called on Agent Notification() calls towards CFProtocol.

@param aEvent event type
@param aInfo free form data associated with event
*/
	{
	switch (aEvent)
		{
	case EAgentToFlowEventTsyConfig:
		// NOTE:
		// We are presuming that the object pointed to by "aInfo" is on the heap of the Agent and
		// not on the stack.  This sounds risky, but we are following the same conditions as
		// with old Nifs and Agents - the Notification() method was used to communicate a pointer
		// from Agent to Nif of a structure whose lifetime was guaranteed longer than that of the
		// Nif.  This hasn't changed with the introduction of CFProtocols.  Alternatively, we would
		// have allocate space for and copy the (very large - several K) TContextConfigGPRS.
		iProvision->iGprsConfig = reinterpret_cast<const TPacketDataConfigBase*>(aInfo);
		break;
		
	case EAgentToFlowEventTsyConnectionSpeed:
		if (aInfo != NULL)
			iProvision->iConnectionSpeed = *reinterpret_cast<TUint*>(aInfo);
		break;
		
	default:
		// Default handling
		return CAgentNotificationHandler::NotificationFromAgent(aEvent, aInfo);
		}

	return KErrNone;
	}
