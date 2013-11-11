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
 @internalComponent
 @released
*/

#ifndef __SSMCMNCUSTOMCMDFACTORY_H__
#define __SSMCMNCUSTOMCMDFACTORY_H__

#include <ssm/ssmcustomcommand.h>
#include <e32std.h>
#include <e32cmn.h>

class SsmCmnCustomCmdFactory
	{
public:
	IMPORT_C static MSsmCustomCommand* CmdRendezvousWithEStartL();
	IMPORT_C static MSsmCustomCommand* CmdPersistRebootsNewL();
	IMPORT_C static MSsmCustomCommand* CmdCancelMonitoringL();
private:
	SsmCmnCustomCmdFactory();
};

#endif // __SSMCMNCUSTOMCMDFACTORY_H__
