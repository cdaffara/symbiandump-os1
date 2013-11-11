/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/
/**
@file
@internalTechnology
*/


#ifndef __LEGACY_LOOPBACK_AGENT_HANDLER_H__
#define __LEGACY_LOOPBACK_AGENT_HANDLER_H__

#include <comms-infras/agentmessages.h>

NONSHARABLE_CLASS(CLegacyLoopbackProtoAgentHandler) : public CAgentNotificationHandler
	{
public:
	virtual void ConnectCompleteL();
	};

#endif // __LEGACY_LOOPBACK_AGENT_HANDLER_H__
