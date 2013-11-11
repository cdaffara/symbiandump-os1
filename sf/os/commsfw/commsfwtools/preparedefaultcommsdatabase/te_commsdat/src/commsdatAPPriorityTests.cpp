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
// Example CTestStep derived implementation
// 
//

/**
 @file commsdatAPPriorityTests.cpp
*/
#include "commsdatAPPriorityTests.h"
#include "Te_commsdatSuiteDefs.h"
#include <e32test.h>



//----------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------

CCommsdat500Step::~CCommsdat500Step()
/**
 * Destructor
 */
	{
	delete iCDSession;
	}

CCommsdat500Step::CCommsdat500Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat500Step);                                                                                           
	}

TVerdict CCommsdat500Step::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 */
	{
	SetTestStepResult(EFail);
	iCDSession = CMDBSession::NewL(CMDBSession::LatestVersion()); 
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TBool CCommsdat500Step::CheckPrioritiesL(CMDBSession& aCDSession)
	{
	const TInt KAPRecordsCount=19;
	// use lowest priority (KMaxTUint) for case when no priority field has been specified for AP record in cfg file
	TUint priorities[KAPRecordsCount] = {5, 1, 4, KMaxTUint, KMaxTUint, KMaxTUint, 3, KMaxTUint,
										 KMaxTUint, 2, KMaxTUint, KMaxTUint, 6, 10, KMaxTUint, KMaxTUint,
										 9, KMaxTUint, KMaxTUint
										};
	
	CMDBRecordSet<CCDAccessPointRecord>* ptrAccessPointRecordSet = new (ELeave) CMDBRecordSet<CCDAccessPointRecord>(KCDTIdAccessPointRecord);
	CleanupStack::PushL(ptrAccessPointRecordSet);
	ptrAccessPointRecordSet->LoadL(aCDSession);
	TInt totalcount = ptrAccessPointRecordSet->iRecords.Count();
	if (totalcount != KAPRecordsCount)
		{
		CleanupStack::PopAndDestroy(ptrAccessPointRecordSet);
		return EFalse;
		} 
	for (TInt i = 0; i<totalcount; i++)
		{
		CCDAccessPointRecord* apRecord = static_cast<CCDAccessPointRecord *>(ptrAccessPointRecordSet->iRecords[i]);
		if (apRecord->iPriority.IsNull() && (priorities[i] != KMaxTUint) ||
			!apRecord->iPriority.IsNull() && (apRecord->iPriority != priorities[i]))
			{
			CleanupStack::PopAndDestroy(ptrAccessPointRecordSet);
			return EFalse;
			}
		}

	CleanupStack::PopAndDestroy(ptrAccessPointRecordSet);
	return ETrue;
	}

TVerdict CCommsdat500Step::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{	
	SetTestStepResult(EFail);
	if (CheckPrioritiesL(*iCDSession))
		{
		INFO_PRINTF1(_L("Access points priorities are correct"));
		SetTestStepResult(EPass);		
		}

	return TestStepResult();
	}
	
//----------------------------------------------------------------------------------------------------------------
#ifndef __TOOLS2__
// Test is not suitable for tools2 build.

CCommsdat501Step::~CCommsdat501Step()
/**
 * Destructor
 */
	{
	delete iCDSession;
	}

CCommsdat501Step::CCommsdat501Step()
/**
 * Constructor
 */
	{
	SetTestStepName(KCommsdat501Step);                                                                                           
	}

TVerdict CCommsdat501Step::doTestStepPreambleL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 */
	{
	SetTestStepResult(EFail);
	iCDSession = CMDBSession::NewL(CMDBSession::LatestVersion()); 
	SetTestStepResult(EPass);
	return TestStepResult();
	}

TBool CCommsdat501Step::LaunchProcess(const TDesC& aProcName, const TDesC& aParams)
	{
	RProcess process;
	TInt err = process.Create(aProcName, aParams);
	if(err != KErrNone)
		{
		process.Close();
		return EFalse;
		}
	
	TRequestStatus reqStatus;
	process.Rendezvous(reqStatus);
	process.Resume();

	User::WaitForRequest(reqStatus);
	if(reqStatus.Int() != KErrNone)
		{
		process.Close();
		return EFalse;
		}

	process.Close();
	
	return ETrue;
	}

TVerdict CCommsdat501Step::doTestStepL()
/**
 * @return - TVerdict code
 * Override of base class pure virtual
 * Our implementation only gets called if the base class doTestStepPreambleL() did
 * not leave. That being the case, the current test result value will be EPass.
 */
	{	
	SetTestStepResult(EFail);
	
	_LIT(ceddumpFile,"ceddump.exe");
	_LIT(cedFile,"ced.exe");
	_LIT(cedParams,"c:\\cedout.cfg");
	_LIT(procLaunchErr, "Can't launch %S");
	
	if (!LaunchProcess(ceddumpFile, _L("")))
		{
		INFO_PRINTF2(procLaunchErr, &ceddumpFile);
		return TestStepResult();
		}
		
	if (!LaunchProcess(cedFile, cedParams))
		{
		INFO_PRINTF2(procLaunchErr, &cedFile);
		return TestStepResult();
		}

	if (CCommsdat500Step::CheckPrioritiesL(*iCDSession))
		{
		INFO_PRINTF1(_L("Access points priorities are correct"));
		SetTestStepResult(EPass);
		}
	return TestStepResult();
	}
#endif // __TOOLS2__
	
//----------------------------------------------------------------------------------------------------------------


