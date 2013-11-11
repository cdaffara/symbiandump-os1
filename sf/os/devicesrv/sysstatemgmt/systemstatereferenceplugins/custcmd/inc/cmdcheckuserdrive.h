/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors: Custom command for checking user drive
*
* Description:
* Declaration of CSsmCheckUserDrive class.
*
*/

#ifndef CMDCHECKUSERDRIVE_H
#define CMDCHECKUSERDRIVE_H

#include <ssm/ssmcustomcommand.h>

class CSsmCustomCommandEnv;

NONSHARABLE_CLASS(CCustomCmdCheckUserDrive) : public CBase, public MSsmCustomCommand
    {

public:
	static CCustomCmdCheckUserDrive* NewL();

	// From MSsmCustomCommand
	TInt Initialize(CSsmCustomCommandEnv* aCmdEnv);
	void Execute(const TDesC8& aParams, TRequestStatus& aRequest);
	void ExecuteCancel();
	void Close();
	void Release();

private:
	CCustomCmdCheckUserDrive();
	~CCustomCmdCheckUserDrive();
    
private:
    CSsmCustomCommandEnv* iCmdEnv;
    };

#endif // CMDCHECKUSERDRIVE_H
