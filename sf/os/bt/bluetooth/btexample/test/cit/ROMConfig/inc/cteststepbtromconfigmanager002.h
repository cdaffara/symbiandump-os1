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

#ifndef CTESTSTEPBTROMCONFIGMANAGER002_H
#define CTESTSTEPBTROMCONFIGMANAGER002_H

#include "cteststepbtromconfigbase.h"

class CTestStepBtRomConfigManager002 : public CTestStepBtRomConfigBase
	{
public:
	static CTestStepBtRomConfigManager002* New(CTestServer& aParent);
	~CTestStepBtRomConfigManager002();
	TVerdict doTestStepL();
	
private:
	CTestStepBtRomConfigManager002(CTestServer& aParent);
	};

_LIT(KTestNameManager002, "BT_ROMCONFIG_MANAGER_002"); 
#endif //  CTESTSTEPBTROMCONFIGMANAGER002_H

// EOF
