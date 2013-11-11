// Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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
// CCommsdat170Step
// CCommsdat171Step
// CCommsdat172Step
// 
//

/**
 @file commsdatConcurrencyTests.cpp
*/
#include "commsdatConcurrencyTests.h"
#include "Te_commsdatSuiteDefs.h"
#include <e32test.h>
 
//----------------------------------------------------------------------------------------------------------------
CCommsdat170Step::CCommsdat170Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat170Step);                                                                                           
	}

TVerdict CCommsdat170Step::doTestStepL()
/**
 * @return - TVerdict code
 * Test open transaction in concurrency situations
 * CED -i Z:\testdata\configs\te_commsdat.cfg
 */
	{
    SetTestStepResult(EFail);	
	INFO_PRINTF1(_L("Testing concurrency in CCommsdat170Step::doTestStepL() "));
	
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_2);
#else
 	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_1);
#endif
	CleanupStack::PushL(cmdbSession);
	
	cmdbSession->OpenTransactionL();
	
	CCDVirtualBearerRecord* record = static_cast<CCDVirtualBearerRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdVirtualBearerRecord));
	CleanupStack::PushL(record);
	
	record->SetRecordId(1);
	TPtrC nifName(_L("MainNIF"));
	record->iVirtualBearerNifName.SetMaxLengthL(nifName.Length());
	record->iVirtualBearerNifName = nifName;
	record->StoreL(*cmdbSession);
	
	_LIT(KThreadName, "TestThread170");
	RThread thread;
	if(thread.Create(KThreadName, &ThreadFunction, KDefaultStackSize, NULL, NULL) == KErrNone)
		{
		TRequestStatus waitForThreadPanic;
		thread.Rendezvous(waitForThreadPanic);
		thread.Resume();
		User::WaitForRequest(waitForThreadPanic);
			
		INFO_PRINTF2(_L("Thread returns : %d"), thread.ExitReason());
		if(thread.ExitReason() == KErrLocked)
			{
			SetTestStepResult(EPass);	
			}
		thread.Close();
		}
	
	cmdbSession->CommitTransactionL();

	CleanupStack::PopAndDestroy(record);	
	CleanupStack::PopAndDestroy(cmdbSession);
	
	return TestStepResult();
	}
	
TInt CCommsdat170Step::ThreadFunction(TAny *aPtr)
	{
	CTrapCleanup* cleanup=CTrapCleanup::New();
	
	TRAPD(errCode, DoThreadFunctionL(aPtr));
	
	delete cleanup;
	return errCode;
	}
	
TInt CCommsdat170Step::DoThreadFunctionL(TAny */*aPtr*/)
	{
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_2);
#else
 	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_1);
#endif
	CleanupStack::PushL(cmdbSession);
	
	cmdbSession->OpenTransactionL(); 
	
	CCDVirtualBearerRecord* record = static_cast<CCDVirtualBearerRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdVirtualBearerRecord));
	CleanupStack::PushL(record);
	
	record->SetRecordId(1);
	TPtrC nifName(_L("ThreadNIF"));
	record->iVirtualBearerNifName.SetMaxLengthL(nifName.Length());
	record->iVirtualBearerNifName = nifName;
	record->StoreL(*cmdbSession); //Should leave here with KErrLocked
	
	cmdbSession->CommitTransactionL();
	
	CleanupStack::PopAndDestroy(record);
	
	
	CleanupStack::PopAndDestroy(cmdbSession);
	
	return KErrNone; //Should not come here
	}	
//----------------------------------------------------------------------------------------------------------------
CCommsdat171Step::CCommsdat171Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat171Step);                                                                                           
	}

TVerdict CCommsdat171Step::doTestStepL()
/**
 * @return - TVerdict code
 * Test commit transaction in concurrency situations
 * CED -i Z:\testdata\configs\te_commsdat_virtualbearer_table1.cfg
 */
	{
    SetTestStepResult(EFail);	
	INFO_PRINTF1(_L("Testing concurrency in CCommsdat171Step::doTestStepL() "));
	
	_LIT(KThreadName, "TestThread171");
	RThread thread;
	if(thread.Create(KThreadName, &ThreadFunction, KDefaultStackSize, NULL, NULL) == KErrNone)
		{		
		TRequestStatus waitForThreadModify;
		thread.Rendezvous(waitForThreadModify);
		thread.Resume();
		User::WaitForRequest(waitForThreadModify); //wait for thread to modify but not commit
		
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_2);
#else
 	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_1);
#endif
		CleanupStack::PushL(cmdbSession);
	
		CCDVirtualBearerRecord* record = static_cast<CCDVirtualBearerRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdVirtualBearerRecord));
		CleanupStack::PushL(record);
	
		record->SetRecordId(1);
		record->LoadL(*cmdbSession);
		
		_LIT(KOldNifName, "NIF1");
		TBool intermediate = EFalse;
	
		TPtrC vbNifName1(record->iVirtualBearerNifName);
		if( vbNifName1 == KOldNifName)
			{
			intermediate = ETrue;
			}
	
		INFO_PRINTF2(_L("Step 171-a NIF name before modify: %S"), &vbNifName1);	
		
		TRequestStatus waitForThreadCommit;
		thread.Rendezvous(waitForThreadCommit);
		RProcess::Rendezvous(KErrNone); //Signal to thread that load is complete
		User::WaitForRequest(waitForThreadCommit); //wait for thread to commit
		
		record->RefreshL(*cmdbSession);
		_LIT(KNewNifName, "NIF2");
	
		TPtrC vbNifName2(record->iVirtualBearerNifName);
		if(vbNifName2 == KNewNifName && intermediate)
			{
			SetTestStepResult(EPass);
			}
		
		INFO_PRINTF2(_L("Step 171-b NIF name after modify: %S"), &vbNifName2);	
		
		CleanupStack::PopAndDestroy(record);
	
		CleanupStack::PopAndDestroy(cmdbSession);
		thread.Close();
		}
	
	return TestStepResult();
	}
	
TInt CCommsdat171Step::ThreadFunction(TAny *aPtr)
	{
	CTrapCleanup* cleanup=CTrapCleanup::New();
	
	TRAPD(errCode, DoThreadFunctionL(aPtr));
	
	delete cleanup;
	return errCode;
	}
	
TInt CCommsdat171Step::DoThreadFunctionL(TAny */*aPtr*/)
	{
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_2);
#else
 	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_1);
#endif
	CleanupStack::PushL(cmdbSession);
	
	cmdbSession->OpenTransactionL();
	
	CCDVirtualBearerRecord* record = static_cast<CCDVirtualBearerRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdVirtualBearerRecord));
	CleanupStack::PushL(record);
	
	record->SetRecordId(1);
	record->LoadL(*cmdbSession);	
	TPtrC nifName(_L("NIF2"));
	record->iVirtualBearerNifName.SetMaxLengthL(nifName.Length());
	record->iVirtualBearerNifName = nifName;
	record->ModifyL(*cmdbSession);	
	
	TRequestStatus waitForMainProcessLoad;
	RProcess().Rendezvous(waitForMainProcessLoad);
	RThread::Rendezvous(KErrNone); //Signal to main thread that modified but not commited yet	
	User::WaitForRequest(waitForMainProcessLoad);
	
	cmdbSession->CommitTransactionL();
		
	CleanupStack::PopAndDestroy(record);
	CleanupStack::PopAndDestroy(cmdbSession);
	
	return KErrNone; //This will signal to main thread that commited 
	}	
//----------------------------------------------------------------------------------------------------------------
CCommsdat172Step::CCommsdat172Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat172Step);                                                                                           
	}

TVerdict CCommsdat172Step::doTestStepL()
/**
 * @return - TVerdict code
 * Test rollback transaction in concurrency situations
 * CED -i Z:\testdata\configs\te_commsdat_virtualbearer_table1.cfg
 */
	{
    SetTestStepResult(EFail);	
	INFO_PRINTF1(_L("Testing concurrency in CCommsdat172Step::doTestStepL() "));
	
	_LIT(KThreadName, "TestThread172");
	RThread thread;
	if(thread.Create(KThreadName, &ThreadFunction, KDefaultStackSize, NULL, NULL) == KErrNone)
		{
		TRequestStatus waitForThreadModify;
		thread.Rendezvous(waitForThreadModify);
		thread.Resume();
		User::WaitForRequest(waitForThreadModify); //wait for thread to modify but not rollback
		
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_2);
#else
 	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_1);
#endif
		CleanupStack::PushL(cmdbSession);
	
		CCDVirtualBearerRecord* record = static_cast<CCDVirtualBearerRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdVirtualBearerRecord));
		CleanupStack::PushL(record);
	
		record->SetRecordId(1);
		record->LoadL(*cmdbSession);
	
		_LIT(KOldNifName, "NIF1");
		TBool intermediate = EFalse;
		
		TPtrC vbNifName1(record->iVirtualBearerNifName);
		if( vbNifName1 == KOldNifName)
			{
			intermediate = ETrue;
			}
	
		INFO_PRINTF2(_L("Step 172-a NIF name before modify: %S"), &vbNifName1);	
		TRequestStatus waitForThreadRollback;
		thread.Rendezvous(waitForThreadRollback);
		RProcess::Rendezvous(KErrNone); //Signal to thread that load is complete
		User::WaitForRequest(waitForThreadRollback); //wait for thread to rollback
		
		record->RefreshL(*cmdbSession);
		
		TPtrC vbNifName2(record->iVirtualBearerNifName);
		if(vbNifName2 == KOldNifName && intermediate)
		{
			SetTestStepResult(EPass);
		}
		
		INFO_PRINTF2(_L("Step172-b NIF name after modify: %S"), &vbNifName2);	
	
		CleanupStack::PopAndDestroy(record);
	
		CleanupStack::PopAndDestroy(cmdbSession);
		thread.Close();
		}
	
	return TestStepResult();
	}

TInt CCommsdat172Step::ThreadFunction(TAny *aPtr)
	{
	CTrapCleanup* cleanup=CTrapCleanup::New();
	
	TRAPD(errCode, DoThreadFunctionL(aPtr));
	
	delete cleanup;
	return errCode;
	}
	
TInt CCommsdat172Step::DoThreadFunctionL(TAny */*aPtr*/)
	{
#ifdef SYMBIAN_NON_SEAMLESS_NETWORK_BEARER_MOBILITY
	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_2);
#else
 	CMDBSession *cmdbSession = CMDBSession::NewL(KCDVersion1_1);
#endif
	CleanupStack::PushL(cmdbSession);
	
	cmdbSession->OpenTransactionL(); 
	
	CCDVirtualBearerRecord* record = static_cast<CCDVirtualBearerRecord*>(CCDRecordBase::RecordFactoryL(KCDTIdVirtualBearerRecord));
	CleanupStack::PushL(record);
	
	record->SetRecordId(1);
	record->LoadL(*cmdbSession);	
	TPtrC nifName(_L("NIF2"));
	record->iVirtualBearerNifName.SetMaxLengthL(nifName.Length());
	record->iVirtualBearerNifName = nifName;
	record->ModifyL(*cmdbSession);	

	TRequestStatus waitForMainProcessLoad;
	RProcess().Rendezvous(waitForMainProcessLoad);
	RThread::Rendezvous(KErrNone); //Signal to main thread that modified but not rolled back yet	
	User::WaitForRequest(waitForMainProcessLoad);
	
	cmdbSession->RollbackTransactionL();
	
	CleanupStack::PopAndDestroy(record);
	CleanupStack::PopAndDestroy(cmdbSession);
	
	return KErrNone; //This will signal to main thread that rolled back 
	}	
//----------------------------------------------------------------------------------------------------------------


