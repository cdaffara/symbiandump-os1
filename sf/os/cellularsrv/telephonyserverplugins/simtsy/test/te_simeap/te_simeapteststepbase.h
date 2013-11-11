// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef _TE_SIMEAP_TESTSTEP_BASE_
#define _TE_SIMEAP_TESTSTEP_BASE_

#include <test/testexecutestepbase.h>

#include <etel.h>
#include <etelmm.h>
#include "SimTsy.h"

_LIT(KSimtsyName,"SIM");

class CSimEapTestStepBase : public CTestStep
	{
public:
 	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();

protected:
	RTelServer iTelServer;
	RMobilePhone iPhone;
	};

#endif // _TE_SIMEAP_TESTSTEP_BASE_
