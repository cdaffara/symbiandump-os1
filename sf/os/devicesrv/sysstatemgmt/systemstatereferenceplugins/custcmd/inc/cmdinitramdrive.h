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
// Description: Custom command to intialise the RAM drive.
//

#ifndef CMDINITRAMDRIVE_H
#define CMDINITRAMDRIVE_H

#include <ssm/ssmcustomcommand.h>
#include <f32file.h>

class CSsmCustomCommandEnv;

NONSHARABLE_CLASS(CCustomCmdInitRamDrive) : public CBase, public MSsmCustomCommand
    {
public:
	static CCustomCmdInitRamDrive* NewL();

	// From MSsmCustomCommand
	TInt Initialize(CSsmCustomCommandEnv* aCmdEnv);
	void Execute(const TDesC8& aParams, TRequestStatus& aRequest);
	void ExecuteCancel();
	void Close();
	void Release();

private:
	CCustomCmdInitRamDrive();
	~CCustomCmdInitRamDrive();

private:
    RFs iFs; //Not owned
    };
#endif // CMDINITRAMDRIVE_H
