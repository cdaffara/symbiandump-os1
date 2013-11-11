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
// the message enums/opcodes shared between the client and the server sides
//
// NOTE: This file should not be modified or used by any new applications 
//

/**
 @file
 @publishedAll
 @released
*/

#ifndef __LBSMESSAGEENUMS_H__
#define __LBSMESSAGEENUMS_H__


enum EClientServerMessageEnums
	{
	ESecureSubSessionBaseOpen = 0,
	ESecureSubSessionBaseClose = 1,
	// insert your session enums here
	ELastSessionMessageId = KMaxTInt32 >> 1, // aka KMaxTInt32/2
	ELastValidSubSessionMessageId = KMaxTInt32// MUST be last - see the TPolicy
	};


// old version
/*
enum EClientServerMessageEnums
	{
	ESecureSubSessionBaseOpen = 0,
	ESecureSubSessionBaseClose = 1,
	// insert your session enums here
	ELastSessionMessageId = 2,
	// insert you subsession enums here. Note the trailing comma above...
	ELbsPosNotifyPositionUpdate = 3,
	ELbsPosNotifyPositionUpdateCancel = 4,
	ELbsSetRequestor = 5,
	ELbsSetRequestorStack = 6,
	ELbsSetUpdateOptions = 7,
	ELbsGetUpdateOptions = 8,
	ELbsGetLastKnownPosition = 9,
	ELbsGetLastKnownPositionCancel = 10,
	ENrhPrivacyControllerRegister = 11, 
	ENrhPrivacyControllerCancelRegister = 12,
	ENrhPrivacyControllerResponse = 13,
	ENrhPrivacyControllerCancel = 14,
	EX3pTransmitPosition = 15,
	EX3pCancelTransmitPosition = 16,
	EX3pSetTransmitOptions = 17,
	EX3pGetTransmitOptions = 18,
	// Owen - this is a hack to be removed very very soon!
	EExTimeServerGetTimeSync = 19,
	EExTimeServerGetTimeAsync = 20,
	EExTimeServerCancelGetTime = 21,
	EExTimeServerGetServerName = 22,
	EExInVaildMessage = 23,
	ELastValidSubSessionMessageId = 24// MUST be last - see the TPolicy
	};
*/

#endif // __LBSMESSAGEENUMS_H__
