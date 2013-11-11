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

#include <e32base.h>
#include "tsu_mdf_videocodecs_step.h"

const TInt KHeapSize = 5000000;

RTestStepVideoCodecs::RTestStepVideoCodecs()
	{
	iHeapSize = KHeapSize;
	}
	
RTestStepVideoCodecs::~RTestStepVideoCodecs()
	{
	}
	
TVerdict RTestStepVideoCodecs::OpenL()
	{
	iActiveScheduler = new(ELeave) CActiveScheduler;
	CleanupStack::PushL( iActiveScheduler );
	CActiveScheduler::Install(iActiveScheduler);
	CleanupStack::Pop();
	return EPass;
	}
	
void RTestStepVideoCodecs::CleanupAfterOpenFail()
	{
	}
	
void RTestStepVideoCodecs::Close()
	{
	delete iActiveScheduler;
	iActiveScheduler = NULL;
	}


void RTestStepVideoCodecs::SetVerdict(const TDesC& aText, TVerdict aVerdict)
	{	
	_LIT(KPass, "PASS");
	_LIT(KFail, "FAIL");
	INFO_PRINTF3(_L("Text: %S, Verdict: %S"), &aText, ((aVerdict == EPass)? &KPass :&KFail));
	// only record a failure if it is the first one
	if (iVerdict == EPass)
		{
		iVerdict = aVerdict;	
		}
		
	}


void RTestStepVideoCodecs::InfoMessage(const TDesC& aText)
	{	
	INFO_PRINTF2(_L("Text: %S"),&aText);
	}

TVerdict RTestStepVideoCodecs::Verdict()
	{
	return iVerdict;
	}


