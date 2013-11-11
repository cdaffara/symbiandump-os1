// Copyright (c) 2000-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef BTSDPAGENTUTIL_H
#define BTSDPAGENTUTIL_H

#include <e32base.h>

enum TSdpAgPanic
	{
	ESdpAgentBadResultLength		= 0,
	ESdpAgentTwoRequests			= 1,
	ESdpAgRecordFilterNotSet		= 2,
	ESdpAgServiceSearchResultError	= 3,
	ESdpAgentBadStateAtCancel		= 4,
	};

void AgPanic(TSdpAgPanic aCode);

#endif
