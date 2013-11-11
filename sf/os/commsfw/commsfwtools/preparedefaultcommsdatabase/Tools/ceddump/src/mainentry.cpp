// Copyright (c) 2004-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// Contains the global definitions needed across the ceddump module/exe
//
//

/**
 @file
 @internalComponent
*/

#include "ceddumper.h"

LOCAL_C void doMainL()
	{
	CCedDumper* cedDumper=NULL;
	TRAPD(error, cedDumper=CCedDumper::NewL());
	CleanupStack::PushL(cedDumper);
	if (error == KErrNone)
		{
		error = cedDumper->ProcessCommandLineL();
		if (error == KErrNone)
			{
			cedDumper->DumpContents();
			}
		}
	CleanupStack::Pop(cedDumper);
	delete cedDumper;
	}

TInt E32Main()
	{
	__UHEAP_MARK;

	CTrapCleanup* cleanup=CTrapCleanup::New();

	TRAPD(error,doMainL());

	__ASSERT_ALWAYS(!error, User::Panic(_L("CEDDUMP PANIC"), error));

	delete cleanup;

	__UHEAP_MARKEND;

	return KErrNone;
	}

