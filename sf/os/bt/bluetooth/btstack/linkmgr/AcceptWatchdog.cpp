// Copyright (c) 2003-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Implements the accept watchdog timer for baseband SAPs
// 
//

#include <bluetooth/logger.h>
#include "AcceptWatchdog.h"
#include "basebandsap.h"
#include "btsockettimer.h"

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_HCI_WATCHDOG);
#endif

#ifdef _DEBUG
PANICCATEGORY("HciWatch");
#endif

TAcceptWatchdog::TAcceptWatchdog(CBTBasebandSAP& aParent)
: iParent(aParent), iActive(EFalse)
	{
	TCallBack cb(AcceptTimeout, this);
	iIdleTimerEntry.Set(cb);
	}

void TAcceptWatchdog::Start()
	{
	BTSocketTimer::Queue(KAcceptWatchdogTimeout, iIdleTimerEntry);
	// add entry to timer queue
	iActive = ETrue;
	}

void TAcceptWatchdog::Cancel()
	{
	if (iActive)
		{
		BTSocketTimer::Remove(iIdleTimerEntry);
		iActive = EFalse;
		}
	}

/*static*/ TInt TAcceptWatchdog::AcceptTimeout(TAny* aTAcceptWatchdog)
	{
	TAcceptWatchdog* w = reinterpret_cast<TAcceptWatchdog*>(aTAcceptWatchdog);

	ASSERT_DEBUG(w);
	w->iActive = EFalse;
	w->iParent.Timeout(EAccept);

	return EFalse;
	}


#ifdef _DEBUG
TAcceptWatchdog::~TAcceptWatchdog()
	{
	ASSERT_DEBUG(!iActive);
	}
#endif
