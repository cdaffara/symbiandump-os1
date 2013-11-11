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
// The message enums/opcodes shared between the client and the server sides
// This file defines enums specific to the NRH server, on top of the
// generic server framework enums in lbsmessageenums.h
// The NRH supports two types of subsession: a privacy controller client, 
// and a X3P client. This is why there are two groups of message IDs in
// the enum below.
// 
//

/**
 @file
 @internalComponent
 @released
*/

#ifndef __LBSNRHMESSAGEENUMS_H__
#define __LBSNRHMESSAGEENUMS_H__

#include "lbsmessageenums.h"

enum ENrhClientServerMessageEnums
	{
	// ELastSessionMessageId is KMaxInt32/2 aka 16383 (0x3FFFFFFF)
	
	// Start of IDs for IPC messages for privacy controller client subsessions.
	//
	// Any new privacy controller message IDs should be added between 
	// ENrhPrivacyControllerSubSessionFirstMessageId and 
	// ENrhPrivacyControllerSubSessionLastMessageId.
	ENrhPrivacyControllerSubSessionFirstMessageId =	0x40000000, // (ELastSessionMessageId + 1)
	
	ENrhPrivacyControllerRegister = 				ENrhPrivacyControllerSubSessionFirstMessageId,
	ENrhPrivacyControllerCancelRegister =			0x40000001,
	ENrhPrivacyControllerResponse =					0x40000002,
	ENrhPrivacyControllerCancel = 					0x40000003,
	
	ENrhPrivacyControllerSubSessionLastMessageId,
	
	// Start of IDs for IPC messages for X3P client subsessions.
	//
	// Any new privacy controller message IDs should be added between 
	// ENrhPrivacyControllerSubSessionFirstMessageId and 
	// ENrhPrivacyControllerSubSessionLastMessageId.
	EX3pSubSessionFirstMessageId = 					0x40001000, // Leave space for 4096 NrhPrivacyController messages
	
	EX3pTransmitPosition =							EX3pSubSessionFirstMessageId,
	EX3pCancelTransmitPosition = 					0x40001001,
	EX3pSetTransmitOptions = 						0x40001002,
	EX3pGetTransmitOptions = 						0x40001003,
	EX3pReportReferenceLocation =					0x40001004,
	
	EX3pSubSessionLastMessageId
	};
	
#endif //#define __LBSNRHMESSAGEENUMS_H__
	