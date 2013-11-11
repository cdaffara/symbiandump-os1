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
// Description: This command is used to issue a request to sysmon server during device shutdown to cancel all the outstanding monitors
//

/**
 @file
 @internalComponent
 @released
*/

#ifndef __CMDCANCELMONITORING_H__
#define __CMDCANCELMONITORING_H___

#include <ssm/ssmcustomcommand.h>
#include <sysmonclisess.h> 

NONSHARABLE_CLASS (CCustomCmdCancelMonitoring) : public CBase, public MSsmCustomCommand
	{
public:
	static CCustomCmdCancelMonitoring* NewL();

	// From MSsmCustomCommand
	TInt Initialize(CSsmCustomCommandEnv* aCmdEnv);
	void Execute(const TDesC8& aParams, TRequestStatus& aStatus);
	void Close();
	void Release();
	void ExecuteCancel();
	
private:
	CCustomCmdCancelMonitoring();
	~CCustomCmdCancelMonitoring();

private:
    RSysMonSession iSysMonSession;
	};

#endif // __CMDCANCELALLMONITORS_H__
