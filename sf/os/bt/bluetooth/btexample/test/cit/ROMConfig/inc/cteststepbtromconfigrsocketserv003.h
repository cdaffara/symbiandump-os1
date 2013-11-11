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

#ifndef CTESTSTEPBTROMCONFIGRSOCKETSERV003_H
#define CTESTSTEPBTROMCONFIGRSOCKETSERV003_H

#include "cteststepbtromconfigbase.h"

class CTestStepBtRomConfigRSocketServ003 : public CTestStepBtRomConfigBase
	{
public:
	static CTestStepBtRomConfigRSocketServ003* New(CTestServer& aParent);
	~CTestStepBtRomConfigRSocketServ003();
	TVerdict doTestStepL();
	
private:
	CTestStepBtRomConfigRSocketServ003(CTestServer& aParent);
	};

_LIT(KTestNameRSocketServ003, "BT_ROMCONFIG_RSocketServ_003"); 
#endif //  CTESTSTEPBTROMCONFIGRSOCKETSERV003_H

// EOF
