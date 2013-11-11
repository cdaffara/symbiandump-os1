// Copyright (c) 2001-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <power.h>
#include <e32std.h>
#include <hal.h>
#include <e32svr.h>


/**
Bring the machine out of 'silent running' mode
Switch on the User IO devices
*/
EXPORT_C void PowerManager::SwitchOn()
	{
	// We tell if we are silent running by checking the screen state
	TInt on=0;
	TInt r=HAL::Get(HAL::EDisplayState, on);
	r=r;
	__ASSERT_DEBUG(r==KErrNone, User::Panic(_L("PWRCLI"), EPowerManagerCantGetDisplayState));
	if (!on)
		{
		// This lets E32 know that user switch on has occured
		// It goes to kernel PowerModel first, then WSERV,
		// to turn on the screen, and finally to LafPowerMon
		TRawEvent switchon;
		switchon.Set(TRawEvent::ESwitchOn);
		UserSvr::AddEvent(switchon);
		}
	}
