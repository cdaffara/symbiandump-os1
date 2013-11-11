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

#ifndef CTESTSTEPBTROMCONFIGRCOMMSERV001_H
#define CTESTSTEPBTROMCONFIGRCOMMSERV001_H

#include "cteststepbtromconfigbase.h"

class CTestStepBtRomConfigRCommServ001 : public CTestStepBtRomConfigBase
	{
public:
	static CTestStepBtRomConfigRCommServ001* New(CTestServer& aParent);
	~CTestStepBtRomConfigRCommServ001();
	TVerdict doTestStepL();
	
private:
	CTestStepBtRomConfigRCommServ001(CTestServer& aParent);
	};

_LIT(KTestNameRCommServ001, "BT_ROMCONFIG_RCommServ_001"); 
#endif //  CTESTSTEPBTROMCONFIGRCOMMSERV001_H

// EOF
