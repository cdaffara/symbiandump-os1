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
//

/**
 @file
 @test
 @internalComponent - Internal Symbian test code   
*/

#ifndef __SYSMONTESTHELPER_H__
#define __SYSMONTESTHELPER_H__

#include <bacline.h> 		// Command line arguments parser
#include <sysmonclisess.h>	//SysMon Client
#include <startup.hrh>
#include <startupproperties.h>
#include <e32std.h>
#include <e32err.h>
#include <e32base.h>

#include <e32debug.h> 		// Debug output


_LIT(KFilenameDeregTimeout, "tsysmon_app_deregister.exe");
_LIT(KFilenameRegisterTwice1, "tsysmon_app_registertwice1proc.exe");
_LIT(KFilenameRegisterTwice2, "tsysmon_app_registertwice2procs.exe");
_LIT(KFilenameDoNothing, "tsysmon_app_donothing.exe");

_LIT(KResultsRoot, "C:\\testdata\\results\\");
_LIT(KResultsRootDereg, "C:\\testdata\\results\\dereg_");
_LIT(KRunCountsRoot, "C:\\testdata\\runcounts\\");
_LIT(KTxtExtension, ".txt");

_LIT(KRegisterCountFile, "C:\\Register_ResultFile.txt");
_LIT(KRestartCountFile, "C:\\Restart_ResultFile.txt");

const TInt KMaxTestIdLength = 20; // TestId is of the format APPFWK-SYSMON-00xx

class CSysMonTestHelper : public CBase
	{
public:
	IMPORT_C static TRecoveryMethod StringToRecoveryMethodL(const TDesC& aRecoveryMethodString);
	IMPORT_C static void GetResultIntL(const TDesC& aTestId, TInt& aResult);
	IMPORT_C static void GetDeregResultIntL(const TDesC& aTestId, TInt& aResult);
	IMPORT_C static TInt ReadRunCountL(const TDesC& aProcessFilename);
	IMPORT_C static void IncrementRunCountL(const TDesC& aProcessFilename);
	IMPORT_C static TInt GetRestartCountL();
	IMPORT_C static TInt GetRegisterCountL();
	
	IMPORT_C static CSysMonTestHelper* NewLC();
	IMPORT_C static CSysMonTestHelper* NewL();
	IMPORT_C ~CSysMonTestHelper();
	
	IMPORT_C void GetRecoveryMethodArgsL(TRecoveryMethod& aRecoveryMethod, TInt& aRecoveryMode);
	IMPORT_C void GetTimeoutArgL(TInt& aTimeout);
	IMPORT_C void WriteResultL(const TInt& aResult);
	IMPORT_C void WriteDeregResultL(const TInt& aResult);
	IMPORT_C void GetTestId(TDes& aTestId);
	
	IMPORT_C void TestErrorcodeL(TInt& aErrorcode);
	
private:
	void ConstructL();
	CSysMonTestHelper();

private:
	TBufC<KMaxTestIdLength> iTestId; 
	CCommandLineArguments* iArgs;
	
	};
	
#endif  // __SYSMONTESTHELPER_H__
