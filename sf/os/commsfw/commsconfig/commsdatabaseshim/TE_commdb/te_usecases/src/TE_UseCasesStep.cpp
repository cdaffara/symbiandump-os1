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
 * This contains CTE_UseCasesStep class which is
 * the base class for all the Encrpt test steps
 *
 */
#include "TE_UseCasesStep.h"


CTE_UseCasesStep::CTE_UseCasesStep() 
	{			
	}

CTE_UseCasesStep::~CTE_UseCasesStep()
	{ 
	}	

enum TVerdict CTE_UseCasesStep::Test(TInt aResult)
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

void CTE_UseCasesStep::Set(TGlobalData * aGlobals)
	{ 
	UserSvr::DllSetTls(67338722,aGlobals);
	}

TGlobalData* CTE_UseCasesStep::Get()
	{ 
	TGlobalData * d=(TGlobalData *)UserSvr::DllTls(67338722); 
	return d; 
	}




