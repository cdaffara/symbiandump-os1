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

#ifndef CTESTSTEPBTROMCONFIGRSOCKETSERV002_H
#define CTESTSTEPBTROMCONFIGRSOCKETSERV002_H

#include "cteststepbtromconfigbase.h"
#include <es_sock.h>

class CTestStepBtRomConfigRSocketServ002 : public CTestStepBtRomConfigBase
	{
public:
	static CTestStepBtRomConfigRSocketServ002* New(CTestServer& aParent);
	~CTestStepBtRomConfigRSocketServ002();
	TVerdict doTestStepL();
	
private:
	CTestStepBtRomConfigRSocketServ002(CTestServer& aParent);	
	void TestStartProtocol(TUint aPacketType, TUint aProtocolNumber);
	
	RSocketServ iSocketServ;
	};

_LIT(KTestNameRSocketServ002, "BT_ROMCONFIG_RSocketServ_002"); 
#endif //  CTESTSTEPBTROMCONFIGRSOCKETSERV002_H

// EOF
