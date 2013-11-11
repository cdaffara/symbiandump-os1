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

#include "mctest_f.h"
#include <e32cons.h>

_LIT(KLogFile,"C:\\logs\\testexecute\\mctest_f.htm");
_LIT8(KCrLf,"\r\n");
_LIT8(KRTest,"RTEST:");

CHwrmMcTest_F* CHwrmMcTest_F::NewL(CConsoleBase& aConsole)
	{
	CHwrmMcTest_F* self = NewLC(aConsole);
	CleanupStack::Pop(self);
	return self;
	}

CHwrmMcTest_F* CHwrmMcTest_F::NewLC(CConsoleBase& aConsole)
	{
	CHwrmMcTest_F* self = new (ELeave) CHwrmMcTest_F(aConsole);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CHwrmMcTest_F::CHwrmMcTest_F(CConsoleBase& aConsole)
:	iConsole(aConsole)
	{
	}
		
void CHwrmMcTest_F::ConstructL()
	{
	User::LeaveIfError(iFs.Connect());
	iLogStream.Replace(iFs,KLogFile,EFileWrite | EFileShareReadersOrWriters | EFileStreamText);

	iTestController = CHwrmTestCaseController::NewL(*this);
	}
	
CHwrmMcTest_F::~CHwrmMcTest_F()
	{
	delete iTestController;
	iLogStream.Close();
	iFs.Close();
	}
	
void CHwrmMcTest_F::RunMulticlientTestCases()
	{
	iTestController->RunMulticlientTestCases();
	}
	
void CHwrmMcTest_F::InfoMessage(const TDesC& aMessage)
	{
	iConsole.Printf(aMessage);
	iConsole.Printf(_L("\r\n"));
	}
	
void CHwrmMcTest_F::LogMessage(const TDesC& aMessage)
	{
	HBufC8* buf = HBufC8::NewLC(aMessage.Length());
	buf->Des().Copy(aMessage);
	iLogStream.WriteL(KRTest);
	iLogStream.WriteL(*buf);
	iLogStream.WriteL(KCrLf);
	iLogStream.CommitL();
	CleanupStack::PopAndDestroy(buf);
	}

void CHwrmMcTest_F::DebugMessage(const TDesC& /*aMessage*/)
	{
	}
