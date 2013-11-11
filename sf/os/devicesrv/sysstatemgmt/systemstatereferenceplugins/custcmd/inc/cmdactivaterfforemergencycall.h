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

#ifndef __CMDACTIVATERFFOREMERGENCYCALL_H__
#define __CMDACTIVATERFFOREMERGENCYCALL_H__

#include <ssm/ssmcustomcommand.h>
#include <ssm/ssmadaptationcli.h>

NONSHARABLE_CLASS (CCustomCmdActivateRfForEmergencyCall) : public CBase, public MSsmCustomCommand
	{
public:
	static CCustomCmdActivateRfForEmergencyCall* NewL();

	// from MSsmcustomCommand
	TInt Initialize(CSsmCustomCommandEnv* aCmdEnv);
	void Execute(const TDesC8& aParams, TRequestStatus& aStatus);
	void Close();
	void Release();
	void ExecuteCancel();

private:
	CCustomCmdActivateRfForEmergencyCall();
	~CCustomCmdActivateRfForEmergencyCall();

private:
	RSsmEmergencyCallRfAdaptation iSsmEmergencyCallRfAdaptation;
	};

#endif // __CMDACTIVATERFFOREMERGENCYCALL_H__