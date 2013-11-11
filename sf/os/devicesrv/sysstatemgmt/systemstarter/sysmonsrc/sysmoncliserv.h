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
//

#if !defined (__SYSMONCLISERV_H__)
#define __SYSMONCLISERV_H__

#include <e32std.h>

/** Defines the name of the Sysmon critical server
@internalComponent
*/
_LIT(KSysMonServerName,"!SysMon");


/** Defines the message types processed by the Sysmon server
@internalComponent
*/
enum TSysMonMessages
	{
	EMonitor = 0,
	EMonitorSelf,
	ECancelSelf,
	EMaxSysMonMessage
	};

#endif // __SYSMONCLISERV_H__
