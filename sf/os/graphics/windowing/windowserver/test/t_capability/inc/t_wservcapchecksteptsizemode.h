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

#ifndef __T_WSERV_CAPCHECK_STEP_TSIZEMODE_H__
#define __T_WSERV_CAPCHECK_STEP_TSIZEMODE_H__

#include <test/testexecutestepbase.h>
#include "w32cmd.h"

_LIT(KT_WServCapCheckStepTSizeModePos, "WServCapCheckStepTSizeModePos");
_LIT(KT_WServCapCheckStepTSizeModeNeg, "WServCapCheckStepTSizeModeNeg");

_LIT(KExecutableNeg,	"c:\\sys\\bin\\t_sizemode_none.exe");
_LIT(KExecutablePos,	"c:\\sys\\bin\\t_sizemode_writedevicedata.exe");

/**
WServ Capability test commands
*/
class CT_WServCapCheckStepTSizeMode : public CTestStep
	{
public:
    // Constructor
	CT_WServCapCheckStepTSizeMode();
	virtual enum TVerdict doTestStepL();
	virtual enum TVerdict doTestStepPostambleL();
	enum TVerdict runTestCaseL(const TDesC& aExecutable, const TInt& aExitReason);
	};

class CT_WServCapCheckStepTSizeModePos : public CT_WServCapCheckStepTSizeMode
	{
public:
    // Constructor
	CT_WServCapCheckStepTSizeModePos();
	virtual enum TVerdict doTestStepL();
	};

class CT_WServCapCheckStepTSizeModeNeg : public CT_WServCapCheckStepTSizeMode
	{
public:
    // Constructor
	CT_WServCapCheckStepTSizeModeNeg();
	virtual enum TVerdict doTestStepL();
	};

#endif //__T_WSERV_CAPCHECK_STEP_TSIZEMODE_H__
