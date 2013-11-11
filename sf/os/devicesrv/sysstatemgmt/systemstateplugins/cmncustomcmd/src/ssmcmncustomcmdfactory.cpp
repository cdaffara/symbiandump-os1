// Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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
 @released
*/

#include "ssmcmncustomcmdfactory.h"
#include "cmdrendezvouswithestart.h"
#include "cmdcancelmonitoring.h"

EXPORT_C MSsmCustomCommand* SsmCmnCustomCmdFactory::CmdRendezvousWithEStartL()
	{
	CCustomCmdRendezvousWithEStart* self = CCustomCmdRendezvousWithEStart::NewL();
	return self;
	}

EXPORT_C MSsmCustomCommand* SsmCmnCustomCmdFactory::CmdCancelMonitoringL()
    {
    CCustomCmdCancelMonitoring* self = CCustomCmdCancelMonitoring::NewL();
    return self;
    }


