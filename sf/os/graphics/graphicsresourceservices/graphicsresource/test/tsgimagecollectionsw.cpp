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

#include "tsgimagecollectionsw.h"

CTSgImageCollectionSw::CTSgImageCollectionSw()
	{
	INFO_PRINTF1(_L("Graphics resource component test - RSgImageCollection Sw Implementation Tests.\r\n"));
	}

CTSgImageCollectionSw::~CTSgImageCollectionSw()
	{
	}

/** 
Overrides of base class pure virtual
Our implementation only gets called if the base class doTestStepPreambleL() did
not leave. That being the case, the current test result value will be EPass.
@leave Gets system wide error code
@return TVerdict code
*/
TVerdict CTSgImageCollectionSw::doTestStepL()
	{	
	return TestStepResult();
	}
