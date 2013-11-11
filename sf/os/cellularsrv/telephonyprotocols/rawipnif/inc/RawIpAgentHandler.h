/**
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:
* Header file declaring the RawIP Provisioning structures.
* 
*
*/



/**
 @file
 @internalTechnology
*/

#ifndef RAWIPAGENTHANDLER_H_
#define RAWIPAGENTHANDLER_H_

#include <comms-infras/agentmessages.h>
#include <comms-infras/linkprovision.h>
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <comms-infras/nifprvar_internal.h>
#endif

class CRawIpAgentConfig;

NONSHARABLE_CLASS(CRawIpAgentHandler) : public CAgentNotificationHandler
	{
private:
	virtual void ConnectCompleteL();
	virtual TInt NotificationFromAgent(TAgentToFlowEventType aEvent, TAny* aInfo);

private:
	CRawIpAgentConfig* iProvision;		// pointer to provisioning structure to populate (ownership ultimately
										// given to AccessPointConfig, so do not clean up on destruction)
	};

#endif
// RAWIPAGENTHANDLER_H_
