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

#ifndef __SSMCUSTOMCMDFACTORY_H__
#define __SSMCUSTOMCMDFACTORY_H__

#include <ssm/ssmcustomcommand.h>
#include <e32std.h>
#include <e32cmn.h>

class SsmCustomCmdFactory
	{
public:
	IMPORT_C static MSsmCustomCommand* CmdSimSecurityCheckNewL();
	IMPORT_C static MSsmCustomCommand* CmdCoopSysSelfTestNewL();
	IMPORT_C static MSsmCustomCommand* CmdCoopSysPerformRestartActionsNewL();
	IMPORT_C static MSsmCustomCommand* CmdCcoopSysPerformShutdownActionsNewL();
	IMPORT_C static MSsmCustomCommand* CmdCcoopSysPerformRfsActionsNewL();
	IMPORT_C static MSsmCustomCommand* CmdActivateRfForEmergencyCallNewL();
	IMPORT_C static MSsmCustomCommand* CmdDeactivateRfForEmergencyCallNewL();
	IMPORT_C static MSsmCustomCommand* CmdCoopSysStateChangeNewL();
	IMPORT_C static MSsmCustomCommand* CmdPublishStartupModeNewL();
	IMPORT_C static MSsmCustomCommand* CmdPublishSimOwnedAndChangedNewL();
	IMPORT_C static MSsmCustomCommand* CmdDeviceSecurityCheckNewL();
	IMPORT_C static MSsmCustomCommand* CmdValidateRTCNewL();
	IMPORT_C static MSsmCustomCommand* CmdCheckUserDriveNewL();
	IMPORT_C static MSsmCustomCommand* CmdInitRamDriveNewL();
	IMPORT_C static MSsmCustomCommand* CmdClearStartupReasonNewL();
	IMPORT_C static MSsmCustomCommand* CmdDeleteTempFilesNewL();

private:
	SsmCustomCmdFactory();
};

#endif // __SSMCUSTOMCMDFACTORY_H__
