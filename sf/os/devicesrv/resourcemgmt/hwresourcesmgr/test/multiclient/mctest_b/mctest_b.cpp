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

#include "mctest_b.h"
#include <e32cons.h>

_LIT(KLogFile,"C:\\logs\\mctest_b.log");
_LIT8(KCrLf,"\r\n");
_LIT8(KRTest,"RTEST:");

CHwrmMcTest_B* CHwrmMcTest_B::NewL(CConsoleBase& aConsole)
	{
	CHwrmMcTest_B* self = NewLC(aConsole);
	CleanupStack::Pop(self);
	return self;
	}

CHwrmMcTest_B* CHwrmMcTest_B::NewLC(CConsoleBase& aConsole)
	{
	CHwrmMcTest_B* self = new (ELeave) CHwrmMcTest_B(aConsole);
	CleanupStack::PushL(self);
	self->ConstructL();
	return self;
	}

CHwrmMcTest_B::CHwrmMcTest_B(CConsoleBase& aConsole)
:	iConsole(aConsole)
	{
	}

void CHwrmMcTest_B::ConstructL()
	{
	User::LeaveIfError(iFs.Connect());
	iLogStream.Replace(iFs,KLogFile,EFileWrite | EFileShareReadersOrWriters | EFileStreamText);

	iTestController = CHwrmTestCaseController::NewL(*this);
	}
	
CHwrmMcTest_B::~CHwrmMcTest_B()
	{
	delete iTestController;
	iLogStream.Close();
	iFs.Close();
	}
	
void CHwrmMcTest_B::RunMulticlientTestCases_B()
	{
	iTestController->RunSlaveMode();
	}
	
void CHwrmMcTest_B::InfoMessage(const TDesC& aMessage)
	{
	iConsole.Printf(aMessage);
	iConsole.Printf(_L("\r\n"));
	}
	
void CHwrmMcTest_B::LogMessage(const TDesC& aMessage)
	{
	HBufC8* buf = HBufC8::NewLC(aMessage.Length());
	buf->Des().Copy(aMessage);
	iLogStream.WriteL(KRTest);
	iLogStream.WriteL(*buf);
	iLogStream.WriteL(KCrLf);
	iLogStream.CommitL();
	CleanupStack::PopAndDestroy(buf);
	}

void CHwrmMcTest_B::DebugMessage(const TDesC& /*aMessage*/)
	{
	}
