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

#ifndef CTESTSTEPBTROMCONFIGSDP001_H
#define CTESTSTEPBTROMCONFIGSDP001_H

#include "cteststepbtromconfigbase.h"

class CTestStepBtRomConfigSdp001 : public CTestStepBtRomConfigBase
	{
public:
	static CTestStepBtRomConfigSdp001* New(CTestServer& aParent);
	~CTestStepBtRomConfigSdp001();
	TVerdict doTestStepL();
	
private:
	CTestStepBtRomConfigSdp001(CTestServer& aParent);
	};

_LIT(KTestNameSdp001, "BT_ROMCONFIG_SDP_001"); 
#endif //  CTESTSTEPBTROMCONFIGSDP001_H

// EOF
