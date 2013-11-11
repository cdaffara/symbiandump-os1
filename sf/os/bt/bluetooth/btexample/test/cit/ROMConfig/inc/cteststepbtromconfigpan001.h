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

#ifndef CTESTSTEPBTROMCONFIGPAN001_H
#define CTESTSTEPBTROMCONFIGPAN001_H

#include "cteststepbtromconfigbase.h"
#include <es_sock.h>

class CPan001TestTimeout : public CTimer
	{
public:	
	static CPan001TestTimeout* NewL();
	~CPan001TestTimeout();

private:	
	CPan001TestTimeout();
	void RunL();
	void DoCancel();
	};
	
class CActiveConnStart : public CActive
	{
public:
	static CActiveConnStart* NewL();
	~CActiveConnStart();

	void RunL();
	void DoCancel();

	void DoRConnectionStart();
		
private:
	CActiveConnStart();
	void ConstructL();
	
	RSocketServ iSocketServ;
	RConnection iConnection;
	};

class CTestStepBtRomConfigPan001 : public CTestStepBtRomConfigBase
	{
public:
	static CTestStepBtRomConfigPan001* New(CTestServer& aParent);
	~CTestStepBtRomConfigPan001();
	TVerdict doTestStepL();

private:
	CTestStepBtRomConfigPan001(CTestServer& aParent);
	
	CActiveConnStart* iConnStart;
	CPan001TestTimeout* iTestTimeout;
	};

_LIT(KTestNamePan001, "BT_ROMCONFIG_PAN_001"); 
#endif //  CTESTSTEPBTROMCONFIGPAN001_H

// EOF
