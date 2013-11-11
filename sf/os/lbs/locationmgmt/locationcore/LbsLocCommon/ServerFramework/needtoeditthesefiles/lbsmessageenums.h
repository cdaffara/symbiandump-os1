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
// Extend the enum list as required for each server - see restrictions
// butDO NOT rename the file
// 
//

#ifndef __LBSMESSAGEENUMS_H__
#define __LBSMESSAGEENUMS_H__


// NOTE that the enumerated values in this file are now available in LBS\inc\lbsmessageenums.h
// (both client and server must include it when using the Server Framework)

// RESTRICTIONS to new enumerated values for messages IDs in client-server that uses the server framework:
//
// 1) The values of ESecureSubSessionBaseOpen and ESecureSubSessionBaseClose MUST remain 0 and 1 respectively
// 2) ANY session messages MUST have values smaller than ELastSessionMessageId
// 3) NONE OF ESecureSubSessionBaseOpen, ESecureSubSessionBaseClose, ELastSessionMessageId can be renamed
// 4) NONE OF ESecureSubSessionBaseOpen, ESecureSubSessionBaseClose, ELastSessionMessageId should be missing UNLESS
// you are certain you are NOT using subsessions. i.e. NOT deriving from CSecureSessionSubSessionBase
// 5) ALL subsession messages must come AFTER ELastSessionMessageId
// 6) Remember that you have to pad the CPolicyServer::TPolicy for any enums added in addition to those below
//
enum EClientServerMessageEnums
	{
	ESecureSubSessionBaseOpen = 0,
	ESecureSubSessionBaseClose = 1,
	// insert your session enums here
	ELastSessionMessageId = KMaxTInt32 >> 1, // aka KMaxTInt32/2
	// insert you subsession enums here. Note the trailing comma above...
	ELastValidSubSessionMessageId = 3 // MUST be last - see the TPolicy
	};

#endif // __LBSMESSAGEENUMS_H__
