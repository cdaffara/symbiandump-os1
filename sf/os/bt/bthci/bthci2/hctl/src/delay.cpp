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

/**
 @file
 @internalComponent
*/

#include "delay.h"

#include <e32base.h>
#include <bluetooth/logger.h>

#ifdef __FLOG_ACTIVE
_LIT8(KLogComponent, LOG_COMPONENT_HCTL_BASE);
#endif

/**
Factory function.
@param aCallback The callback function
@param aPriority The Active Object priority of the timer used for the delay.
@return Ownership of a new CDelay.
*/
EXPORT_C CDelay* CDelay::NewL(TCallBack& aCallback, TPriority aPriority)
	{
	LOG_STATIC_FUNC

	CDelay* self = new(ELeave) CDelay(aCallback, aPriority);
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
	}

CDelay::CDelay(TCallBack& aCallback, TPriority aPriority)
  : CTimer(aPriority),
	iCallback(aCallback)
	{
	LOG_FUNC

    CActiveScheduler::Add(this);
	}

EXPORT_C CDelay::~CDelay()
	{
	LOG_FUNC
	
	Cancel();
	}

void CDelay::RunL()
	{
	LOG_LINE
	LOG_FUNC
	LOG1(_L8("\tiStatus = %d"), iStatus.Int());

	// Ignore any return value from the callback function.
	(void)(iCallback.CallBack());
	}
