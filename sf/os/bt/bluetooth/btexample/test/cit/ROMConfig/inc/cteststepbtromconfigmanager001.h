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

#ifndef CTESTSTEPBTROMCONFIGMANAGER001_H
#define CTESTSTEPBTROMCONFIGMANAGER001_H

#include "cteststepbtromconfigbase.h"

class CTestStepBtRomConfigManager001 : public CTestStepBtRomConfigBase
	{
public:
	static CTestStepBtRomConfigManager001* New(CTestServer& aParent);
	~CTestStepBtRomConfigManager001();
	TVerdict doTestStepL();
	
private:
	CTestStepBtRomConfigManager001(CTestServer& aParent);
	};

_LIT(KTestNameManager001, "BT_ROMCONFIG_MANAGER_001"); 
#endif //  CTESTSTEPBTROMCONFIGMANAGER001_H

// EOF
