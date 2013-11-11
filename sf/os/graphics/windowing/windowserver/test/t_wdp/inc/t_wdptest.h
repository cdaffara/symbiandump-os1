// Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

/**
@file
@test
@internalComponent
*/

#ifndef __T_WDPTEST_H__
#define __T_WDPTEST_H__

#include <w32std.h>
#include <test/testexecutestepbase.h>

_LIT(KT_WDPTestStep,"T_WDPTestStep");

class CT_WDPTest : public CTestStep
	{
public:
	CT_WDPTest();
	~CT_WDPTest();
	// from CTestStep
	virtual enum TVerdict doTestStepPreambleL();
	virtual enum TVerdict doTestStepL();
	virtual enum TVerdict doTestStepPostambleL();

private:
	// Test cases
	TBool WServDefaultDataPagedL();
	
private:	// Data members
	TInt					iTestId;
	};

#endif	// __T_WDPTEST_H__
