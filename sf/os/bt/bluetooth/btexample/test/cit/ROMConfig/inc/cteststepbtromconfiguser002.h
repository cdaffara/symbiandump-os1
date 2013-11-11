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

#ifndef CTESTSTEPBTROMCONFIGUSER002_H
#define CTESTSTEPBTROMCONFIGUSER002_H

#include <test/testexecutestepbase.h>
#include <test/testexecuteserverbase.h>
#include "cteststepbtromconfigbase.h"
#include <es_sock.h>
#include <bt_sock.h>

class CTestStepBtRomConfigUser002 : public CTestStepBtRomConfigBase, 
									public MBluetoothSocketNotifier
	{
public:
	static CTestStepBtRomConfigUser002* New(CTestServer& aParent);
	~CTestStepBtRomConfigUser002();
	TVerdict doTestStepL();
	
private:
	CTestStepBtRomConfigUser002(CTestServer& aParent);
	void TestBluetoothSocketConstructorL(TInt aTestSubCase, 
										 TInt& aExpectedError);

	// from MBluetoothPhysicalLinksNotifier
	void HandleConnectCompleteL(TInt aErr);
	void HandleAcceptCompleteL(TInt aErr);
	void HandleShutdownCompleteL(TInt aErr);
	void HandleSendCompleteL(TInt aErr);
	void HandleReceiveCompleteL(TInt aErr);
	void HandleIoctlCompleteL(TInt aErr);
	void HandleActivateBasebandEventNotifierCompleteL(TInt, 
													  TBTBasebandEventNotification&);
	
	RSocketServ iSocketServ;
	};

_LIT(KTestNameUser002, "BT_ROMCONFIG_USER_002"); 
#endif //  CTESTSTEPBTROMCONFIGUSER002_H

// EOF
