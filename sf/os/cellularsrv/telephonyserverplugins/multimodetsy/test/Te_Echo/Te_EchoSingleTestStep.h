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
//

#ifndef __TE_ECHO_TESTSTEP_BASE__
#define __TE_ECHO_TESTSTEP_BASE__

#include <test/testexecutestepbase.h>
#include "Te_EchoMacros.h"

#include <etelmm.h>
#include <etelmmcs.h>

_LIT(KEchoTestStepName,"EchoTest");
const TInt KRemoteNum=12;

class CEchoTestStep : public CTestStep
	{
public:
	CEchoTestStep();
	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();
	virtual TVerdict doTestStepL();

private:
	void AnswerIncomingCallTestL();
    void DialRemoteModemTestL();

private:
	TRequestStatus iStatus;
	RTelServer iTelServer;
	RMobilePhone iPhone;
	RPhone::TCaps iPhoneCaps;
	TBuf8<KRemoteNum> iRemoteNumber;
	};

#endif // __TE_ECHO_TESTSTEP_BASE__
