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

#ifndef CTESTSTEPBTROMCONFIGSOCKET001_H
#define CTESTSTEPBTROMCONFIGSOCKET001_H

#include "cteststepbtromconfigbase.h"

class CTestStepBtRomConfigRSocket001 : public CTestStepBtRomConfigBase
	{
public:
	static CTestStepBtRomConfigRSocket001* New(CTestServer& aParent);
	~CTestStepBtRomConfigRSocket001();
	TVerdict doTestStepL();
	
private:
	CTestStepBtRomConfigRSocket001(CTestServer& aParent);
	};

_LIT(KTestNameRSocket001, "BT_ROMCONFIG_RSocket_001"); 
#endif //  CTESTSTEPBTROMCONFIGSOCKET001_H

// EOF
