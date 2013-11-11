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
 * @file TE_ConnPrefStep.cpp
 *
 * This contains CTE_ConnPrefStep class which is
 * the base class for all the ConnPref test steps
 *
 */
#include "TE_ConnPrefStep.h"


// Constructor
CTE_ConnPrefStep::CTE_ConnPrefStep() 
	{			
	}

// Destructor
CTE_ConnPrefStep::~CTE_ConnPrefStep()
	{ 
	}	

void CTE_ConnPrefStep::ExpLogExtra(const TText8* aFile, TInt aLine, TInt aSeverity,
		TRefByValue<const TDesC> aFmt,...)
	{
	//VA_LIST aList;
	//VA_START( aList, aFmt );


	// Log to the suite's logger
	INFO_PRINTF5(_L("File: %S, line: %d, severity: %d, message: $S"), 
		aFile, aLine, aSeverity, aFmt);

	//VA_END( aList ); 

	}

enum TVerdict CTE_ConnPrefStep::Test(TInt aResult)
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



