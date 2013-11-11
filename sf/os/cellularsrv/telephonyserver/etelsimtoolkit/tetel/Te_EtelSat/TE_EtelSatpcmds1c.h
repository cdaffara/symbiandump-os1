// Copyright (c) 2000-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#ifndef _TE_ETELSATPCMDS1C_H_
#define _TE_ETELSATPCMDS1C_H_

#include <etel.h>

class CTestSatPCmds1c : public CRSatTestStep
	{
public:
	CTestSatPCmds1c() ;
	~CTestSatPCmds1c(){} ;
	virtual enum TVerdict doTestStepL();

private:
	void ExtendedTest1(TRequestStatus& reqStatus);
	void ExtendedTest2L(TRequestStatus& reqStatus);
	void ExtendedTest3L(TRequestStatus& reqStatus);
	void ExtendedTest4L(TRequestStatus& reqStatus);
	void ExtendedTest5(TRequestStatus& reqStatus);
	void TestLaunchBrowser(TDes8* aLaunchBrowserPCmdPckg);
	};

#endif // _TE_ETELSATPCMDS1C_H_
