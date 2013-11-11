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

#include "Te_SimNoConfigFileTest.h"

CSimNoConfigFileTest::CSimNoConfigFileTest() 
	{ 
	SetTestStepName(_L("NoConfigFileTest"));
	}

TVerdict CSimNoConfigFileTest::doTestStepL()
	{
	INFO_PRINTF1(_L("Testing Phone open without Config File present"));
	DeleteConfigFileL();
	TESTL(iPhone.Open(iTelServer,KPhoneName)==KErrNotFound);
	iPhone.Close();
	ASSERT(RThread().RequestCount()==0);
	return TestStepResult();
	}
