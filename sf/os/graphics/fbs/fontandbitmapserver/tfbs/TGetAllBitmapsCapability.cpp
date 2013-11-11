// Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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

#include "../sfbs/UTILS.H"
#include "TGetAllBitmapsCapability.h"

CTGetAllBitmapsCapability::CTGetAllBitmapsCapability(CTestStep* aStep) : CTGraphicsBase(aStep)
	{		
	}

CTGetAllBitmapsCapability::~CTGetAllBitmapsCapability()
	{	
	}

void CTGetAllBitmapsCapability::RunTestCaseL(TInt aCurTestCase)
    {
    ((CTGetAllBitmapsCapabilityStep*)iStep)->SetTestStepID(KUnknownSYMTestCaseIDName);
	switch(aCurTestCase)
		{
	case 1:	
/**
    @SYMTestCaseID GRAPHICS-FBSERV-0622
*/
		((CTGetAllBitmapsCapabilityStep*)iStep)->SetTestStepID(_L("GRAPHICS-FBSERV-0622"));	
		TRAPD(err,GetAllBitmapsTestL());
		if(err != KErrNone)
			TEST(err == KErrPermissionDenied);
		break;
	case 2:
		((CTGetAllBitmapsCapabilityStep*)iStep)->SetTestStepID(KNotATestSYMTestCaseIDName);
		((CTGetAllBitmapsCapabilityStep*)iStep)->CloseTMSGraphicsStep();
		TestComplete();
		break;
		}
	((CTGetAllBitmapsCapabilityStep*)iStep)->RecordTestResultL();
    }

void CTGetAllBitmapsCapability::GetAllBitmapsTestL()
	{
	CFbsBitmap* bitmap = new (ELeave) CFbsBitmap;
	CleanupStack::PushL(bitmap);
	RArray <TInt> bitmapHandleArray;
	CleanupClosePushL(bitmapHandleArray);
	User::LeaveIfError(bitmap->GetAllBitmapHandles(bitmapHandleArray));
	CleanupStack::PopAndDestroy(2, bitmap);
	}

//--------------
__CONSTRUCT_STEP__(GetAllBitmapsCapability)
