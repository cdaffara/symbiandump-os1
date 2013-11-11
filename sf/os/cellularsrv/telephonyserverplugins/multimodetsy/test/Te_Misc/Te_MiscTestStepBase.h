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

#ifndef __TE_MISC_TESTSTEP_BASE__
#define __TE_MISC_TESTSTEP_BASE__

#include <test/testexecutestepbase.h>

#include <e32base.h>
#include <etel.h>
#include <etelmm.h>

_LIT(KMmtsyName,"MM");
_LIT(KPhoneName,"GsmPhone1");

class CMiscTestStep : public CTestStep
	{
public:
 	virtual TVerdict doTestStepPreambleL();
	virtual TVerdict doTestStepPostambleL();

protected:
	RTelServer iTelServer;
	RMobilePhone iPhone;
	TRequestStatus iStatus;
	};

#endif // __TE_MISC_TESTSTEP_BASE__
