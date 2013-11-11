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

#include "hwrmtests.h"
#include <e32cons.h>

_LIT(KDirectoryPath, "C:\\logs\\");
_LIT(KLogFile,"C:\\logs\\testexecute\\hwrmtests.htm");
_LIT8(KCrLf,"\r\n");
_LIT8(KRTest,"RTEST:");

CHwrmTests* CHwrmTests::NewL(CConsoleBase& aConsole)
	{
	CHwrmTests* self = NewLC(aConsole);
	CleanupStack::Pop(self);
	return self;
	}

CHwrmTests* CHwrmTests::NewLC(CConsoleBase& aConsole)
	{
	CHwrmTests* self = new (ELeave) CHwrmTests(aConsole);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CHwrmTests::CHwrmTests(CConsoleBase& aConsole)
:	iConsole(aConsole)
	{
	}
		
void CHwrmTests::ConstructL()
	{
	User::LeaveIfError(iFs.Connect());
    
    //If c:\logs folder is not created, create it.
    const TInt err = iFs.MkDirAll(KDirectoryPath);
    if(err != KErrNone && err != KErrAlreadyExists)
        {
        User::Leave(err);
        }
    
	iLogStream.Replace(iFs,KLogFile,EFileWrite | EFileShareReadersOrWriters | EFileStreamText);

	iTestController = CHwrmTestCaseController::NewL(*this);
	}
	
CHwrmTests::~CHwrmTests()
	{
	delete iTestController;
	iLogStream.Close();
	iFs.Close();
	}

void CHwrmTests::RunTests()
	{
//	__UHEAP_MARK;
	RunBasicTests_SDK();
//	__UHEAP_MARKEND;
	
//	__UHEAP_MARK;
	RunBasicTests_Extended();
//	__UHEAP_MARKEND;

//	__UHEAP_MARK;
	RunBasicTests_Enhanced();
//	__UHEAP_MARKEND;


//	__UHEAP_MARK;
	RunVibraTests();
//	__UHEAP_MARKEND;
	
//	__UHEAP_MARK;
	RunInvalidParamTests();
//	__UHEAP_MARKEND;
	
//	__UHEAP_MARK;
	RunWaitChargingNotifyTests();
//	__UHEAP_MARKEND;
	}

void CHwrmTests::RunBasicTests_SDK()
	{
	InfoAndLogMessage(_L("*** Starting Basic Tests (using SDK API) ***"));
	iTestController->RunBasicTestCases(EHWRMApiTypeSAPI);
	InfoAndLogMessage(_L("*** Finished Basic tests (using SDK API) ***"));
	}
	
void CHwrmTests::RunBasicTests_Extended()
	{
	InfoAndLogMessage(_L("*** Starting Basic Tests (using Extended API) ***"));
	iTestController->RunBasicTestCases(EHWRMApiTypeXAPI);
	InfoAndLogMessage(_L("*** Finished Basic Tests (using Extended API) ***"));
	}

void CHwrmTests::RunBasicTests_Enhanced()
	{
	InfoAndLogMessage(_L("*** Starting Basic Tests (using Enhanced API) ***"));
	iTestController->RunBasicTestCases(EHWRMApiTypeEAPI);
	InfoAndLogMessage(_L("*** Finished Basic Tests (using Enhanced API) ***"));
	}

	
void CHwrmTests::RunVibraTests()
	{
	InfoAndLogMessage(_L("*** Starting Malicious Vibra Tests ***"));
	iTestController->RunTestCases( EMaliciousVibraTest );
	InfoAndLogMessage(_L("*** Finished Malicious Vibra Tests ***"));
	}
	
void CHwrmTests::RunInvalidParamTests()
	{
	InfoAndLogMessage(_L("*** Starting Invalid Parameter Tests ***"));
	iTestController->RunTestCases(ERepeatInvalidParamsTest);
	InfoAndLogMessage(_L("*** Finished Invalid Parameter Tests ***"));
	}
	
void CHwrmTests::RunWaitChargingNotifyTests()
	{
	InfoAndLogMessage(_L("*** Starting Wait Charging Notifications Tests ***"));
	iTestController->RunTestCases( EWaitForChargingNotify );
	InfoAndLogMessage(_L("*** Finished Wait Charging Notifications Tests ***"));
	}
	
void CHwrmTests::InfoMessage(const TDesC& aMessage)
	{
	iConsole.Printf(aMessage);
	iConsole.Printf(_L("\r\n"));
	}
	
void CHwrmTests::LogMessage(const TDesC& aMessage)
	{
	TRAP_IGNORE(DoLogMessageL(aMessage));
	}

void CHwrmTests::DoLogMessageL(const TDesC& aMessage)	
	{
//	__UHEAP_MARK;	
	HBufC8* buf = HBufC8::NewLC(aMessage.Length());
	buf->Des().Copy(aMessage);
	iLogStream.WriteL(KRTest);
	iLogStream.WriteL(*buf);
	iLogStream.WriteL(KCrLf);
	iLogStream.CommitL();
	CleanupStack::PopAndDestroy(buf);
//	__UHEAP_MARKEND;
	}

void CHwrmTests::DebugMessage(const TDesC& /*aMessage*/)
	{
	}

void CHwrmTests::InfoAndLogMessage(const TDesC& aMessage)
	{
	LogMessage(aMessage);
	InfoMessage(aMessage);
	}
