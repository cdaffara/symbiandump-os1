// Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
 @internalComponent - Internal Symbian test code 
*/

#include "tsgimagecollectionmbx.h"

CTSgImageCollectionMbx::CTSgImageCollectionMbx()
	{
	INFO_PRINTF1(_L("Graphics resource component test - RSgImageCollection MBX Tests.\r\n"));
	}

CTSgImageCollectionMbx::~CTSgImageCollectionMbx()
	{
	}

/** 
Overrides of base class pure virtual
Our implementation only gets called if the base class doTestStepPreambleL() did
not leave. That being the case, the current test result value will be EPass.
@leave Gets system wide error code
@return TVerdict code
*/
TVerdict CTSgImageCollectionMbx::doTestStepL()
	{	
	return TestStepResult();
	}
