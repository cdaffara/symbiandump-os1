// Copyright (c) 2000-2010 Nokia Corporation and/or its subsidiary(-ies).
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
// Utility functions for SDP
// 
//

#ifndef SDPUTIL_H
#define SDPUTIL_H

enum TSdpPanic
	{
	ESdpUnexpectedEvent,				//  0
	ESdpNotBound,						//  1
	ESdpErrorSettingAddress,			//  2
	ESdpConnectWithData,				//  3
	ESdpClientPassiveConnect,			//  4
	ESdpClientAlreadyConnected,			//  5
	ESdpClientBadIdleState,				//  6
	ESdpDisconnectWithData,				//  7
	ESdpTwoQuerys,						//  8
	ESdpNoRequestPdu,					//  9
	ESdpBadUUID,						// 10
	ESdpAgentBadResultLength,			// 11
	ESdpAgentErrorSettingNoSecurity,	// 12
	};

void Panic(TSdpPanic aPanic);

#endif
