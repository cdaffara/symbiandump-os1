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

#ifndef __TE_SIMMISC_TESTSTEP_BASE__
#define __TE_SIMMISC_TESTSTEP_BASE__

#include <test/testexecutestepbase.h>

#include <e32base.h>
#include <e32test.h>
#include <e32keys.h>
#include <c32comm.h>
#include <f32file.h>
#include <etel.h>
#include <etelmm.h>
#include "SimTsy.h"

_LIT(KSimtsyName,"SIM");

class CSimMiscTestStepBase : public CTestStep
	{
public:
 	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();

protected:
	RTelServer iTelServer;
	RMobilePhone iPhone;
	TRequestStatus iStatus; 
private:
	RMobilePhone iMyPhone;

	};

#endif // __TE_SIMMISC_TESTSTEP_BASE__
