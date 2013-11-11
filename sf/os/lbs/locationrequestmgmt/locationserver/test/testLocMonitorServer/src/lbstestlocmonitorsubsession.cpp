/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
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


#include <e32std.h>
#include <s32mem.h>
#include <lbspositioninfo.h>
#include <lbssatellite.h>

#include "lbstestlocmonitorsubsession.h"

#include "lbstestlocmonitorsession.h"
#include "lbstestlocmonitorserver.h"

CTe_LocMonitorServerSubSession* CTe_LocMonitorServerSubSession::NewL()
	{
	CTe_LocMonitorServerSubSession* self = new (ELeave) CTe_LocMonitorServerSubSession;
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop();
	return self;
	}

CTe_LocMonitorServerSubSession::CTe_LocMonitorServerSubSession()
	{
	}

void CTe_LocMonitorServerSubSession::ConstructL()
	{

	}

CTe_LocMonitorServerSubSession::~CTe_LocMonitorServerSubSession()
	{

	}

void CTe_LocMonitorServerSubSession::ServiceL(const RMessage2& /*aMessage*/)
	{

	}


