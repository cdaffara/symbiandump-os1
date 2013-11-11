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
// NOTE: This is just an abstraction of common funtionality for the test to create
// the appropriate database.
// 
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code 
*/


#include <e32debug.h>
#include "appfwk_test_utils.h"
#include "tamastarter_database_utils.h"


//----------------------

void DscDatabaseBackupL()
	{
	// Create DBMS / SQL data store backups and delete original data stores
	//		(if failures are reported, this may indicate that the ddbb didn't previously exist - should not be a problem)
	RSmlTestUtils fs;
	CleanupClosePushL(fs);
	User::LeaveIfError(fs.Connect());

	// DBMS ddbb
	RDebug::Print(_L("*** DscDatabaseBackupL(): Create DBMS data store backups and delete original data store"));
	TBool fileExists = EFalse;
	fs.IsFilePresent(KDbmsDscDdbbOriginal, fileExists);
	if(fileExists)
		{
		User::LeaveIfError(fs.CopyFileL(KDbmsDscDdbbOriginal, KDbmsDscDdbbBackup));
		User::LeaveIfError(fs.DeleteFileL(KDbmsDscDdbbOriginal));
		}
		
	// SQL ddbb
	RDebug::Print(_L("*** DscDatabaseBackupL(): Create SQL data store backups and delete original data store\n"));
	fileExists = EFalse;
	fs.IsFilePresent(KSqlDscDdbbOriginal, fileExists);
	if(fileExists)
		{
		User::LeaveIfError(fs.CopyFileL(KSqlDscDdbbOriginal, KSqlDscDdbbBackup));
		User::LeaveIfError(fs.DeleteFileL(KSqlDscDdbbOriginal));
		}

	CleanupStack::PopAndDestroy(&fs);
	}

void DscDatabaseRestoreL()
	{
	// Restore DBMS / SQL data store backups available prior to running the test
	//		(if failures are reported, this may indicate that the ddbb didn't previously exist - should not be a problem)

	RSmlTestUtils fs;
	CleanupClosePushL(fs);
	User::LeaveIfError(fs.Connect());

	// DBMS ddbb
	RDebug::Print(_L("*** DscDatabaseBackupL(): Restore DBMS data store backup available prior to running the test"));
	TBool fileExists = EFalse;
	fs.IsFilePresent(KDbmsDscDdbbOriginal, fileExists);
	if(fileExists)
		{
		User::LeaveIfError(fs.DeleteFileL(KDbmsDscDdbbOriginal));
		}
	fs.IsFilePresent(KDbmsDscDdbbBackup, fileExists);
	if(fileExists)
		{
		User::LeaveIfError(fs.CopyFileL(KDbmsDscDdbbBackup, KDbmsDscDdbbOriginal));
		User::LeaveIfError(fs.DeleteFileL(KDbmsDscDdbbBackup));
		}

	// SQL ddbb
	RDebug::Print(_L("*** DscDatabaseBackupL(): Restore SQL data store backup available prior to running the test"));
	fileExists = EFalse;
	fs.IsFilePresent(KSqlDscDdbbOriginal, fileExists);
	if(fileExists)
		{
		User::LeaveIfError(fs.DeleteFileL(KSqlDscDdbbOriginal));
		}
	fs.IsFilePresent(KSqlDscDdbbBackup, fileExists);
	if(fileExists)
		{
		User::LeaveIfError(fs.CopyFileL(KSqlDscDdbbBackup, KSqlDscDdbbOriginal));
		User::LeaveIfError(fs.DeleteFileL(KSqlDscDdbbBackup));
		}

	CleanupStack::PopAndDestroy(&fs);
	}


void CreateDscItemL(RDscStore& aStore, TUid aDscUid, const TDesC& aAmaFilename, const TDesC& aAmaArguments, TStartupType aStartupType, TStartMethod aStartMethod, TNoOfRetries aNoOfRetries, TInt aTimeout, TBool aIsMonitored, TBool aIsStartInBackground, TBool aIsViewless, TDscPosition aDscPosition)
	{
	CDscItem* dscItem = CDscItem::NewL();
	CleanupStack::PushL(dscItem);
	
	dscItem->SetDscId(aDscUid);
	dscItem->SetFileParamsL(aAmaFilename, aAmaArguments);
	dscItem->SetStartupType(TStartupType(aStartupType));
	dscItem->SetStartMethodL(TStartMethod(aStartMethod));
	dscItem->SetNoOfRetriesL(aNoOfRetries);
	dscItem->SetTimeoutL(aTimeout);			
	dscItem->SetMonitored(aIsMonitored);
	dscItem->SetStartInBackground(aIsStartInBackground);
	dscItem->SetViewless(aIsViewless);

	// Insert the Dsc item in the position specified
	TRAPD(ret, aStore.AddItemL(*dscItem, TDscPosition(aDscPosition)));
	RDebug::Print(_L("*** CreateDscItemL(): Adding single item to DSC database finished with value '%d'"), ret);
	User::LeaveIfError(ret);

	CleanupStack::PopAndDestroy(dscItem);
	}


void CreateTestDatabaseL()
	{
	// Create appropriate data store for the tests
	RDscStore store;
	CleanupClosePushL(store);

	// Connecting to DSC database
	store.OpenL();
	RDebug::Print(_L("*** CreateTestDatabaseL(): Connected to DSC database"));

	// Add entries needed for test case 1
	RDebug::Print(_L("*** CreateTestDatabaseL(): Add entries needed for test case 1:"));
	store.CreateDscL(TUid::Uid(0x10000011), KNullDesC);
	RDebug::Print(_L("*** CreateTestDatabaseL(): DSC for 0x10000011 created"));
	CreateDscItemL(store, TUid::Uid(0x10000011), KAMCFileNameApp1, _L("-Log tamastarter_result_1 -Re 0"), EStartProcess, EWaitForStart, ERetry0, 5000, EFalse, EFalse, EFalse, ELast);
	CreateDscItemL(store, TUid::Uid(0x10000011), KAMCFileNameApp2, _L("-Log tamastarter_result_1 -Re 0 -T 7000 -R 1"), EStartApp, EWaitForStart, ERetry1, 5000, EFalse, ETrue, ETrue, ELast);
	CreateDscItemL(store, TUid::Uid(0x10000011), KAMCFileNameApp3, _L("-Log tamastarter_result_1"), EStartProcess, EFireAndForget, ERetry0, 5000, EFalse, ETrue, EFalse, ELast);

	// Add entries needed for test case 2
	RDebug::Print(_L("*** CreateTestDatabaseL(): Add entries needed for test case 2:"));
	store.CreateDscL(TUid::Uid(0x10000012), KNullDesC);
	RDebug::Print(_L("*** CreateTestDatabaseL(): DSC for 0x10000012 created"));
	//CIniData used to create the log files isn't threadsafe, so we can't start 2 processes with 
	//EFireAndForget and let them use the same logfile. Instead we let each process have its own logfile.
	CreateDscItemL(store, TUid::Uid(0x10000012), KAMCFileNameApp1, _L("-Log tamastarter_result_2_1 -Re 0 -T 7000 -R 1"), EStartProcess, EWaitForStart, ERetry0, 5000, EFalse, EFalse, EFalse, ELast);
	CreateDscItemL(store, TUid::Uid(0x10000012), KAMCFileNameApp2, _L("-Log tamastarter_result_2_2 -Re 0"), EStartApp, EWaitForStart, ERetry0, 5000, EFalse, ETrue, ETrue, ELast);
	CreateDscItemL(store, TUid::Uid(0x10000012), KAMCFileNameApp3, _L("-Log tamastarter_result_2_3"), EStartProcess, EFireAndForget, ERetry0, 5000, EFalse, ETrue, EFalse, ELast);
	CreateDscItemL(store, TUid::Uid(0x10000012), KAMCFileNameApp4, _L("-Log tamastarter_result_2_4"), EStartProcess, EFireAndForget, ERetry0, 5000, EFalse, ETrue, EFalse, ELast);

	// Add entries needed for test case 3
	RDebug::Print(_L("*** CreateTestDatabaseL(): Add entries needed for test case 3:"));
	store.CreateDscL(TUid::Uid(0x10000013), KNullDesC);
	RDebug::Print(_L("*** CreateTestDatabaseL(): DSC for 0x10000013 created"));
	//CIniData used to create the log files isn't threadsafe, so we can't start 2 processes with 
	//EFireAndForget and let them use the same logfile. Instead we let each process have its own logfile.
	CreateDscItemL(store, TUid::Uid(0x10000013), KAMCFileNameApp1, _L("-Log tamastarter_result_3_1 -P -1000"), EStartProcess, EWaitForStart, ERetry1, 5000, EFalse, EFalse, EFalse, ELast);
	CreateDscItemL(store, TUid::Uid(0x10000013), KAMCFileNameApp2, _L("-Log tamastarter_result_3_2 -Re 0"), EStartApp, EWaitForStart, ERetry0, 5000, EFalse, ETrue, ETrue, ELast);
	CreateDscItemL(store, TUid::Uid(0x10000013), KAMCFileNameApp3, _L("-Log tamastarter_result_3_3"), EStartProcess, EFireAndForget, ERetry0, 5000, EFalse, ETrue, EFalse, ELast);
	CreateDscItemL(store, TUid::Uid(0x10000013), KAMCFileNameApp4, _L("-Log tamastarter_result_3_4"), EStartProcess, EFireAndForget, ERetry0, 5000, EFalse, ETrue, EFalse, ELast);
	CreateDscItemL(store, TUid::Uid(0x10000013), KAMCFileNameNonExistantApp, _L("-IMInvalidArgumentDoingNothing -NeverUsed"), EStartProcess, EFireAndForget, ERetry0, 5000, EFalse, ETrue, EFalse, ELast);
	CreateDscItemL(store, TUid::Uid(0x10000013), KAMCFileNameAppWillNeverRun, _L("-IMInvalidArgumentDoingNothing -NeverUsed"), EStartApp, EWaitForStart, ERetry0, 5000, EFalse, ETrue, ETrue, ELast);

	// Add entries needed for test case 4
	RDebug::Print(_L("*** CreateTestDatabaseL(): Add entries needed for test case 4:"));
	store.CreateDscL(TUid::Uid(0x10000014), KNullDesC);
	RDebug::Print(_L("*** CreateTestDatabaseL(): DSC for 0x10000014 created"));
	store.CreateDscL(TUid::Uid(0x10000024), KNullDesC);
	RDebug::Print(_L("*** CreateTestDatabaseL(): DSC for 0x10000024 created"));
	store.CreateDscL(TUid::Uid(KDefaultSymbianDsc), KNullDesC);
	RDebug::Print(_L("*** CreateTestDatabaseL(): DSC for KDefaultSymbianDsc created"));
	CreateDscItemL(store, TUid::Uid(0x10000014), KAMCFileNameApp1, _L("-Log tamastarter_result_4 -RE 0"), EStartProcess, EWaitForStart, ERetry1, 5000, EFalse, EFalse, EFalse, ELast);
	CreateDscItemL(store, TUid::Uid(0x10000024), KAMCFileNameApp1, _L("-Log tamastarter_result_4 -RE 0"), EStartProcess, EWaitForStart, ERetry1, 5000, EFalse, EFalse, EFalse, ELast);
	CreateDscItemL(store, TUid::Uid(KDefaultSymbianDsc), KAMCFileNameApp1, _L("-Log tamastarter_result_4 -RE 0"), EStartProcess, EWaitForStart, ERetry1, 5000, EFalse, EFalse, EFalse, ELast);

	// Add entries needed for test case 5
	RDebug::Print(_L("*** CreateTestDatabaseL(): Add entries needed for test case 5:"));
	store.CreateDscL(TUid::Uid(0x10000015), KNullDesC);
	RDebug::Print(_L("*** CreateTestDatabaseL(): DSC for 0x10000015 created"));
	//CIniData used to create the log files isn't threadsafe, so we can't start 2 processes with 
	//EFireAndForget and let them use the same logfile. Instead we let each process have its own logfile.
	CreateDscItemL(store, TUid::Uid(0x10000015), KAMCFileNameApp1, _L("-Log tamastarter_result_5_1 -Re 0"), EStartProcess, EWaitForStart, ERetry0, 5000, ETrue, EFalse, EFalse, ELast);
	CreateDscItemL(store, TUid::Uid(0x10000015), KAMCFileNameApp2, _L("-Log tamastarter_result_5_2 -Re 0"), EStartApp, EWaitForStart, ERetry0, 5000, ETrue, ETrue, ETrue, ELast);
	CreateDscItemL(store, TUid::Uid(0x10000015), KAMCFileNameApp3, _L("-Log tamastarter_result_5_3"), EStartProcess, EFireAndForget, ERetry0, 5000, ETrue, ETrue, EFalse, ELast);
	CreateDscItemL(store, TUid::Uid(0x10000015), KAMCFileNameApp4, _L("-Log tamastarter_result_5_4"), EStartApp, EFireAndForget, ERetry0, 5000, ETrue, EFalse, EFalse, ELast);

	// Add entries needed for test case 6
	RDebug::Print(_L("*** CreateTestDatabaseL(): Add entries needed for test case 6:"));
	store.CreateDscL(TUid::Uid(0x10000016), KNullDesC);
	RDebug::Print(_L("*** CreateTestDatabaseL(): DSC for 0x10000016 created"));
	store.CreateDscL(TUid::Uid(0x10000026), KNullDesC);
	RDebug::Print(_L("*** CreateTestDatabaseL(): DSC for 0x10000026 created"));
	store.CreateDscL(TUid::Uid(0x10000036), KNullDesC);
	RDebug::Print(_L("*** CreateTestDatabaseL(): DSC for 0x10000036 created"));
	CreateDscItemL(store, TUid::Uid(0x10000016), KAMCFileNameApp1, _L("-Log tamastarter_result_6 -Re 0"), EStartProcess, EWaitForStart, ERetry0, 5000, ETrue, EFalse, EFalse, ELast);
	CreateDscItemL(store, TUid::Uid(0x10000026), KAMCFileNameApp1, _L("-Log tamastarter_result_6 -Re 0"), EStartProcess, EWaitForStart, ERetry0, 5000, ETrue, EFalse, EFalse, ELast);
	CreateDscItemL(store, TUid::Uid(0x10000036), KAMCFileNameApp1, _L("-Log tamastarter_result_6 -Re 0"), EStartProcess, EFireAndForget, ERetry0, 5000, ETrue, ETrue, EFalse, ELast);

#ifdef SYMBIAN_SYSTEM_STATE_MANAGEMENT
	// Add entries needed for test case 6
	RDebug::Print(_L("*** CreateTestDatabaseL(): Add entries needed for test case 6:"));
	store.CreateDscL(TUid::Uid(0x10000017), KNullDesC);
	RDebug::Print(_L("*** CreateTestDatabaseL(): DSC for 0x10000017 created"));
	CreateDscItemL(store, TUid::Uid(0x10000017), KSysStartTescProcSlowLog, _L("0"), EStartProcess, EWaitForStart, ERetry0, 
	5000, EFalse, EFalse, EFalse, ELast);
	CreateDscItemL(store, TUid::Uid(0x10000017), KSysStartTescProcSlowLog, _L("1"), EStartProcess, EWaitForStart, ERetry0, 
	5000, EFalse, EFalse, EFalse, ELast);
	CreateDscItemL(store, TUid::Uid(0x10000017), KSysStartTescProcSlowLog, _L("2"), EStartProcess, EWaitForStart, ERetry0, 
	5000, EFalse, EFalse, EFalse, ELast);
	CreateDscItemL(store, TUid::Uid(0x10000017), KSysStartTescProcSlowLog, _L("3"), EStartProcess, EWaitForStart, ERetry0, 
	5000, EFalse, EFalse, EFalse, ELast);
	CreateDscItemL(store, TUid::Uid(0x10000017), KSysStartTescProcSlowLog, _L("4"), EStartProcess, EWaitForStart, ERetry0, 
	5000, EFalse, EFalse, EFalse, ELast);
	CreateDscItemL(store, TUid::Uid(0x10000017), KSysStartTescProcSlowLog, _L("5"), EStartProcess, EWaitForStart, ERetry0, 
	5000, EFalse, EFalse, EFalse, ELast);
	CreateDscItemL(store, TUid::Uid(0x10000017), KSysStartTescProcSlowLog, _L("6"), EStartProcess, EWaitForStart, ERetry0, 
	5000, EFalse, EFalse, EFalse, ELast);
	CreateDscItemL(store, TUid::Uid(0x10000017), KSysStartTescProcSlowLog, _L("7"), EStartProcess, EWaitForStart, ERetry0, 
	5000, EFalse, EFalse, EFalse, ELast);
	CreateDscItemL(store, TUid::Uid(0x10000017), KSysStartTescProcSlowLog, _L("8"), EStartProcess, EWaitForStart, ERetry0, 
	5000, EFalse, EFalse, EFalse, ELast);
	CreateDscItemL(store, TUid::Uid(0x10000017), KSysStartTescProcSlowLog, _L("9"), EStartProcess, EWaitForStart, ERetry0, 
	5000, EFalse, EFalse, EFalse, ELast);
#endif // SYMBIAN_SYSTEM_STATE_MANAGEMENT

	// Add entries needed for test case 7
	RDebug::Print(_L("*** CreateTestDatabaseL(): Add entries needed for test case 7:"));
	store.CreateDscL(TUid::Uid(0x10000018), KNullDesC);
	RDebug::Print(_L("*** CreateTestDatabaseL(): DSC for 0x10000018 created"));
	CreateDscItemL(store, TUid::Uid(0x10000018), KAMCFileNameApp1, _L("-Log tamastarter_result_7 -Re 0"), EStartProcess, EWaitForStart, ERetry0, 5000, EFalse, EFalse, EFalse, ELast);
	CreateDscItemL(store, TUid::Uid(0x10000018), KAMCFileNameApp3, _L("-Log tamastarter_result_7"), EStartProcess, EFireAndForget, ERetry0, 5000, EFalse, ETrue, EFalse, ELast);
	
	// Disconnecting from DSC database
	CleanupStack::PopAndDestroy(&store); //store.Close();
	RDebug::Print(_L("*** CreateTestDatabaseL(): Disonnected from DSC database"));
	}

TBool CompareDscItemsL(TUid aDscUid, const TDesC& aAmaFilename, const TDesC& aAmaArguments, TStartupType aStartupType, TStartMethod aStartMethod, TNoOfRetries aNoOfRetries, TInt aTimeout, TBool aIsMonitored, TBool aIsStartInBackground, TBool aIsViewless)
	{
	// Create a new dsc item from the expected parameters
	CDscItem* dscItem = CDscItem::NewL();
	CleanupStack::PushL(dscItem);
	
	dscItem->SetDscId(aDscUid);
	dscItem->SetFileParamsL(aAmaFilename, aAmaArguments);
	dscItem->SetStartupType(TStartupType(aStartupType));
	dscItem->SetStartMethodL(TStartMethod(aStartMethod));
	dscItem->SetNoOfRetriesL(aNoOfRetries);
	dscItem->SetTimeoutL(aTimeout);			
	dscItem->SetMonitored(aIsMonitored);
	dscItem->SetStartInBackground(aIsStartInBackground);
	dscItem->SetViewless(aIsViewless);
	
	// Read the item from the database to be compared (based on FileName, Arguments and DscId provided)
	RDscStore store;
	CleanupClosePushL(store);
	store.OpenL();

	CDscItem *dscItemFromDb = CDscItem::NewL();					
	CleanupStack::PushL(dscItemFromDb);

	//Read the item from the database to be compared (based on FileName, Arguments and DscId provided)
	dscItemFromDb->SetDscId(aDscUid);
	dscItemFromDb->SetFileParamsL(aAmaFilename, aAmaArguments);
	store.ReadItemL(*dscItemFromDb);

	TBool same = ETrue;
	// Comparing item from database with expected item	
	RDebug::Print(_L("Comparing item from database with expected item..."));
	if (dscItem->FileName().Compare(dscItemFromDb->FileName()) != 0)
		{
		same = EFalse;
		RDebug::Print(_L("Comparing item FileName differs from expected!"));		
		}
	if (dscItem->Args().Compare(dscItemFromDb->Args()) != 0)
		{
		same = EFalse;
		RDebug::Print(_L("Comparing item Args differs from expected!"));		
		}
	if (dscItem->StartupType() != dscItemFromDb->StartupType())
		{
		same = EFalse;
		RDebug::Print(_L("Comparing item StartupType differs from expected!"));		
		}
	if (dscItem->StartMethod() != dscItemFromDb->StartMethod())
		{
		same = EFalse;
		RDebug::Print(_L("Comparing item StartMethod differs from expected!"));		
		}
	if (dscItem->NoOfRetries() != dscItemFromDb->NoOfRetries())
		{
		same = EFalse;
		RDebug::Print(_L("Comparing item NoOfRetries differs from expected!"));		
		}
	if (dscItem->Timeout() != dscItemFromDb->Timeout())
		{
		same = EFalse;
		RDebug::Print(_L("Comparing item Timeout differs from expected!"));		
		}
	if (dscItem->Monitored() != dscItemFromDb->Monitored())
		{
		same = EFalse;
		RDebug::Print(_L("Comparing item Monitored differs from expected!"));		
		}
	if (dscItem->Viewless() != dscItemFromDb->Viewless())
		{
		same = EFalse;
		RDebug::Print(_L("Comparing item Viewless differs from expected!"));		
		}
	if (dscItem->StartInBackground() == dscItemFromDb->StartInBackground())
		{
		same = EFalse;
		RDebug::Print(_L("Comparing item StartInBackground differs from expected!"));		
		}

	// Cleanup
	CleanupStack::PopAndDestroy(dscItemFromDb);
	CleanupStack::PopAndDestroy(&store);
	CleanupStack::PopAndDestroy(dscItem);
	
	return same;
	}
