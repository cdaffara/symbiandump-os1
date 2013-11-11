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
//

#ifndef __SSFEXAMPLEMESSAGEENUMS_H__
#define __SSFEXAMPLEMESSAGEENUMS_H__

#include "lbsmessageenums.h"

enum EExClientServerMessageEnums
	{
	// ELastSessionMessageId is KMaxInt32/2 aka 16383 (0x3FFF)
	EExTimeServerGetTimeSync = ELastSessionMessageId + 1,
	EExTimeServerGetTimeAsync = ELastSessionMessageId + 2,
	EExTimeServerCancelGetTime = ELastSessionMessageId + 3,
	EExTimeServerGetServerName = ELastSessionMessageId + 4,
	EExInVaildMessage = ELastSessionMessageId + 5
	};

#endif //__SSFEXAMLEMESSAGEENUMS_H__
