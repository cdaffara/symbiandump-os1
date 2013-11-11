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
// Description:
//

/**
 @file
 @internalComponent
 @released
*/

#ifndef CMDCLEARSTARTUPREASON_H
#define CMDCLEARSTARTUPREASON_H

#include <ssm/ssmcustomcommand.h>
//#include <f32file.h>

class CSsmCustomCommandEnv;
class CRepository;

NONSHARABLE_CLASS(CCustomCmdClearStartupReason) : public CBase, public MSsmCustomCommand
    {
public:
	static CCustomCmdClearStartupReason* NewL();

	// From MSsmCustomCommand
	TInt Initialize(CSsmCustomCommandEnv* aCmdEnv);
	void Execute(const TDesC8& aParams, TRequestStatus& aRequest);
	void ExecuteCancel();
	void Close();
	void Release();

private:
	CCustomCmdClearStartupReason();
	~CCustomCmdClearStartupReason();

private:
    CRepository* iRepository;
    };
#endif // CMDCLEARSTARTUPREASON_H
