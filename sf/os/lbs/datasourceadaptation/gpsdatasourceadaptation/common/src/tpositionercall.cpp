// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @InternalComponent
*/

#include <lbspositioninfo.h>
#include "tpositionercall.h"
#include "mpositioner.h"

TLocationUpdate::TLocationUpdate(TPositionInfo& aPositionInfo, TInt aError, TTime& aActualTime, TUint aMode):
	iPositionInfo(aPositionInfo),
	iError(aError),
	iActualTime(aActualTime),
	iMode(aMode)
{}

void TLocationUpdate::Call(MPositioner& client)
	{
	client.RequestCompleteNotify(iPositionInfo, iError, iActualTime);
	}
	

TUpdateFailed::TUpdateFailed(TInt aError):
	iError(aError)
{}

void TUpdateFailed::Call(MPositioner& client)
	{
	client.UpdateFailed(iError);
	}
	
TModeUpdate::TModeUpdate(TUint aMode):
	iMode(aMode)
{}

void TModeUpdate::Call(MPositioner& client)
	{
	client.ModeUpdate(iMode);
	}

TBool TActiveClients::Call(MPositioner& client)
	{
	return client.IsActive();
	}
	
TBool TWarmingDownClients::Call(MPositioner& client)
	{
	return client.IsWarmingDown();
	}

TBool TTracking::Call(MPositioner& client)
	{
	return client.IsTracking();
	}

TUint TInactivityTimeout::Call(MPositioner& client)
	{
	return client.InactivityTimeout();
	}
