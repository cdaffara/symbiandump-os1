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

#ifndef CTESTSTEPBTROMCONFIGRHOSTRESOLVER001_H
#define CTESTSTEPBTROMCONFIGRHOSTRESOLVER001_H

#include "cteststepbtromconfigbase.h"
#include <es_sock.h>

class CTestStepBtRomConfigRHostResolver001 : public CTestStepBtRomConfigBase
	{
public:
	static CTestStepBtRomConfigRHostResolver001* New(CTestServer& aParent);
	~CTestStepBtRomConfigRHostResolver001();
	TVerdict doTestStepL();
	
private:
	CTestStepBtRomConfigRHostResolver001(CTestServer& aParent);
	void TestOpenHostResolver(TUint aProtocolNumber);
	
	RSocketServ iSocketServ;
	};

_LIT(KTestNameRHostResolver001, "BT_ROMCONFIG_RHostResolver_001"); 
#endif //  CTESTSTEPBTROMCONFIGRHOSTRESOLVER001_H

// EOF
