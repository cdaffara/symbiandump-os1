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

#ifndef CTESTSTEPBTROMCONFIGUSER001_H
#define CTESTSTEPBTROMCONFIGUSER001_H

#include "cteststepbtromconfigbase.h"
#include <es_sock.h>
#include <bt_sock.h>

class CTestStepBtRomConfigUser001 : public CTestStepBtRomConfigBase, public MBluetoothPhysicalLinksNotifier
	{
public:
	static CTestStepBtRomConfigUser001* New(CTestServer& aParent);
	~CTestStepBtRomConfigUser001();
	TVerdict doTestStepL();
	
private:
	CTestStepBtRomConfigUser001(CTestServer& aParent);
	void TestPhysicalLinksConstructorL(TInt aTestSubCase);

	// from MBluetoothPhysicalLinksNotifier
	void HandleCreateConnectionCompleteL(TInt aErr);
	void HandleDisconnectCompleteL(TInt aErr);
	void HandleDisconnectAllCompleteL(TInt aErr);
	
	RSocketServ iSocketServ;
	};

_LIT(KTestNameUser001, "BT_ROMCONFIG_USER_001"); 
#endif //  CTESTSTEPBTROMCONFIGUSER001_H

// EOF
