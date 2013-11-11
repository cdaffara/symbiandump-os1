//
// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
//

/**
 * @file TE_EncrptStep.cpp
 *
 * This contains CTE_EncrptStep class which is
 * the base class for all the Encrpt test steps
 *
 */
#include "TE_EncrptStep.h"

CTE_EncrptStep::CTE_EncrptStep()
	{	
	}

CTE_EncrptStep::~CTE_EncrptStep()
	{ 
	}	

void CTE_EncrptStep::ExpLogExtra(const TText8* aFile, TInt aLine, TInt aSeverity,
		TRefByValue<const TDesC> aFmt,...)
	{

	// Log to the suite's logger
	INFO_PRINTF5(_L("File: %S, line: %d, severity: %d, message: $S"), 
		aFile, aLine, aSeverity, aFmt);

	}

enum TVerdict CTE_EncrptStep::Test(TInt aResult)
	{	
		
	if (!aResult)
		{
			INFO_PRINTF1(KEGeneric);
         	SetTestStepResult(EFail);

			return TestStepResult();
		}
        SetTestStepResult(EPass);

	return TestStepResult();
	}


void CTE_EncrptStep::Set(TGlobalDbIds* aGlobals)
	{ UserSvr::DllSetTls(67338721,aGlobals); }

TGlobalDbIds* CTE_EncrptStep::Get()
	{ TGlobalDbIds * d=(TGlobalDbIds*)UserSvr::DllTls(67338721); return d; }





