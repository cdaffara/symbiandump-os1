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

#ifndef CTESTSTEPBTROMCONFIGUSER004_H
#define CTESTSTEPBTROMCONFIGUSER004_H

#include "cteststepbtromconfigbase.h"

class CTestStepBtRomConfigUser004 : public CTestStepBtRomConfigBase
	{
public:
	static CTestStepBtRomConfigUser004* New(CTestServer& aParent);
	~CTestStepBtRomConfigUser004();
	TVerdict doTestStepL();
	
private:
	CTestStepBtRomConfigUser004(CTestServer& aParent);
	};

_LIT(KTestNameUser004, "BT_ROMCONFIG_USER_004"); 
#endif //  CTESTSTEPBTROMCONFIGUSER004_H

// EOF
