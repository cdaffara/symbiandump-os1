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

#ifndef CTESTSTEPBTROMCONFIGRBTPHYLNKADAPT001_H
#define CTESTSTEPBTROMCONFIGRBTPHYLNKADAPT001_H

#include <test/testexecutestepbase.h>
#include <test/testexecuteserverbase.h>
#include "cteststepbtromconfigbase.h"
#include <es_sock.h>

class CTestStepBtRomConfigRBtPhyLnkAdapt001 : public CTestStepBtRomConfigBase
	{
public:
	static CTestStepBtRomConfigRBtPhyLnkAdapt001* New(CTestServer& aParent);
	~CTestStepBtRomConfigRBtPhyLnkAdapt001();
	TVerdict doTestStepL();
	
private:
	CTestStepBtRomConfigRBtPhyLnkAdapt001(CTestServer& aParent);
	TInt TestPhyLnkAdapt(const TInt);
	
	RSocketServ iSocketServ;	
	};

_LIT(KTestNameRBtPhyLnkAdapt001, "BT_ROMCONFIG_RBTPHYLNKADAPT001"); 
#endif //	 CTESTSTEPBTROMCONFIGRBTPHYLNKADAPT001_H

// EOF
