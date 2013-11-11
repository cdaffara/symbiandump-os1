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

#ifndef CTESTSTEPBTROMCONFIGREMCON001_H
#define CTESTSTEPBTROMCONFIGREMCON001_H

#include "cteststepbtromconfigbase.h"
#include <remconcoreapicontrollerobserver.h>

class CTestStepBtRomConfigRemCon001 : public CTestStepBtRomConfigBase, public MRemConCoreApiControllerObserver
	{
public:
	static CTestStepBtRomConfigRemCon001* New(CTestServer& aParent);
	~CTestStepBtRomConfigRemCon001();
	TVerdict doTestStepL();
	
	// MRemConCoreApiControllerObserver
	void MrccacoResponse(TRemConCoreApiOperationId aOperationId, TInt aError);
	
private:
	CTestStepBtRomConfigRemCon001(CTestServer& aParent);
	};

_LIT(KTestNameRemCon001, "BT_ROMCONFIG_REMCON_001"); 
#endif //  CTESTSTEPBTROMCONFIGREMCON001_H

// EOF
