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

#include "restartsys.h"
#include <e32svr.h>
#include <hal.h>
#include <hal_data.h>
#include <e32modes.h>

#include "SysStartDebug.h"
#include "sysstartpanic.h"

/** Safely restarts the system in a given startup mode. 
@post This function will return after posting an event in system event queue. The processing of this event might 
take some time and even might be cancelled if the system cannot be restarted safely. The execution of the calling 
thread will continue for an indefinte period of time after this call, but it has no guarantees to finish execution.
It is strongly advised to delay for a sufficient period of time to let the shutdown request be processed.
@code
// Usage example to restart the system in licensee-defined mode 3
	RestartSys::RestartSystem(3);
	User::After(5000000);
@endcode 
@param aStartupMode The startup mode the device will start in after reboot 
@panic ERestartNotSupportedOnEmulator if this function is called on emulator
@capability PowerMgmt
@capability WriteDeviceData
@capability SwEvent
@return KErrNone, if successful; KErrArgument if aStartupMode is out of the valid range; KErrPermissionDenied, if the caller has insufficient capability; otherwise, one of the other system-wide error codes.
*/
EXPORT_C TInt RestartSys::RestartSystem(TInt aStartupMode)
	{
	(void) aStartupMode;
	DEBUGPRINT2(_L("RestartSys::RestartSystem called with aStartupMode=%d"), aStartupMode);

#ifdef __WINS__ // Restart using event is not supported on emulator because Base change is needed
	PanicNow(KPanicRestartSys, ERestartNotSupportedOnEmulator);
	return KErrNotSupported;
#else
	TInt err = HAL::Set(HALData::EPersistStartupModeKernel, aStartupMode);
	if (err!=KErrNone)
		{
		DEBUGPRINT2(_L("RestartSys::HAL::Set return err=%d"), err);
		return err;
		}

	TRawEvent event;
	event.Set(TRawEvent::ERestartSystem);
	return UserSvr::AddEvent(event);
#endif
	}


/** Safely restarts the system in the default startup mode (EStartupModeUndefined).
@capability PowerMgmt
@capability SwEvent
@see RestartSys::RestartSystem(TInt aStartupMode)
*/
EXPORT_C TInt RestartSys::RestartSystem()
	{
	return RestartSystem(EStartupModeUndefined);
	}

