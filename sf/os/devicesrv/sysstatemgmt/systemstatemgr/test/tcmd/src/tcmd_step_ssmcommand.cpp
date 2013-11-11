//tcmd_step_ccmcommand.cpp

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

#include "tcmd_step_ssmcommand.h"
#include <ssm/ssmsystemwideproperty.h>
#include <ssm/ssmcmd.hrh>
#include <dscstore.h>
#include <dscitem.h>
#include <ssm/ssmloadamastart.h>
#include <bautils.h>

_LIT(KDbmsDscDdbbOriginal,"c:\\private\\100012a5\\DBS_2000836D_DBS_DSC.db");
_LIT(KDbmsDscDdbbBackup,"c:\\private\\100012a5\\DBS_2000836D_DBS_DSC.db.backup");
_LIT(KSqlDscDdbbOriginal,"c:\\private\\10281e17\\[2000836D]DBS_DSC.db");
_LIT(KSqlDscDdbbBackup,"c:\\private\\10281e17\\[2000836D]DBS_DSC.db.backup");
_LIT(KAMCFileNameApp1,"tamastarter_app1.exe");
_LIT(KGoodApp, "ssmtestappgood.exe");
_LIT(KTestProcGood, "ssmtestprocgood.exe");

CCmdTestSsmCommand::CCmdTestSsmCommand()
	{
	SetTestStepName(KTCCmdTestSsmCommand);
	}

CCmdTestSsmCommand::~CCmdTestSsmCommand()
	{
	}

void DscDatabaseRestoreL()
	{
	// Restore DBMS / SQL data store backups available prior to running the test
	//		(if failures are reported, this may indicate that the ddbb didn't previously exist - should not be a problem)
	RFs fs;
	CleanupClosePushL(fs);
	User::LeaveIfError(fs.Connect());
	
	CFileMan* fileMan; 
	fileMan = CFileMan::NewL(fs);
	CleanupStack::PushL(fileMan);
	
	// DBMS ddbb
	RDebug::Print(_L("*** DscDatabaseBackupL(): Restore DBMS data store backup available prior to running the test"));
	TEntry fileEntry;
	TInt err = fs.Entry(KDbmsDscDdbbOriginal, fileEntry);
	if(KErrNone == err)
		{
		User::LeaveIfError(fs.Delete(KDbmsDscDdbbOriginal));
		}
	err = fs.Entry(KDbmsDscDdbbBackup, fileEntry);
	if(KErrNone == err)
		{
		User::LeaveIfError(fileMan->Copy(KDbmsDscDdbbBackup, KDbmsDscDdbbOriginal));
		User::LeaveIfError(fs.Delete(KDbmsDscDdbbBackup));
		}

	// SQL ddbb
	RDebug::Print(_L("*** DscDatabaseBackupL(): Restore SQL data store backup available prior to running the test"));
	err = fs.Entry(KSqlDscDdbbOriginal, fileEntry);
	if(KErrNone == err)
		{
		User::LeaveIfError(fs.Delete(KSqlDscDdbbOriginal));
		}
	err = fs.Entry(KSqlDscDdbbBackup, fileEntry);
	if(KErrNone == err)
		{
		User::LeaveIfError(fileMan->Copy(KSqlDscDdbbBackup, KSqlDscDdbbOriginal));
		User::LeaveIfError(fs.Delete(KSqlDscDdbbBackup));
		}
	CleanupStack::PopAndDestroy(fileMan);
	CleanupStack::PopAndDestroy(&fs);
	}

LOCAL_C void CreateDscItemL(RDscStore& aStore, TUid aDscUid, const TDesC& aAmaFilename, const TDesC& aAmaArguments, TSsmCommandType aStartupType, TSsmExecutionBehaviour aStartMethod, TNoOfRetries aNoOfRetries, TInt aTimeout, TBool aIsMonitored, TBool aIsStartInBackground, TBool aIsViewless, TDscPosition aDscPosition)
	{
	CDscItem* dscItem = CDscItem::NewL();
	CleanupStack::PushL(dscItem);
	
	dscItem->SetDscId(aDscUid);
	dscItem->SetFileParamsL(aAmaFilename, aAmaArguments);
	dscItem->SetStartupTypeL(TStartupType(aStartupType));
	dscItem->SetStartMethodL(TStartMethod(aStartMethod));
	dscItem->SetNoOfRetriesL(aNoOfRetries);
	dscItem->SetTimeoutL(aTimeout);			
	dscItem->SetMonitored(aIsMonitored);
	dscItem->SetStartInBackground(aIsStartInBackground);
	dscItem->SetViewless(aIsViewless);

	// Insert the Dsc item in the position specified
	TRAPD(ret, aStore.AddItemL(*dscItem, TDscPosition(aDscPosition)));
	RDebug::Print(_L("*** CreateDscItemL(): Adding single item to DSC database finished with value '%d'"), ret);
	//User::LeaveIfError(ret);

	CleanupStack::PopAndDestroy(dscItem);
	}

LOCAL_C void amastartL(TSsmExecutionBehaviour aExecutionBehaviour)
	{
	DscDatabaseRestoreL();
	// Create appropriate data store for the tests
	RDscStore store;
	// Connecting to DSC database
	store.OpenL();
	CleanupClosePushL(store);
	
	TRAPD(err, store.CreateDscL(TUid::Uid(0x10000011), KNullDesC));
	
	RDebug::Print(_L("*** CreateTestDatabaseL(): DSC for 0x10000011 created with error %d"),err);
	CreateDscItemL(store, TUid::Uid(0x10000011), KAMCFileNameApp1, _L("-Log tamastarter_result_1"), ESsmCmdStartProcess, aExecutionBehaviour, ERetry0, 5000, ETrue, ETrue, EFalse, ELast);
	// Disconnecting from DSC database
	CleanupStack::PopAndDestroy(&store); //store.Close();
	}
	
/**
Helper function to find and kill the specified process
*/
TInt CCmdTestSsmCommand::FindAndKill(const TDesC& aProcessName)
	{
	TFullName searchTerm(aProcessName);
	StripExtension( searchTerm );	
	searchTerm += _L("*");
	TFindProcess find(searchTerm);
	TFullName name;
	TInt instancesFound = 0;
	while(find.Next(name) == KErrNone)
		{
		RProcess process;
		const TInt err = process.Open(find);

		if (KErrNone == err)
			{
			if (process.ExitType() == EExitPending)
				{
				instancesFound++;
				process.Kill(KErrCancel);
				process.Close();
				INFO_PRINTF2(_L("Process %S found and killed"), &aProcessName);
				}
			process.Close();
			}
		}
	return instancesFound;
	}
	
void CCmdTestSsmCommand::StripExtension( TDes& aFilename )
	{
	TInt dot = aFilename.Find( _L(".") );
	
	if( KErrNotFound != dot)
		{
		aFilename.SetLength( dot );
		}
	}	
		
TVerdict CCmdTestSsmCommand::doTestStepPreambleL()
	{
	iActiveScheduler = new(ELeave) CActiveScheduler;
	CActiveScheduler::Install (iActiveScheduler);

	iActiveSchedulerWait = new(ELeave) CActiveSchedulerWait;
             
 	//Connect to SsmStateManager
 	const TInt connect = iClient.Connect();
	TEST(KErrNone == connect);
	return TestStepResult();
	}
		
TVerdict CCmdTestSsmCommand::doTestStepL()
	{
	INFO_PRINTF1(_L("doTestStepL"));
	TInt err = 0;
	
	INFO_PRINTF1(_L("TestStartApp execution behaviour is WaitForSignal"));	
	TRAP(err,TestStartApp(ESwpStartAppWFS));
	TEST(KErrNone == err);
	INFO_PRINTF1(_L("TestStartApp execution behaviour is DeferredWaitForSignal"));
	TRAP(err,TestStartApp(ESwpStartAppDWFS));
	TEST(KErrNone == err);
	INFO_PRINTF1(_L("TestStartApp execution behaviour is FireAndForget"));
	TRAP(err,TestStartApp(ESwpStartAppFAF));
	TEST(KErrNone == err);
	INFO_PRINTF1(_L("TestStartAppCancel the execution behaviour is WaitForSignal"));	
	TRAP(err,TestStartAppCancel(ESwpStartAppWFS));
	TEST(KErrNone == err);
	INFO_PRINTF1(_L("TestStartAppCancel execution behaviour is DeferredWaitForSignal"));
	TRAP(err,TestStartAppCancel(ESwpStartAppDWFS));
	TEST(KErrNone == err);
	
	INFO_PRINTF1(_L("TestStartProc execution behaviour is WaitForSignal"));	
	TRAP(err,TestStartApp(ESwpStartProcWFS));
	TEST(KErrNone == err);
	INFO_PRINTF1(_L("TestStartProc execution behaviour is DeferredWaitForSignal"));
	TRAP(err,TestStartProc(ESwpStartProcDWFS));
	TEST(KErrNone == err);
	INFO_PRINTF1(_L("TestStartProc execution behaviour is FireAndForget"));
	TRAP(err,TestStartProc(ESwpStartProcFAF));
	TEST(KErrNone == err);
	User::After(5000000);
	INFO_PRINTF1(_L("TestStartProcCancel the execution behaviour is WaitForSignal"));	
	TRAP(err,TestStartProcCancel(ESwpStartProcWFS));
	TEST(KErrNone == err);
	INFO_PRINTF1(_L("TestStartProcCancel execution behaviour is DeferredWaitForSignal"));
	TRAP(err,TestStartProcCancel(ESwpStartProcDWFS));
	TEST(KErrNone == err);
	
	TRAP(err,TestAMAStarterCommandL(ESwpAmaStarterFAF));
	TEST(KErrNone == err);
	INFO_PRINTF3(_L("TestAMAStarterCommandL completed with %d, expected %d"),err,KErrNone);
	TRAP(err,TestAMAStarterCommandL(ESwpAmaStarterWFS));
	TEST(KErrNone == err);
	INFO_PRINTF3(_L("TestAMAStarterCommandL completed with %d, expected %d"),err,KErrNone);
	
	INFO_PRINTF1(_L("TestWaitForApparcInit "));	
	TRAP(err,TestWaitForApparcInit());
	TEST(KErrNone == err);
	
	INFO_PRINTF1(_L("TestFinaliseDrive "));	
	TRAP(err,TestFinaliseDrive(ESwpFinaliseDrive));
	TEST(KErrNone == err);
	
	INFO_PRINTF1(_L("TestFinaliseDriveCancel "));	
	TRAP(err,TestFinaliseDriveCancel(ESwpFinaliseDrive));
	TEST(KErrNone == err);
	
	INFO_PRINTF1(_L("TestMultipleWaitCommand "));	
	TRAP(err,TestMultipleWaitCommand());
	TEST(KErrNone == err);
	
	INFO_PRINTF1(_L("TestMultipleWaitCommandCancel "));
	TRAP(err,TestMultipleWaitCommandCancel());
	TEST(KErrNone == err);
	
	INFO_PRINTF1(_L("TestPersisthalAttributes "));	
	TRAP(err,TestPersisthalAttributesL(ESwpPersisthalAttributes));
	TEST(KErrNone == err);
	
	INFO_PRINTF1(_L("TestPersisthalAttributesCancel "));	
	TRAP(err,TestPersisthalAttributesCancelL(ESwpPersisthalAttributes));
	TEST(KErrNone == err);
	#ifdef __WINS__
	#define __SSPLUGINSTEST_CNDEVAL_CR__
	INFO_PRINTF1(_L("TestPowerOff "));	
	TRAP(err,TestPowerOff(ESwpPowerOff));
	TEST(KErrNone == err);
	
	INFO_PRINTF1(_L("TestPowerOffCancel "));	
	TRAP(err,TestPowerOffCancel(ESwpPowerOff));
	#endif
	TEST(KErrNone == err);
	__UHEAP_MARK;
	
    return TestStepResult();
	}

/**
Old Test CaseID 		AFSS-CMDBCS-0014
New Test CaseID 		DEVSRVS-SSMA-CMD-0008
 */

void CCmdTestSsmCommand::TestAMAStarterCommandL(TSwpBuiltInCmdValues aCmdValue)
	{
	INFO_PRINTF1(_L("TestAMAStarterCommandL"));
	switch(aCmdValue)
		{
		case ESwpAmaStarterWFS:
			amastartL(ESsmWaitForSignal);
			break;
		case ESwpAmaStarterFAF:
			amastartL(ESsmFireAndForget);
			break;
		default:
			break;
		}
	CreateAndExecuteCmd1(aCmdValue);
	TInt instances = FindAndKill(KAMCFileNameApp1);
	INFO_PRINTF2(_L("Instances of KGoodApp found was '%d'."), instances);
	RDebug::Print(_L("TestAMAStarterCommandL completed with %d, expected %d"),iStatus.Int(),KErrNone);
	INFO_PRINTF3(_L("TestAMAStarterCommandL completed with %d, expected %d"),iStatus.Int(),KErrNone);
	}

/**
Old Test CaseID 		AFSS-CMDBCS-0014
New Test CaseID 		DEVSRVS-SSMA-CMD-0008
 */
	
void CCmdTestSsmCommand::TestAMAStarterCommandCancelL(TSwpBuiltInCmdValues aCmdValue)
	{
	INFO_PRINTF1(_L("TestAMAStarterCommandCancelL"));
	switch(aCmdValue)
		{
		case ESwpAmaStarterWFS:
			amastartL(ESsmWaitForSignal);
			break;
		case ESwpAmaStarterFAF:
			amastartL(ESsmFireAndForget);
			break;
		}
	CreateAndExecuteCancelCmd1(aCmdValue);
	TInt instances = FindAndKill(KAMCFileNameApp1);
	INFO_PRINTF2(_L("Instances of KGoodApp found was '%d'."), instances);
	RDebug::Print(_L("TestAMAStarterCommandCancelL completed with %d, expected %d"),iStatus.Int(),KErrNone);
	INFO_PRINTF3(_L("TestAMAStarterCommandCancelL completed with %d, expected %d"),iStatus.Int(),KErrNone);
	}
/**
Old Test CaseID 		AFSS-CMDBCS-0014
New Test CaseID 		DEVSRVS-SSMA-CMD-0008
 */

void CCmdTestSsmCommand::TestStartApp(TSwpBuiltInCmdValues aCmdValue)
	{
	CreateAndExecuteCmd(aCmdValue);
	RDebug::Print(_L("TestStartApp completed with %d, expected %d"),iStatus.Int(),KErrNone);	
	INFO_PRINTF3(_L("TestStartApp completed with %d, expected %d"),iStatus.Int(),KErrNone);	
	}
/**
Old Test CaseID 		AFSS-CMDBCS-0014
New Test CaseID 		DEVSRVS-SSMA-CMD-0008
 */
	
void CCmdTestSsmCommand::TestStartAppCancel(TSwpBuiltInCmdValues aCmdValue)
	{
	CreateAndExecuteCancelCmd(aCmdValue);
	RDebug::Print(_L("TestStartAppCancel completed with %d, expected %d"),iCancelReqStatus.Int(),KErrCancel);	
	INFO_PRINTF3(_L("TestStartAppCancel completed with %d, expected %d"),iCancelReqStatus.Int(),KErrCancel);	
	}	

/**
Old Test CaseID 		AFSS-CMDBCS-0014
New Test CaseID 		DEVSRVS-SSMA-CMD-0008
 */
	
void CCmdTestSsmCommand::TestStartProc(TSwpBuiltInCmdValues aCmdValue)
	{
	CreateAndExecuteCmd(aCmdValue);
	RDebug::Print(_L("TestStartProc completed with %d, expected %d"),iStatus.Int(),KErrNone);	
	INFO_PRINTF3(_L("TestStartProc completed with %d, expected %d"),iStatus.Int(),KErrNone);	
	}
	
/**
Old Test CaseID 		AFSS-CMDBCS-0014
New Test CaseID 		DEVSRVS-SSMA-CMD-0008
 */
	

void CCmdTestSsmCommand::TestStartProcCancel(TSwpBuiltInCmdValues aCmdValue)
	{
	CreateAndExecuteCancelCmd(aCmdValue);
	RDebug::Print(_L("TestStartProcCancel completed with %d, expected %d"),iCancelReqStatus.Int(),KErrCancel);	
	INFO_PRINTF3(_L("TestStartProcCancel completed with %d, expected %d"),iCancelReqStatus.Int(),KErrCancel);	
	}						

/**
Old Test CaseID 		AFSS-CMDBCS-0014
New Test CaseID 		DEVSRVS-SSMA-CMD-0008
 */
	
void CCmdTestSsmCommand::TestMultipleWaitCommand()
	{
	INFO_PRINTF1(_L("TestWaitForApparcInit"));
	CreateAndExecuteCmd(ESwpMultipleWait);
	RDebug::Print(_L("TestMultipleWaitCommand completed with %d, expected %d"),iStatus.Int(),KErrNone);	
	INFO_PRINTF3(_L("TestMultipleWaitCommand completed with %d, expected %d"),iStatus.Int(),KErrNone);	
	}

/**
Old Test CaseID 		AFSS-CMDBCS-0014
New Test CaseID 		DEVSRVS-SSMA-CMD-0008
 */

void CCmdTestSsmCommand::TestMultipleWaitCommandCancel()
	{
	INFO_PRINTF1(_L("TestMultipleWaitCommandCancel"));
	CreateAndExecuteCancelCmd(ESwpMultipleWait);
	RDebug::Print(_L("TestMultipleWaitCommandCancel completed with %d, expected %d"),iCancelReqStatus.Int(),KErrCancel);	
	INFO_PRINTF3(_L("TestMultipleWaitCommandCancel completed with %d, expected %d"),iCancelReqStatus.Int(),KErrCancel);	
	}

/**
Old Test CaseID 		AFSS-CMDBCS-0014
New Test CaseID 		DEVSRVS-SSMA-CMD-0008
 */

void CCmdTestSsmCommand::TestPersisthalAttributesL(TSwpBuiltInCmdValues aCmdValue)
	{
	//HAL attributes data file name 
	_LIT(KHalFile,"c:\\private\\102825B1\\HAL.DAT");
	RFs fs;
    User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	fs.Delete(KHalFile);
	CreateAndExecuteCmd1(aCmdValue);
	TBool found = BaflUtils::FileExists(fs, KHalFile);
	CleanupStack::PopAndDestroy(&fs); 
	TEST(found);
	RDebug::Print(_L("TestPersisthalAttributes completed with %d, expected %d"),iStatus.Int(),KErrNone);	
	INFO_PRINTF3(_L("TestPersisthalAttributes completed with %d, expected %d"),iStatus.Int(),KErrNone);	
	}               

/**
Old Test CaseID 		AFSS-CMDBCS-0014
New Test CaseID 		DEVSRVS-SSMA-CMD-0008
 */
	
void CCmdTestSsmCommand::TestPersisthalAttributesCancelL(TSwpBuiltInCmdValues aCmdValue)
	{
	//HAL attributes data file name 
	_LIT(KHalFile,"c:\\private\\102825B1\\HAL.DAT");
	RFs fs;
    User::LeaveIfError(fs.Connect());
	CleanupClosePushL(fs);
	fs.Delete(KHalFile);
	CreateAndExecuteCancelCmd1(aCmdValue);
	TBool found = BaflUtils::FileExists(fs, KHalFile);
	CleanupStack::PopAndDestroy(&fs); 
	RDebug::Print(_L("TestPersisthalAttributesCancel completed with %d, expected %d"),iCancelReqStatus.Int(),KErrCancel);	
	INFO_PRINTF3(_L("TestPersisthalAttributesCancel completed with %d, expected %d"),iCancelReqStatus.Int(),KErrCancel);	
	}               

/**
Old Test CaseID 		AFSS-CMDBCS-0014
New Test CaseID 		DEVSRVS-SSMA-CMD-0008
 */

void CCmdTestSsmCommand::TestFinaliseDrive(TSwpBuiltInCmdValues aCmdValue)
	{
	CreateAndExecuteCmd1(aCmdValue);
	RDebug::Print(_L("TestFinaliseDrive completed with %d, expected %d"),iStatus.Int(),KErrNone);	
	INFO_PRINTF3(_L("TestFinaliseDrive completed with %d, expected %d"),iStatus.Int(),KErrNone);	
	}

/**
Old Test CaseID 		AFSS-CMDBCS-0014
New Test CaseID 		DEVSRVS-SSMA-CMD-0008
 */

void CCmdTestSsmCommand::TestFinaliseDriveCancel(TSwpBuiltInCmdValues aCmdValue)
	{
	CreateAndExecuteCancelCmd1(aCmdValue);
	RDebug::Print(_L("TestFinaliseDriveCancel completed with %d, expected %d"),iCancelReqStatus.Int(),KErrCancel);	
	INFO_PRINTF3(_L("TestFinaliseDriveCancel completed with %d, expected %d"),iCancelReqStatus.Int(),KErrCancel);	
	}	
/**
Old Test CaseID 		AFSS-CMDBCS-0014
New Test CaseID 		DEVSRVS-SSMA-CMD-0008
 */
void CCmdTestSsmCommand::TestPowerOff(TSwpBuiltInCmdValues aCmdValue)
	{
	const TUint testCategory = 0x000456;
	const TUid KPropertyCategory={0x2000D75B};	// ssmserver SID 
	const TUid KServerSID = KPropertyCategory;
	TInt err = RProperty::Define(KServerSID, testCategory, RProperty::EInt, EFalse);
	CreateAndExecuteCmd1(aCmdValue);;
	RDebug::Print(_L("TestPowerOff completed with %d, expected %d"),iStatus.Int(),KErrNone);	
	INFO_PRINTF3(_L("TestPowerOff completed with %d, expected %d"),iStatus.Int(),KErrNone);	
	}		

/**
Old Test CaseID 		AFSS-CMDBCS-0014
New Test CaseID 		DEVSRVS-SSMA-CMD-0008
 */
	
void CCmdTestSsmCommand::TestPowerOffCancel(TSwpBuiltInCmdValues aCmdValue)
	{
	CreateAndExecuteCancelCmd1(aCmdValue);;
	RDebug::Print(_L("TestPowerOffCancel completed with %d, expected %d"),iCancelReqStatus.Int(),KErrCancel);	
	INFO_PRINTF3(_L("TestPowerOffCancel completed with %d, expected %d"),iCancelReqStatus.Int(),KErrCancel);	
	}	
	
/**
Old Test CaseID 		AFSS-CMDBCS-0014
New Test CaseID 		DEVSRVS-SSMA-CMD-0008
 */
void CCmdTestSsmCommand::TestWaitForApparcInit()
	{
	INFO_PRINTF1(_L("TestWaitForApparcInit"));
	CreateAndExecuteCmd1(ESwpWaitForApparcInit);
	RDebug::Print(_L("TestWaitForApparcInit completed with %d, expected %d"),iStatus.Int(),KErrNone);	
	INFO_PRINTF3(_L("TestWaitForApparcInit completed with %d, expected %d"),iStatus.Int(),KErrNone);	
	}
			
void CCmdTestSsmCommand::CreateAndExecuteCmd(TSwpBuiltInCmdValues aCmdValue)
	{
	TInt err = RProperty::Define(KMySID, 0, RProperty::EInt);
	RDebug::Print(_L(" CCmdTestSsmCommand::CreateAndExecuteCmd RProperty::Define %d"), err);

	RSsmStateManager sess;
	err = sess.Connect();
	RDebug::Print(_L("RSsmStateManager sess connect %d"), err);

	err = sess.RegisterSwpMapping(KSwpTestKey1, KTestSwpPolicyCmd);
	TSsmSwp swp(KSwpTestKey1,0);

	swp.Set(KSwpTestKey1, aCmdValue);		
	RSsmSystemWideProperty property;
	err = property.Connect(KSwpTestKey1);
	RDebug::Print(_L("RSsmStateManager RSsmSystemWideProperty Connect %d"), err);
	sess.RequestSwpChange(swp, iStatus);
	User::WaitForRequest(iStatus);
	// Wait for 0.5 sec to allow transitions to fully complete
	User::After(5000000);		
	TEST(KErrNone == iStatus.Int());
	//Find any instances of running process started by this test and kill them
	TInt instances = 0;
	if(aCmdValue == ESwpStartProcWFS || aCmdValue == ESwpStartProcDWFS || aCmdValue == ESwpStartProcFAF )
		{
		instances = FindAndKill(KTestProcGood);
		INFO_PRINTF2(_L("Instances of KTestProcGood found was '%d'."), instances);
		}
	else
		{
		instances = FindAndKill(KGoodApp);
		INFO_PRINTF2(_L("Instances of KGoodApp found was '%d'."), instances);
		}	
	TEST(instances==1);
	}
					
void CCmdTestSsmCommand::CreateAndExecuteCancelCmd(TSwpBuiltInCmdValues aCmdValue)
	{
	TInt err = RProperty::Define(KMySID, 0, RProperty::EInt);
	RDebug::Print(_L(" CCmdTestSsmCommand::CreateAndExecuteCancelCmd RProperty::Define %d"), err);

	RSsmStateManager sess;
	err = sess.Connect();
	RDebug::Print(_L("RSsmStateManager sess connect %d"), err);

	err = sess.RegisterSwpMapping(KSwpTestKey1, KTestSwpPolicyCmd);
	TSsmSwp swp(KSwpTestKey1,0);

	swp.Set(KSwpTestKey1, aCmdValue);

	RSsmSystemWideProperty property;
	err = property.Connect(KSwpTestKey1);
	RDebug::Print(_L("RSsmStateManager RSsmSystemWideProperty Connect %d"), err);

	//Transition engine will not cancel any request that is currently in execution. It will delete only quad request.
	sess.RequestSwpChange(swp, iStatus);		
	sess.RequestSwpChange(swp, iCancelReqStatus);			
	
	TEST(iStatus == KRequestPending);
	User::WaitForRequest(iStatus);
	
	sess.RequestSwpChangeCancel();
	User::WaitForRequest(iCancelReqStatus);
	
	TEST(iStatus.Int() == KErrNone);
	TEST(iCancelReqStatus.Int() == KErrCancel);
	RDebug::Print(_L("CreateAndExecuteCancelCustCmdL completes with %d err"),iStatus.Int());
	
	// Wait for 0.5 sec to allow transitions to fully complete
	User::After(5000000);
	
	//Find any instances of running process started by this test and kill them
	TInt instances = 0 ;
	if(aCmdValue == ESwpStartProcWFS || aCmdValue == ESwpStartProcDWFS || aCmdValue == ESwpStartProcFAF )
		{
		instances = FindAndKill(KTestProcGood);
		INFO_PRINTF2(_L("Instances of KTestProcGood found was '%d'."), instances);
		}
	else
		{
		instances = FindAndKill(KGoodApp);	
		INFO_PRINTF2(_L("Instances of KGoodApp found was '%d'."), instances);
		}	
	TEST(instances==1);
	}
	
void CCmdTestSsmCommand::CreateAndExecuteCmd1(TSwpBuiltInCmdValues aCmdValue)	
	{
	TInt err = RProperty::Define(KMySID, 0, RProperty::EInt);
	RDebug::Print(_L("RProperty::Define completed with %d"), err);
	RSsmStateManager sess;
	err = sess.Connect();
	RDebug::Print(_L("RSsmStateManager sess connect %d"), err);

	err = sess.RegisterSwpMapping(KSwpTestKey1, KTestSwpPolicyCmd);
	TSsmSwp swp(KSwpTestKey1,0);

	swp.Set(KSwpTestKey1, aCmdValue);		
	RSsmSystemWideProperty property;
	err = property.Connect(KSwpTestKey1);
	RDebug::Print(_L("RSsmStateManager RSsmSystemWideProperty Connect %d"), err);
	sess.RequestSwpChange(swp, iStatus);
	User::WaitForRequest(iStatus);
	User::After(5000000);		
	
	TEST(KErrNone == iStatus.Int());	
	}
	
void CCmdTestSsmCommand::CreateAndExecuteCancelCmd1(TSwpBuiltInCmdValues aCmdValue)
	{
	TInt err = RProperty::Define(KMySID, 0, RProperty::EInt);
	RDebug::Print(_L(" CCmdTestSsmCommand::CreateAndExecuteCancelCmd1 RProperty::Define %d"), err);

	RSsmStateManager sess;
	err = sess.Connect();
	RDebug::Print(_L("RSsmStateManager sess connect %d"), err);

	err = sess.RegisterSwpMapping(KSwpTestKey1, KTestSwpPolicyCmd);
	TSsmSwp swp(KSwpTestKey1,0);

	swp.Set(KSwpTestKey1, aCmdValue);

	RSsmSystemWideProperty property;
	err = property.Connect(KSwpTestKey1);
	RDebug::Print(_L("RSsmStateManager RSsmSystemWideProperty Connect %d"), err);

	//Transition engine will not cancel any request that is currently in execution. It will delete only quad request.
	sess.RequestSwpChange(swp, iStatus);		
	sess.RequestSwpChange(swp, iCancelReqStatus);			

	TEST(iStatus == KRequestPending);
	User::WaitForRequest(iStatus);
	
	sess.RequestSwpChangeCancel();
	User::WaitForRequest(iCancelReqStatus);
	
	TEST(KErrNone == iStatus.Int());	
	TEST(KErrCancel == iCancelReqStatus.Int());
	RDebug::Print(_L("CreateAndExecuteCancelCmd1 completes with %d err"),iCancelReqStatus.Int());
	}
	
TVerdict CCmdTestSsmCommand::doTestStepPostambleL()
	{
	iClient.Close();
	__UHEAP_MARKEND;
	return TestStepResult();
	}

