// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Provides utility functions for Bluetooth Manager server.

#include <e32base.h>
#include <d32dbms.h>
#include "btmanserverutil.h"

#include <bluetooth/logger.h>

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_BT_MANAGER_SERVER);
#endif

void RDbRowSetCleanup(TAny* aResource)
/**
	Called on CleanupStack::PopAndDestroy when a heap RDbRowSet needs cleaning up
**/
	{
	LOG_STATIC_FUNC
	RDbRowSet* rowset = reinterpret_cast<RDbRowSet*>(aResource);
	if (rowset)
		{
		rowset->Close();
		}
	delete rowset;
	}

void CleanupCloseDeletePushL(TAny* aAny)
	{
	LOG_STATIC_FUNC
	CleanupStack::PushL(TCleanupItem(RDbRowSetCleanup, aAny));
	}
