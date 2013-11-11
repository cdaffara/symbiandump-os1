// Copyright (c) 1997-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include <e32test.h>
#include <etelmm.h>
#include "Te_LoopBackcnocmgf.h"
#include "../../hayes/TSYCONFG.H" // for KInternetAccessPoint

//
// Test-side class
// With the assistance of the base class, this class must start the emulator
// and drive the ETel API.
//
CTestDriveNoCmgf* CTestDriveNoCmgf::NewL(const TScriptList aScriptListEntry)
	{
	CTestDriveNoCmgf* cmgf=new(ELeave) CTestDriveNoCmgf(aScriptListEntry);
	CleanupStack::PushL(cmgf);
	cmgf->ConstructL();
	CleanupStack::Pop();
	return cmgf;
	}

CTestDriveNoCmgf::CTestDriveNoCmgf(const TScriptList aScriptListEntry) : iScriptListEntry(aScriptListEntry)
	{}

TInt CTestDriveNoCmgf::RunTestL()
	{
	iCurrentScript=iScriptListEntry;
	return StartEmulatorL();
	}

TInt CTestDriveNoCmgf::DriveETelApiL()
/**
 * This file contains a slightly different initialisation sequence. This iPhone does not
 * return a response to the AT+CMGF=? Query.
 * @return KErrNone
 */
	{
	// Open and initialise the iPhone object
	_LIT(KMmPhoneName,"GsmPhone1");
	TESTL(iCmgfPhone.Open(iServer,KMmPhoneName)==KErrNone);

	TRequestStatus reqStatus;
	RMobilePhone::TMMTableSettings tableSettings;
	tableSettings.iLocId=KInternetAccessPoint;
	RMobilePhone::TMMTableSettingsPckg tableSettingsPckg(tableSettings);
	iCmgfPhone.InitialiseMM(reqStatus , tableSettingsPckg); 	
	User::WaitForRequest(reqStatus);
	TESTL(reqStatus == KErrNone);

	// Close the iPhone
	iCmgfPhone.Close();		
	return KErrNone;
	}


//
// Emulator-side class
// With the assistance of the base class, this class must run the designated script
//
CTestNoCmgf* CTestNoCmgf::NewL(const TScript* aScript)
	{
	CTestNoCmgf* cmgf=new(ELeave) CTestNoCmgf(aScript);
	CleanupStack::PushL(cmgf);
	cmgf->ConstructL();
	CleanupStack::Pop();
	return cmgf;
	}

CTestNoCmgf::CTestNoCmgf(const TScript* aScript) : iScript(aScript)
	{}

void CTestNoCmgf::ConstructL()
	{
	CATScriptEng::ConstructL();
	}

TInt CTestNoCmgf::Start()
	{
	StartScript(iScript);
	return KErrNone;
	}

void CTestNoCmgf::SpecificAlgorithmL(TInt /* aParam */)
	{
	}

void CTestNoCmgf::Complete(TInt aError)
	{
	iReturnValue=aError;
	CActiveScheduler::Stop();
	}
