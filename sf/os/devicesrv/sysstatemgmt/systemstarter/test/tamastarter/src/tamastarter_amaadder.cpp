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
// NOTE: This aplication is run in conjunction with SSC test.
// The aim of this application is to create the appropriate database at the beginning of the startup sequence, so
// when the startup sequence finds START_AMA_STARTER commands, these are in the ddbb.
// 
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code 
*/


#include <e32debug.h>
#include "tamastarter_database_utils.h"


//----------------------


void AmaAddL()
	{
	// Create DBMS / SQL data store backups and delete original data stores
	DscDatabaseBackupL();
	RDebug::Print(_L("*** AmaAddL(): Backup of original data stores completed"));

	// Create appropriaate data store for the tests
	CreateTestDatabaseL();
	RDebug::Print(_L("*** AmaAddL(): Test database creation completed"));
	}


TInt E32Main()
	{
	__UHEAP_MARK; // mark heap state

	CTrapCleanup* theTrapCleanup = CTrapCleanup::New();
	if (theTrapCleanup == NULL)
		{
		return KErrNoMemory;
		}
		
	TRAPD(err, AmaAddL());
	if (err)
		{
		_LIT(KMainErrorStatement, "*** AmaAddL() - Error: %d");
		RDebug::Print(KMainErrorStatement, err);
	   	User::Panic(KMainErrorStatement,err);
		}
		
    delete theTrapCleanup;

	__UHEAP_MARKEND;
	return KErrNone;
	}
