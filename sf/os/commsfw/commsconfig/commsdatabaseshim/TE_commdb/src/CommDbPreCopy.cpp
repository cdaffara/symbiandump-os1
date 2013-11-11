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
//
// This contains copying cdbv3.dat to one temp dat file

// EPOC includes
#include <e32base.h>
#include <commdb.h>
#include <d32comm.h>

// Test system includes
#include "Teststepcommdb.h"
#include "CommDbPreCopy.h"

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
// Test Pre Copy
/* This New Class is added to move copyfile command from script file
   This test does a copy operation 	
*/
CCommDbPreCopy::CCommDbPreCopy()
	{
	// store the name of this test case
	SetTestStepName(_L("PreCopyTest"));
	}

CCommDbPreCopy::~CCommDbPreCopy()
	{
	}

// Copy Test Main Code
enum TVerdict CCommDbPreCopy::doTestStepL( void )
	{
	copyFileL(KCommDbName, KTempDb3);
        SetTestStepResult(EPass);	return TestStepResult();
	}


