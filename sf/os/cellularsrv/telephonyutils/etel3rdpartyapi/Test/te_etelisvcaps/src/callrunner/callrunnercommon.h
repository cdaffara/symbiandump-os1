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
// This file define all the common values thoughout your test project
// 
//

/**
 @file 
 @internalTechnology
*/
#ifndef __CALL_RUNNER_COMMON_H__
#define __CALL_RUNNER_COMMON_H__

TUint KCallRunnerPropertyKey = 1;

// The call runner property UID is set to the same value as its process SID
TUid  KCallRunnerPropertyUid = {0x102827D8};

// Call runner notification values
enum TCallRunnerNotifications
	{
	ECallRunnerStarted = 0x80,
	ECallRunnerCloseDown
	};


#endif //__CALL_RUNNER_COMMON_H__
