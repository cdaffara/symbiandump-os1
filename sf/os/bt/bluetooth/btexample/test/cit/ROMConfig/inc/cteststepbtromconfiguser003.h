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

#ifndef CTESTSTEPBTROMCONFIGUSER003_H
#define CTESTSTEPBTROMCONFIGUSER003_H

#include "cteststepbtromconfigbase.h"
#include <es_sock.h>
#include <bt_sock.h>

class CTestStepBtRomConfigUser003 : public CTestStepBtRomConfigBase, public MBluetoothSynchronousLinkNotifier 
	{
public:
	static CTestStepBtRomConfigUser003* New(CTestServer& aParent);
	~CTestStepBtRomConfigUser003();
	TVerdict doTestStepL();
	
private:
	CTestStepBtRomConfigUser003(CTestServer& aParent);
	void TestSynchronousLinkConstructorL(TInt aTestSubCase);

// from MBluetoothSynchronousLinkNotifier
	void HandleSetupConnectionCompleteL(TInt aErr);
	void HandleDisconnectionCompleteL(TInt aErr);
	void HandleAcceptConnectionCompleteL(TInt aErr);
	void HandleSendCompleteL(TInt aErr);
	void HandleReceiveCompleteL(TInt aErr);
		
	RSocketServ iSocketServ;
	};

_LIT(KTestNameUser003, "BT_ROMCONFIG_USER_003"); 
#endif //  CTESTSTEPBTROMCONFIGUSER003_H

// EOF
