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


#include <e32std.h>
#include "tsysstart2_teststep.h"
#include "testprocgoodsession.h"


TVerdict CAppFwkSysStart2TestStep::doTestStepPostambleL()
	{

 	return TestStepResult();
	}



TBool CAppFwkSysStart2TestStep::ProcessExists( const TDesC& aProcessName, TFullName& aName )
	{
	TBool found = EFalse;
	TFullName searchTerm(aProcessName);
	searchTerm += _L("*");
	TFindProcess find(searchTerm);
	TFullName name;
	while(find.Next(aName)==KErrNone)
		{
		RProcess process;
		const TInt err = process.Open(find);
		if (err == KErrNone)
			{
			if (process.ExitType() == EExitPending)
				{
				found = ETrue;
				process.Close(); 
				break;
				}
			process.Close(); 
			}
		}
	return found;
	}
	
	
	
void CAppFwkSysStart2TestStep::LogonAndCancelMonL()
	{
	RTestProcGoodSession session;
	
	User::LeaveIfError( session.Connect() );
	User::LeaveIfError( session.CancelMonitor() );
	
	session.Close();
	}
