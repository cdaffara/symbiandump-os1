// Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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
 @test
 @internalComponent
*/

#include "tgraphicsresourceinternalsecondprocessenums.h"
#include "tsgimagegeneric.h"
#include "tsgdrawablegeneric.h"
#include "tgraphicsresourceinternalsecondprocesstesthandler.h"

TInt MainL()
	{
	TInt procHandles1 = 0;
	TInt threadHandles1 = 0;
	RThread().HandleCount(procHandles1, threadHandles1);
	
	TPckgBuf<TSgResIntTestInfo> infoPkg;
	User::LeaveIfError(User::GetDesParameter(KSecondProcessParametersSlot, infoPkg));
	TSgResIntTestInfo& info = infoPkg();
	TInt result = 0;
	
	CTSgResInternalSecondProcessTestHandler* handler = CTSgResInternalSecondProcessTestHandler::NewLC();
	handler->OpenDriverL();

	result = handler->RunTestCaseL(info);
	CleanupStack::PopAndDestroy(handler);

	// Handle check
	if (!(result < KErrNone))
		{
		TInt procHandles2  =0;
		TInt threadHandles2=0;
		RThread().HandleCount(procHandles2,threadHandles2);
		if (threadHandles1 != threadHandles2)
			{
			result = KErrBadHandle;  // Thread-owned handles not closed
			}
		}

	return result;
	}

GLDEF_C TInt E32Main()
	{
	__UHEAP_MARK;
	CTrapCleanup* cleanupStack = CTrapCleanup::New();
	if(cleanupStack == NULL)
		{
			return KErrNoMemory;
		}
	TInt result = 0;
	TRAPD(ret, result=MainL());
	
	if (ret != KErrNone)
		User::Panic(_L("SGRESTEST"), 0);
	
	delete cleanupStack;
	__UHEAP_MARKEND;
	return result;
	}
