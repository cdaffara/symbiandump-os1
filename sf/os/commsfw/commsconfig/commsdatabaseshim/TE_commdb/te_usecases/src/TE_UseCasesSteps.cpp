//
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
//

// TE_UseCasesSteps.CPP
//
//

// Use cases as in the commdb design overview

#include <e32test.h>
#include <f32file.h>
#include <commdb.h>
#include <cdbover.h>
#include <cdbtemp.h>

#include "TE_UseCasesSteps.h"

///
CTE_CreatingDatabaseAndView::CTE_CreatingDatabaseAndView()
	{
	SetTestStepName(KCreatingDatabaseAndView);
	}

CTE_CreatingDatabaseAndView::~CTE_CreatingDatabaseAndView()
	{	
	}

enum TVerdict CTE_CreatingDatabaseAndView::doTestStepL()
	{
		
	CCommsDbTableView* view;
	CCommsDatabase* db = CCommsDatabase::NewL();
	const TPtrC kTable1(DIAL_OUT_ISP);
	const TPtrC kColumn1(ISP_TYPE);
	view = db->OpenViewMatchingUintLC(kTable1, kColumn1, EIspTypeInternetOnly);
	CleanupStack::Pop(view);

	delete view; 
	delete db;		

        SetTestStepResult(EPass);	return TestStepResult();
	}

///
CTE_UsecaseReading::CTE_UsecaseReading()
	{
	SetTestStepName(KUsecaseReading);
	}

CTE_UsecaseReading::~CTE_UsecaseReading()
	{	
	}

enum TVerdict CTE_UsecaseReading::doTestStepL()
	{

	CCommsDbTableView* view;
	CCommsDatabase* db = CCommsDatabase::NewL();
	const TPtrC kTable1(DIAL_OUT_ISP);
	const TPtrC kColumn1(ISP_TYPE);
	view = db->OpenViewMatchingUintLC(kTable1, kColumn1, EIspTypeInternetOnly);
	CleanupStack::Pop(view);	
		
	const TPtrC kColumn(ISP_DESCRIPTION);

	TBuf<KCommsDbSvrMaxFieldLength> description;

	view->GotoFirstRecord();
	view->ReadTextL(kColumn, description);

	delete view; 
	delete db;
	
        SetTestStepResult(EPass);	return TestStepResult();
	}

///
CTE_UsecaseWriting::CTE_UsecaseWriting()
	{
	SetTestStepName(KUsecaseWriting);
	}

CTE_UsecaseWriting::~CTE_UsecaseWriting()
	{	
	}

enum TVerdict CTE_UsecaseWriting::doTestStepL()
	{
	
	CCommsDbTableView* view;
	CCommsDatabase* db = CCommsDatabase::NewL();
	const TPtrC kTable1(DIAL_OUT_ISP);
	const TPtrC kColumn1(ISP_TYPE);
	view = db->OpenViewMatchingUintLC(kTable1, kColumn1, EIspTypeInternetOnly);
	CleanupStack::Pop(view);    
	
	const TPtrC kColumn(ISP_DESCRIPTION);
	const TPtrC kDescription(_L("A new description put in by t_usecases"));

	TInt ret = view->GotoNextRecord();
	if(ret == KErrNone)
		{
		ret = view->UpdateRecord();
		if(ret == KErrNone)
			{
			view->WriteTextL(kColumn, kDescription);
			view->PutRecordChanges();
			}
		}

	delete view; 
	delete db;

        SetTestStepResult(EPass);	return TestStepResult();
	}

///
CTE_UsecaseOverridingATableEntry::CTE_UsecaseOverridingATableEntry()
	{
	SetTestStepName(KUsecaseOverridingATableEntry);
	}

CTE_UsecaseOverridingATableEntry::~CTE_UsecaseOverridingATableEntry()
	{	
	}

enum TVerdict CTE_UsecaseOverridingATableEntry::doTestStepL()
	{
	const TPtrC kTable(DIAL_OUT_ISP);
	const TPtrC kColumn(ISP_DESCRIPTION);
	const TPtrC kOverride(_L("An overriden description"));
	TBuf<KCommsDbSvrMaxFieldLength> overridenValue;

	CCommDbOverrideSettings *override;
	override = CCommDbOverrideSettings::NewL(CCommDbOverrideSettings::EParamListPartial);
	CleanupStack::PushL(override);

	override->SetDesOverride(kTable, kColumn, kOverride);

	if (override->GetDesOverride(kTable, kColumn, overridenValue) != KErrNone)
		{
		User::Leave(__LINE__);
		}
	if (overridenValue != kOverride)
		{
		User::Leave(__LINE__);
		}

	CleanupStack::PopAndDestroy(override);

        SetTestStepResult(EPass);	return TestStepResult();
	}

///
CTE_UsecaseOverridingThePreferenceTable::CTE_UsecaseOverridingThePreferenceTable()
	{
	SetTestStepName(KUsecaseOverridingThePreferenceTable);
	}

CTE_UsecaseOverridingThePreferenceTable::~CTE_UsecaseOverridingThePreferenceTable()
	{	
	}

enum TVerdict CTE_UsecaseOverridingThePreferenceTable::doTestStepL()
	{
	CCommsDbConnectionPrefTableView::TCommDbIapConnectionPref overrideSettings;
	CCommsDbConnectionPrefTableView::TCommDbIapConnectionPref overridenValue;

	CCommDbOverrideSettings* override;
	override = CCommDbOverrideSettings::NewL(CCommDbOverrideSettings::EParamListPartial);
	CleanupStack::PushL(override);

	overrideSettings.iDialogPref = ECommDbDialogPrefPrompt;
	overrideSettings.iRanking = 1;
	overrideSettings.iDirection = ECommDbConnectionDirectionOutgoing;
	override->SetConnectionPreferenceOverride(overrideSettings);

	overridenValue.iRanking = 1;
	override->GetConnectionPreferenceOverride(overridenValue);

	if (!(overrideSettings == overridenValue))
		{
		User::Leave(__LINE__);
		}

	CleanupStack::PopAndDestroy(override);

        SetTestStepResult(EPass);	return TestStepResult();
	}

///
CTE_UsecaseUsingATemplate::CTE_UsecaseUsingATemplate()
	{
	SetTestStepName(KUsecaseUsingATemplate);
	}

CTE_UsecaseUsingATemplate::~CTE_UsecaseUsingATemplate()
	{	
	}

enum TVerdict CTE_UsecaseUsingATemplate::doTestStepL()
	{

	CCommsDbTableView* view;
	CCommsDatabase* db = CCommsDatabase::NewL();
	const TPtrC kTable1(DIAL_OUT_ISP);
	const TPtrC kColumn1(ISP_TYPE);
	view = db->OpenViewMatchingUintLC(kTable1, kColumn1, EIspTypeInternetOnly);
	CleanupStack::Pop(view);   
	
	const TPtrC kTable(DIAL_OUT_ISP);
	const TPtrC kColumn(ISP_DESCRIPTION);
	const TPtrC kDescription(_L("A default description put in by t_usecases"));

	CCommsDbTemplateRecord* temp;
	temp = CCommsDbTemplateRecord::NewL(db, kTable);
	CleanupStack::PushL(temp);

	temp->Modify();
	temp->WriteTextL(kColumn, kDescription);
	temp->StoreModifications();
	
	CleanupStack::PopAndDestroy(temp);
	 
	delete view; 
	delete db;
	
        SetTestStepResult(EPass);	return TestStepResult();
	}
