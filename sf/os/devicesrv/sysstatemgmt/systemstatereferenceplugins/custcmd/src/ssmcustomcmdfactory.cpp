// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "ssmcustomcmdfactory.h"
#include "cmdsimsecuritycheck.h"
#include "cmddevicesecuritycheck.h"
#include "cmdcoopsysselftest.h"
#include "cmdcoopsysperformrestartactions.h"
#include "cmdcoopsysperformshutdownactions.h"
#include "cmdcoopsysperformrfsactions.h"
#include "cmdactivaterfforemergencycall.h"
#include "cmddeactivaterfforemergencycall.h"
#include "cmdcoopsysstatechange.h"
#include "cmdpublishstartupmode.h"
#include "cmdpublishsimownedandchanged.h"
#include "cmdvalidatertc.h"
#include "cmdcheckuserdrive.h"
#include "cmdinitramdrive.h"
#include "cmdclearstartupreason.h"
#include "cmddeletetempfiles.h"

EXPORT_C MSsmCustomCommand* SsmCustomCmdFactory::CmdSimSecurityCheckNewL()
	{
	CCustomCmdSimSecurityCheck* self = CCustomCmdSimSecurityCheck::NewL();
	return self;
	}

EXPORT_C MSsmCustomCommand* SsmCustomCmdFactory::CmdCoopSysSelfTestNewL()
	{
	CCustomCmdCoopSysSelfTest* self = CCustomCmdCoopSysSelfTest::NewL();
	return self;
	}

EXPORT_C MSsmCustomCommand* SsmCustomCmdFactory::CmdCoopSysPerformRestartActionsNewL()
	{
	CCustomCmdCoopSysPerformRestartActions* self = CCustomCmdCoopSysPerformRestartActions::NewL();
	return self;
	}

EXPORT_C MSsmCustomCommand* SsmCustomCmdFactory::CmdCcoopSysPerformShutdownActionsNewL()
	{
	CCustomCmdCoopSysPerformShutdownActions* self = CCustomCmdCoopSysPerformShutdownActions::NewL();
	return self;
	}

EXPORT_C MSsmCustomCommand* SsmCustomCmdFactory::CmdCcoopSysPerformRfsActionsNewL()
	{
	CCustomCmdCoopSysPerformRfsActions* self = CCustomCmdCoopSysPerformRfsActions::NewL();
	return self;
	}

EXPORT_C MSsmCustomCommand* SsmCustomCmdFactory::CmdActivateRfForEmergencyCallNewL()
	{
	CCustomCmdActivateRfForEmergencyCall* self = CCustomCmdActivateRfForEmergencyCall::NewL();
	return self;
	}

EXPORT_C MSsmCustomCommand* SsmCustomCmdFactory::CmdDeactivateRfForEmergencyCallNewL()
	{
	CCustomCmdDeactivateRfForEmergencyCall* self = CCustomCmdDeactivateRfForEmergencyCall::NewL();
	return self;
	}

EXPORT_C MSsmCustomCommand* SsmCustomCmdFactory::CmdCoopSysStateChangeNewL()
	{
	CCustomCmdCoopSysStateChange* self = CCustomCmdCoopSysStateChange::NewL();
	return self;
	}

EXPORT_C MSsmCustomCommand* SsmCustomCmdFactory::CmdPublishStartupModeNewL()
	{
	CCustomCmdPublishStartupMode* self = CCustomCmdPublishStartupMode::NewL();
	return self;
	}

EXPORT_C MSsmCustomCommand* SsmCustomCmdFactory::CmdPublishSimOwnedAndChangedNewL()
	{
	CCustomCmdPublishSimOwnedAndChanged* self = CCustomCmdPublishSimOwnedAndChanged::NewL();
	return self;
	}

EXPORT_C MSsmCustomCommand* SsmCustomCmdFactory::CmdDeviceSecurityCheckNewL()
	{
	CCustomCmdDeviceSecurityCheck* self = CCustomCmdDeviceSecurityCheck::NewL();
	return self;
	}

EXPORT_C MSsmCustomCommand* SsmCustomCmdFactory::CmdValidateRTCNewL()
    {
    CCustomCmdValidateRTC* self = CCustomCmdValidateRTC::NewL();
    return self;
    }

EXPORT_C MSsmCustomCommand* SsmCustomCmdFactory::CmdCheckUserDriveNewL()
    {
    CCustomCmdCheckUserDrive* self = CCustomCmdCheckUserDrive::NewL();
	return self;
    }

EXPORT_C MSsmCustomCommand* SsmCustomCmdFactory::CmdInitRamDriveNewL()
    {
    CCustomCmdInitRamDrive* self = CCustomCmdInitRamDrive::NewL();
    return self;
    }

EXPORT_C MSsmCustomCommand* SsmCustomCmdFactory::CmdClearStartupReasonNewL()
    {
    CCustomCmdClearStartupReason* self = CCustomCmdClearStartupReason::NewL();
    return self;
    }

EXPORT_C MSsmCustomCommand* SsmCustomCmdFactory::CmdDeleteTempFilesNewL()
    {
    CCustomCmdDeleteTempFiles* self = CCustomCmdDeleteTempFiles::NewL();
    return self;
    }
