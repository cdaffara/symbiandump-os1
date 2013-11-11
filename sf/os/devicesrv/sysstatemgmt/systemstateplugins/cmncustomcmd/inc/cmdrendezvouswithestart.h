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

#ifndef __CMDRENDEZVOUSWITHESTART_H__
#define __CMDRENDEZVOUSWITHESTART_H__

#include <ssm/ssmcustomcommand.h>

NONSHARABLE_CLASS (CCustomCmdRendezvousWithEStart) : public CBase, public MSsmCustomCommand
	{
public:
	static CCustomCmdRendezvousWithEStart* NewL();

	// From MSsmCustomCommand
	TInt Initialize(CSsmCustomCommandEnv* aCmdEnv);
	void Execute(const TDesC8& aParams, TRequestStatus& aStatus);
	void Close();
	void Release();
	void ExecuteCancel();
	
private:
	CCustomCmdRendezvousWithEStart();
	~CCustomCmdRendezvousWithEStart();
	};

#endif // __CMDRENDEZVOUSWITHESTART_H__
