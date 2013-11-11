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

// TE_ConnPrefSteps.cpp
//
//

#include <commdbconnpref.h>
#include <e32test.h>

#include "TE_ConnPrefSteps.h"

CTE_ConstructAndVersion::CTE_ConstructAndVersion()
	{
	SetTestStepName(KConstructAndVersion);
	}

CTE_ConstructAndVersion::~CTE_ConstructAndVersion()
	{	
	}

enum TVerdict CTE_ConstructAndVersion::doTestStepL()
	{

	TCommDbConnPref connPref;

	Test(connPref.ExtensionId() == TConnPref::EConnPrefCommDb);		
	TVersion ver = connPref.Version();
	Test(ver.iMajor == TCommDbConnPref::KMajorVersionNumber);		
	Test(ver.iMinor == TCommDbConnPref::KMinorVersionNumber);		
	Test(ver.iBuild == TCommDbConnPref::KBuildVersionNumber);
	Test(connPref.BearerSet() == KCommDbBearerUnknown);		
	Test(connPref.DialogPreference() == ECommDbDialogPrefUnknown);		
	Test(connPref.Direction() == ECommDbConnectionDirectionUnknown);
	Test(connPref.IapId() == 0);		
	Test(connPref.NetId() == 0);
	
        SetTestStepResult(EPass);	return TestStepResult();
	}

CTE_SetAndGetConnPrefs::CTE_SetAndGetConnPrefs()
	{
	SetTestStepName(KSetAndGetConnPrefs); 
	}

CTE_SetAndGetConnPrefs::~CTE_SetAndGetConnPrefs()
	{	
	}

enum TVerdict CTE_SetAndGetConnPrefs::doTestStepL()
	{

	TCommDbConnPref connPref;

	connPref.SetBearerSet((KCommDbBearerWcdma | KCommDbBearerLAN));
	Test(connPref.BearerSet() == (KCommDbBearerWcdma | KCommDbBearerLAN));

	connPref.SetDialogPreference(ECommDbDialogPrefDoNotPrompt);
	Test(connPref.DialogPreference() == ECommDbDialogPrefDoNotPrompt);

	connPref.SetDirection(ECommDbConnectionDirectionOutgoing);
	Test(connPref.Direction() == ECommDbConnectionDirectionOutgoing);

	connPref.SetIapId(123);
	Test(connPref.IapId() == 123);

	connPref.SetNetId(456);
	Test(connPref.NetId() == 456);

        SetTestStepResult(EPass);	return TestStepResult();
	}


CTE_ComparePrefs::CTE_ComparePrefs()
	{
	SetTestStepName(KComparePrefs); 
	}

CTE_ComparePrefs::~CTE_ComparePrefs()
	{	
	}

enum TVerdict CTE_ComparePrefs::doTestStepL()
	{
	
	TCommDbConnPref connPref1;
	connPref1.SetBearerSet((KCommDbBearerWcdma | KCommDbBearerLAN));
	connPref1.SetDialogPreference(ECommDbDialogPrefDoNotPrompt);
	connPref1.SetDirection(ECommDbConnectionDirectionOutgoing);
	connPref1.SetIapId(123);
	connPref1.SetNetId(456);

	TCommDbConnPref connPref2;
	connPref2.SetBearerSet((KCommDbBearerWcdma | KCommDbBearerLAN));
	connPref2.SetDialogPreference(ECommDbDialogPrefDoNotPrompt);
	connPref2.SetDirection(ECommDbConnectionDirectionOutgoing);
	connPref2.SetIapId(123);
	connPref2.SetNetId(456);

	TCommDbConnPref connPref3;
	connPref3.SetBearerSet(KCommDbBearerCSD);
	connPref3.SetDialogPreference(ECommDbDialogPrefWarn);
	connPref3.SetDirection(ECommDbConnectionDirectionIncoming);
	connPref3.SetIapId(987);
	connPref3.SetNetId(654);

	TCommDbConnPref connPref4;
	connPref4.SetBearerSet(KCommDbBearerCSD);
	connPref4.SetDirection(ECommDbConnectionDirectionIncoming);
	connPref4.SetNetId(654);	

	Test(connPref2.Compare(connPref1) == 0);
	Test(connPref2 == connPref1);

	Test(connPref1.Compare(connPref2) == 0);
	Test(connPref1 == connPref2);

	Test(connPref1.Compare(connPref1) == 0);
	Test(connPref1 == connPref1);

	Test(connPref3.Compare(connPref1) != 0);
	Test(connPref3 != connPref1);

	Test(connPref1.Compare(connPref3) != 0);
	Test(connPref1 != connPref3);

	Test(connPref4 == connPref3);
	Test(connPref3 == connPref4);

	Test(connPref4 != connPref2);
	Test(connPref2 != connPref4);

        SetTestStepResult(EPass);	return TestStepResult();
	}

CTE_AssignAndCopyConstruct::CTE_AssignAndCopyConstruct()
	{
	SetTestStepName(KAssignAndCopyConstruct); 
	}

CTE_AssignAndCopyConstruct::~CTE_AssignAndCopyConstruct()
	{	
	}

enum TVerdict CTE_AssignAndCopyConstruct::doTestStepL()
	{

	TCommDbConnPref connPref1;
	connPref1.SetBearerSet((KCommDbBearerWcdma | KCommDbBearerLAN));
	connPref1.SetDialogPreference(ECommDbDialogPrefDoNotPrompt);
	connPref1.SetDirection(ECommDbConnectionDirectionOutgoing);
	connPref1.SetIapId(123);
	connPref1.SetNetId(456);

	TCommDbConnPref connPref2(connPref1);
	Test(connPref2.Compare(connPref1) == 0);
	Test(connPref1.Compare(connPref2) == 0);

	TCommDbConnPref connPref3 = connPref1;
	Test(connPref3.Compare(connPref1) == 0);
	Test(connPref1.Compare(connPref3) == 0);

	TCommDbConnPref connPref4;
	connPref4 = connPref1;
	Test(connPref4.Compare(connPref1) == 0);
	Test(connPref1.Compare(connPref4) == 0);

        SetTestStepResult(EPass);	return TestStepResult();
	}

CTE_ExtensionIdAndCast::CTE_ExtensionIdAndCast()
	{
	SetTestStepName(KExtensionIdAndCast); 
	}

CTE_ExtensionIdAndCast::~CTE_ExtensionIdAndCast()
	{	
	}

enum TVerdict CTE_ExtensionIdAndCast::doTestStepL()
	{

	TCommDbConnPref commdbPref1;
	commdbPref1.SetBearerSet((KCommDbBearerWcdma | KCommDbBearerLAN));
	commdbPref1.SetDialogPreference(ECommDbDialogPrefDoNotPrompt);
	commdbPref1.SetDirection(ECommDbConnectionDirectionOutgoing);
	commdbPref1.SetIapId(123);
	commdbPref1.SetNetId(456);

	TConnPref basePref(commdbPref1);
	Test(basePref.ExtensionId() == TConnPref::EConnPrefCommDb);

	TCommDbConnPref commdbPref2 = TCommDbConnPref::Cast(basePref);
	Test(commdbPref2 == commdbPref1);
	
        SetTestStepResult(EPass);	return TestStepResult();
	}


CTE_MultiConnPref::CTE_MultiConnPref()
	{
	SetTestStepName(KMultiConnPref); 
	}

CTE_MultiConnPref::~CTE_MultiConnPref()
	{	
	}

enum TVerdict CTE_MultiConnPref::doTestStepL()
	{

	TCommDbConnPref connPref1;
	connPref1.SetBearerSet((KCommDbBearerWcdma | KCommDbBearerLAN));
	connPref1.SetDialogPreference(ECommDbDialogPrefDoNotPrompt);
	connPref1.SetDirection(ECommDbConnectionDirectionOutgoing);
	connPref1.SetIapId(123);
	connPref1.SetNetId(456);

	TCommDbConnPref connPref2;
	connPref2.SetBearerSet(KCommDbBearerCSD);
	connPref2.SetDialogPreference(ECommDbDialogPrefWarn);
	connPref2.SetDirection(ECommDbConnectionDirectionIncoming);
	connPref2.SetIapId(987);
	connPref2.SetNetId(654);

	TCommDbMultiConnPref multiPref;
	TInt err(KErrNone);

	err = multiPref.SetPreference(0, connPref1);
	Test(err == KErrArgument);
	
	err = multiPref.SetPreference(1, connPref1);
	Test(err == KErrNone);

	err = multiPref.SetPreference(2, connPref2);
	Test(err == KErrNone);

	err = multiPref.SetPreference(3, connPref2);
	Test(err == KErrArgument);

	TCommDbConnPref connPref3;

	err = multiPref.GetPreference(0, connPref3);
	Test(err == KErrArgument);

	err = multiPref.GetPreference(1, connPref3);
	Test(err == KErrNone);
	Test(connPref3 == connPref1);

	err = multiPref.GetPreference(2, connPref3);
	Test(err == KErrNone);
	Test(connPref3 == connPref2);

	err = multiPref.GetPreference(3, connPref3);
	Test(err == KErrArgument);

	TConnPref basePref(multiPref);
	Test(basePref.ExtensionId() == TConnPref::EConnPrefCommDbMulti);

	TCommDbMultiConnPref multiPref2;	
	TCommDbConnPref connPref4;
	err = multiPref2.GetPreference(1, connPref4);
	Test(err == KErrNone);
	Test(connPref4 == connPref1);

	TCommDbConnPref connPref5;
	err = multiPref2.GetPreference(2, connPref5);
	Test(err == KErrNone);
	Test(connPref5 == connPref2);

	Test(multiPref.ConnectionAttempts()==TCommDbMultiConnPref::KMaxMultiConnPrefCount);

	multiPref.SetConnectionAttempts(0);
	Test(multiPref.ConnectionAttempts()==TCommDbMultiConnPref::KMaxMultiConnPrefCount);

	multiPref.SetConnectionAttempts(1);
	Test(multiPref.ConnectionAttempts()==1);

	multiPref.SetConnectionAttempts(2);
	Test(multiPref.ConnectionAttempts()==2);

	multiPref.SetConnectionAttempts(TCommDbMultiConnPref::KMaxMultiConnPrefCount+1);
	Test(multiPref.ConnectionAttempts()==TCommDbMultiConnPref::KMaxMultiConnPrefCount);

        SetTestStepResult(EPass);	return TestStepResult();
	}

////////////////////////////////////////
//	Test step CTE_MultiConnPrefVersion
/////////////////////////////////////////

// constructor
CTE_MultiConnPrefVersion::CTE_MultiConnPrefVersion()
	{
	// store the name of this test case
	SetTestStepName(KMultiConnPrefVersion);
	}

// destructor
CTE_MultiConnPrefVersion::~CTE_MultiConnPrefVersion()
	{
	}

// do Test step CTE_MultiConnPrefVersion
TVerdict CTE_MultiConnPrefVersion::doTestStepL()
	{
	INFO_PRINTF1(_L("CTE_MultiConnPrefVersion called "));
	SetTestStepResult(EFail);    TCommDbMultiConnPref pref;	
	TVersion dbVersion = pref.Version();
	INFO_PRINTF4(_L("TCommDbMultiConnPref::Version -> Major=%d, Minor=%d, Build=%d"), TCommDbConnPref::KMajorVersionNumber, TCommDbConnPref::KMinorVersionNumber, TCommDbConnPref::KBuildVersionNumber);	
	//Test for the expected return value
	if ( dbVersion.iMinor == TCommDbConnPref::KMinorVersionNumber && 
         dbVersion.iMajor == TCommDbConnPref::KMajorVersionNumber && 
         dbVersion.iBuild == TCommDbConnPref::KBuildVersionNumber )
		{
		SetTestStepResult(EPass);
		}
    return TestStepResult();
	}
	

///////////////////////////////////////////
//	Test step CTE_MultiConnectionAttempts
////////////////////////////////////////////

// constructor
CTE_MultiConnectionAttempts::CTE_MultiConnectionAttempts()
	{
	// store the name of this test case
	SetTestStepName(KMultiConnectionAttempts);
	}

// destructor
CTE_MultiConnectionAttempts::~CTE_MultiConnectionAttempts()
	{
	}	
	
// do Test step CTE_MultiConnectionAttempts
TVerdict CTE_MultiConnectionAttempts::doTestStepL( )
	{
	INFO_PRINTF1(_L("Step CTE_MultiConnectionAttempts called "));
	SetTestStepResult(EFail);
	TCommDbMultiConnPref prefPtr;	
	TInt numAttempts = prefPtr.ConnectionAttempts();
	INFO_PRINTF3(_L("Expected NumAttempts:= %d, Actual NumAttempts:=%d"), TCommDbMultiConnPref::KMaxMultiConnPrefCount,numAttempts);
	if(numAttempts != TCommDbMultiConnPref::KMaxMultiConnPrefCount )
		{
		return TestStepResult();
		}
	numAttempts	 = 3 ;
	prefPtr.SetConnectionAttempts(numAttempts);
	TInt attempsCheck = prefPtr.ConnectionAttempts();
	INFO_PRINTF3(_L("Expected NumAttempts:= %d, Actual NumAttempts:=%d"), TCommDbMultiConnPref::KMaxMultiConnPrefCount,attempsCheck);
	INFO_PRINTF1(_L("This test fails because  no. of attempts is > 2"));
	if(numAttempts == attempsCheck )
		{
		SetTestStepResult(EFail);
		}
	return TestStepResult();
	}
	
	
	
	
