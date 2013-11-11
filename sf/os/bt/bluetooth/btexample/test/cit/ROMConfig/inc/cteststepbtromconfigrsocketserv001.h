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

#ifndef CTESTSTEPIRDAROMCONFIGRSOCKETSERV001_H
#define CTESTSTEPIRDAROMCONFIGRSOCKETSERV001_H

#include "cteststepbtromconfigbase.h"
#include <es_sock.h>

class CTestStepBtRomConfigRSocketServ001 : public CTestStepBtRomConfigBase
	{
public:
	static CTestStepBtRomConfigRSocketServ001* New(CTestServer& aParent);
	~CTestStepBtRomConfigRSocketServ001();
	TVerdict doTestStepL();
	
private:
	CTestStepBtRomConfigRSocketServ001(CTestServer& aParent);
	
	void TestFindProtocol(const TDesC& aProtocol);
	
	RSocketServ iSocketServ;
	};

_LIT(KTestNameRSocketServ001, "BT_ROMCONFIG_RSocketServ_001"); 
#endif //  CTESTSTEPIRDAROMCONFIGRSOCKETSERV001_H

// EOF
