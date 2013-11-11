// Copyright (c) 2007-2010 Nokia Corporation and/or its subsidiary(-ies).
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
 @internalComponent - Internal Symbian test code  
*/



#include <s32mem.h>
#include <e32property.h>
#include "temergencycallrf_adaptationplugin_step.h"

const TUint32 KEmergencyCallRfAdaptationPluginPropertyKey = 0x2000E657;
const TUid KPropertyCategory={0x2000D75B};

//
// Run the tests
//

CTestEmergencyCallRfAdaptationPlugin::CTestEmergencyCallRfAdaptationPlugin()
	:CAdaptationTestBase(KTCTestEmergencyCallRfAdaptationPlugin)
	{
	
	}

CTestEmergencyCallRfAdaptationPlugin::~CTestEmergencyCallRfAdaptationPlugin()
	{

	}


void CTestEmergencyCallRfAdaptationPlugin::TestActivateRfForEmergencyCall()
	{
	INFO_PRINTF1(_L(" >CTestEmergencyCallRfAdaptationPlugin::TestActivateRfForEmergencyCall"));

	//TRequestStatus status;
	iSsmEmergencyCallRfAdaptation.ActivateRfForEmergencyCall(iStatus);
	User::WaitForRequest(iStatus);

	INFO_PRINTF2(_L("CTestEmergencyCallRfAdaptationPlugin::TestActivateRfForEmergencyCall completed with %d error>"), iStatus.Int());	
	}

void CTestEmergencyCallRfAdaptationPlugin::TestDeactivateRfForEmergencyCall()
	{
	INFO_PRINTF1(_L(" >CTestEmergencyCallRfAdaptationPlugin::TestDeactivateRfForEmergencyCall"));
	
	iSsmEmergencyCallRfAdaptation.DeactivateRfForEmergencyCall(iStatus);
	User::WaitForRequest(iStatus);

	INFO_PRINTF2(_L("CTestEmergencyCallRfAdaptationPlugin::TestDeactivateRfForEmergencyCall completed with %d error>"), iStatus.Int());
	}

void CTestEmergencyCallRfAdaptationPlugin::TestCancel()
	{
	INFO_PRINTF1(_L(" >CTestEmergencyCallRfAdaptationPlugin::TestCancel"));
	iSsmEmergencyCallRfAdaptation.Cancel();
	INFO_PRINTF1(_L(" CTestEmergencyCallRfAdaptationPlugin::TestCancel>"));	
	}



//from CAdaptationTestBase
TVerdict CTestEmergencyCallRfAdaptationPlugin::doTestStepL()
	{
	__UHEAP_MARK;
	
	TInt err = RProperty::Define(KPropertyCategory, KEmergencyCallRfAdaptationPluginPropertyKey, RProperty::EInt);
	TEST((KErrNone == err) || (KErrAlreadyExists == err));
	err = RProperty::Set(KPropertyCategory, KEmergencyCallRfAdaptationPluginPropertyKey, 1);
	TEST(KErrNone == err);
	
	TRAP(err, TestActivateRfForEmergencyCall());
	TEST(iStatus.Int() == KErrDisconnected);
	TEST(err == KErrNone);
	TRAP(err, TestDeactivateRfForEmergencyCall());
	TEST(err == KErrNone);
	TEST(iStatus.Int() == KErrDisconnected);
	
	User::LeaveIfError(iSsmEmergencyCallRfAdaptation.Connect());
	

	TRAP(err, TestActivateRfForEmergencyCall());
	TEST(err == KErrNone);
	TEST(iStatus.Int() == KErrNone);
	
	TRAP(err, TestDeactivateRfForEmergencyCall());
	TEST(err == KErrNone);
	TEST(iStatus.Int() == KErrNone);
	TestCancel();
	//TestRelease();			// have to test this part too ...

	err = RProperty::Delete(KPropertyCategory, KEmergencyCallRfAdaptationPluginPropertyKey);
	TEST(KErrNone == err);
	__UHEAP_MARKEND;

	return TestStepResult();
	}
